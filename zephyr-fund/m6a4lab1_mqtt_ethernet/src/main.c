/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2024 Charles Dias, VIRTUS/UFCG
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mqtt_ethernet, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/random.h>

#include <string.h>
#include <errno.h>

#include "config.h"

/* MQTT client context */
static struct mqtt_client client_ctx;
static struct sockaddr_storage broker;

/* Buffers for MQTT client. */
static uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
static uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

static struct pollfd fds[1];
static int nfds;

static bool connected;

static void prepare_fds(struct mqtt_client *client)
{
	if (client->transport.type == MQTT_TRANSPORT_NON_SECURE)
	{
		fds[0].fd = client->transport.tcp.sock;
	}

	fds[0].events = POLLIN;
	nfds = 1;
}

static int wait(int timeout)
{
	int ret = 0;

	if (nfds > 0)
	{
		ret = poll(fds, nfds, timeout);
		if (ret < 0)
		{
			LOG_ERR("poll error: %d", errno);
		}
	}

	return ret;
}

static int publish_topic(struct mqtt_client *client, const char *topic, const char *message, enum mqtt_qos qos)
{
	struct mqtt_publish_param param;

	param.message.topic.qos = qos;
	param.message.topic.topic.utf8 = (uint8_t *)topic;
	param.message.topic.topic.size =
		strlen(param.message.topic.topic.utf8);
	param.message.payload.data = (uint8_t *)message;
	param.message.payload.len =
		strlen(param.message.payload.data);
	param.message_id = sys_rand16_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

	return mqtt_publish(client, &param);
}

static int subscribe_topic(struct mqtt_client *client, const char *topic, enum mqtt_qos qos)
{
	struct mqtt_topic subscribe_topic = {
		.topic = {.utf8 = (uint8_t *)topic, .size = strlen(topic)},
		.qos = qos};

	const struct mqtt_subscription_list subscription_list = {
		.list = &subscribe_topic,
		.list_count = 1,
		.message_id = sys_rand16_get()};

	int err = mqtt_subscribe(client, &subscription_list);
	if (err != 0)
	{
		LOG_ERR("MQTT subscribe failed for topic %s: %d", topic, err);
	}
	else
	{
		LOG_INF("Subscribed to topic %s", topic);
	}
	return err;
}

static void on_mqtt_publish(struct mqtt_client *const client, const struct mqtt_evt *evt)
{
	int rc;
	uint8_t payload[APP_MQTT_BUFFER_SIZE];

	rc = mqtt_read_publish_payload(client, payload, APP_MQTT_BUFFER_SIZE);
	if (rc < 0)
	{
		LOG_ERR("Failed to read received MQTT payload [%d]", rc);
		return;
	}
	/* Adiciona o terminador null ao final do payload buffer */
	payload[rc] = '\0';

	LOG_INF("MQTT payload received!");
	LOG_INF("topic: '%s', payload: %s",
			evt->param.publish.message.topic.topic.utf8, payload);
}

void mqtt_evt_handler(struct mqtt_client *const client,
					  const struct mqtt_evt *evt)
{
	int err;

	switch (evt->type)
	{
	case MQTT_EVT_CONNACK:
		if (evt->result != 0)
		{
			LOG_ERR("MQTT connect failed %d", evt->result);
			break;
		}

		connected = true;
		LOG_INF("MQTT client connected!");

		break;

	case MQTT_EVT_DISCONNECT:
		LOG_INF("MQTT client disconnected %d", evt->result);

		connected = false;
		nfds = 0;

		break;

	case MQTT_EVT_PINGRESP:
		LOG_INF("PINGRESP packet");

		break;

	case MQTT_EVT_PUBACK:
		if (evt->result != 0)
		{
			LOG_ERR("MQTT PUBACK error %d", evt->result);
			break;
		}

		LOG_INF("PUBACK packet id: %u", evt->param.puback.message_id);

		break;

	case MQTT_EVT_PUBREC:
		if (evt->result != 0)
		{
			LOG_ERR("MQTT PUBREC error %d", evt->result);
			break;
		}

		LOG_INF("PUBREC packet id: %u", evt->param.pubrec.message_id);

		const struct mqtt_pubrel_param rel_param = {
			.message_id = evt->param.pubrec.message_id};

		err = mqtt_publish_qos2_release(client, &rel_param);
		if (err != 0)
		{
			LOG_ERR("Failed to send MQTT PUBREL: %d", err);
		}

		break;

	case MQTT_EVT_PUBREL:
		if (evt->result != 0)
		{
			LOG_ERR("MQTT PUBREL error [%d]", evt->result);
			break;
		}

		LOG_INF("PUBREL packet ID: %u", evt->param.pubrel.message_id);

		const struct mqtt_pubcomp_param rec_param = {
			.message_id = evt->param.pubrel.message_id};

		err = mqtt_publish_qos2_complete(client, &rec_param);
		if (err != 0)
		{
			LOG_ERR("Failed to send MQTT PUBCOMP: %d", err);
		}

		break;

	case MQTT_EVT_PUBCOMP:
		if (evt->result != 0)
		{
			LOG_ERR("MQTT PUBCOMP error %d", evt->result);
			break;
		}

		LOG_INF("PUBCOMP packet id: %u", evt->param.pubcomp.message_id);

		break;

	case MQTT_EVT_SUBACK:
		if (evt->result == 0x80)
		{
			LOG_ERR("MQTT SUBACK error [%d]", evt->result);
			break;
		}

		LOG_INF("SUBACK packet ID: %d", evt->param.suback.message_id);

		break;

	case MQTT_EVT_PUBLISH:
		const struct mqtt_publish_param *p = &evt->param.publish;

		if (p->message.topic.qos == MQTT_QOS_1_AT_LEAST_ONCE)
		{
			const struct mqtt_puback_param ack_param = {
				.message_id = p->message_id};
			mqtt_publish_qos1_ack(client, &ack_param);
		}
		else if (p->message.topic.qos == MQTT_QOS_2_EXACTLY_ONCE)
		{
			const struct mqtt_pubrec_param rec_param = {
				.message_id = p->message_id};
			mqtt_publish_qos2_receive(client, &rec_param);
		}

		on_mqtt_publish(client, evt);

		break;

	default:
		LOG_INF("Unhandled MQTT event type: %d", evt->type);
		break;
	}
}

static void broker_init(void)
{
	int err;
	struct addrinfo *result;

	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM};

	err = getaddrinfo(CONFIG_MQTT_BROKER_HOSTNAME, NULL, &hints, &result);
	if (err != 0)
	{
		LOG_ERR("getaddrinfo failed: %d, %s", err, gai_strerror(err));
		return;
	}

	if (result == NULL)
	{
		LOG_ERR("Error, address not found");
		return;
	}

	struct sockaddr_in *broker4 = ((struct sockaddr_in *)&broker);
	broker4->sin_addr.s_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(CONFIG_MQTT_BROKER_PORT);

	char ipv4_addr[NET_IPV4_ADDR_LEN];
	inet_ntop(AF_INET, &broker4->sin_addr.s_addr, ipv4_addr, sizeof(ipv4_addr));
	LOG_INF("IPv4 address of MQTT broker found %s", ipv4_addr);

	freeaddrinfo(result);
}

static void client_init(struct mqtt_client *client)
{
	mqtt_client_init(client);

	broker_init();

	/* MQTT client configuration */
	client->broker = &broker;
	client->evt_cb = mqtt_evt_handler;
	client->client_id.utf8 = (uint8_t *)CONFIG_MQTT_CLIENT_ID;
	client->client_id.size = strlen(client->client_id.utf8);
	client->password = NULL;
	client->user_name = NULL;
	client->protocol_version = MQTT_VERSION_3_1_1;

	/* MQTT buffers configuration */
	client->rx_buf = rx_buffer;
	client->rx_buf_size = sizeof(rx_buffer);
	client->tx_buf = tx_buffer;
	client->tx_buf_size = sizeof(tx_buffer);

	/* MQTT transport configuration */
	client->transport.type = MQTT_TRANSPORT_NON_SECURE;
}

static int try_to_connect(struct mqtt_client *client)
{
	int rc, i = 0;

	while (i++ < APP_CONNECT_TRIES && !connected)
	{

		client_init(client);

		rc = mqtt_connect(client);
		if (rc != 0)
		{
			LOG_WRN("Failed to connect to MQTT broker: %d", rc);
			k_sleep(K_MSEC(APP_SLEEP_MSECS));
			continue;
		}

		prepare_fds(client);

		if (wait(APP_CONNECT_TIMEOUT_MS))
		{
			mqtt_input(client);
		}

		if (!connected)
		{
			mqtt_abort(client);
		}
	}

	if (connected)
	{
		return 0;
	}

	return -EINVAL;
}

static int process_mqtt_and_sleep(struct mqtt_client *client, int timeout)
{
	int64_t remaining = timeout;
	int64_t start_time = k_uptime_get();
	int rc;

	while (remaining > 0 && connected)
	{
		if (wait(remaining))
		{
			rc = mqtt_input(client);
			if (rc != 0)
			{
				LOG_WRN("Failed to process mqtt_input: %d", rc);
				return rc;
			}
		}

		rc = mqtt_live(client);
		if (rc != 0 && rc != -EAGAIN)
		{
			LOG_WRN("Failed to process mqtt_live: %d", rc);
			return rc;
		}
		else if (rc == 0)
		{
			rc = mqtt_input(client);
			if (rc != 0)
			{
				LOG_WRN("Failed to process mqtt_input: %d", rc);
				return rc;
			}
		}

		remaining = timeout + start_time - k_uptime_get();
	}

	return 0;
}

static int start_mqtt_app(void)
{
	int i, rc;

	LOG_INF("attempting to connect: ");
	rc = try_to_connect(&client_ctx);
	if (rc != 0)
	{
		LOG_ERR("Failed to connect to MQTT broker: %d", rc);
		return 1;
	}
	LOG_INF("Connected to MQTT broker");

	rc = mqtt_ping(&client_ctx);
	if (rc != 0)
	{
		LOG_ERR("Failed to ping broker: %d", rc);
		return 1;
	}
	LOG_INF("Sent MQTT PING");

	rc = subscribe_topic(&client_ctx, CONFIG_MQTT_SUB_TOPIC, MQTT_QOS_0_AT_MOST_ONCE);
	if (rc != 0)
	{
		LOG_ERR("Failed to subscribe to topic after connection: %d", rc);
	}

	i = 0;
	while (i++ < CONFIG_NET_SAMPLE_APP_MAX_ITERATIONS && connected)
	{
		rc = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
		if (rc != 0)
		{
			LOG_WRN("Failed to process MQTT and sleep: %d", rc);
			break;
		}

		rc = publish_topic(&client_ctx, CONFIG_MQTT_PUB_TOPIC, "Hello from Zephyr!", MQTT_QOS_0_AT_MOST_ONCE);
		if (rc != 0)
		{
			LOG_WRN("Failed to publish MQTT message: %d", rc);
			break;
		}
		LOG_INF("Published MQTT message");
	}

	rc = mqtt_disconnect(&client_ctx);
	LOG_INF("mqtt_disconnect: %d", rc);

	LOG_INF("Finished MQTT sample");

	return rc;
}

int main(void)
{
	LOG_INF("Starting MQTT Ethernet sample");

	LOG_INF("Waiting %d seconds for network to be ready...",
			CONFIG_ETHERNET_INIT_WAIT_TIME);
	k_sleep(K_SECONDS(CONFIG_ETHERNET_INIT_WAIT_TIME));

	start_mqtt_app();

	return 0;
}
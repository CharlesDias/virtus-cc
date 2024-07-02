#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	int count = 0;

	while(1) 
    {
		printk("%3d - VIRTUS-CC Hello World!\n\r", count++);
		k_msleep(1000);
	}
}



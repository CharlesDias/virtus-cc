# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(lab1_threads)

target_sources(app PRIVATE src/main.c)

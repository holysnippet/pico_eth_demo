#ifndef __MQTT_TEST_H__
#define __MQTT_TEST_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_opts.h"

#include "lwip/ip.h"

void example_publish_str(mqtt_client_t *client, char *name, char *value, void *arg);
void example_publish_uint(mqtt_client_t *client, char *name, uint32_t value, void *arg);
void example_publish_bint(mqtt_client_t *client, char *name, uint64_t value, void *arg);
void example_do_connect(mqtt_client_t *client);

extern bool is_mqtt_online;

#endif
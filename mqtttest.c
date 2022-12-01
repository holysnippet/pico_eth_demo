#include "mqtttest.h"

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);

void example_do_connect(mqtt_client_t *client)
{
    ip4_addr_t mqtt_server_ip;
    IP4_ADDR(&mqtt_server_ip, 192, 168, 1, 20);

    struct mqtt_connect_client_info_t ci;
    err_t err;

    /* Setup an empty client info structure */
    memset(&ci, 0, sizeof(ci));

    /* Minimal amount of information required is client identifier, so set it here */
    ci.client_id = "lwip_test";

    /* Initiate client and connect to server, if this fails immediately an error code is returned
       otherwise mqtt_connection_cb will be called with connection result after attempting
       to establish a connection with the server.
       For now MQTT version 3.1.1 is always used */

    err = mqtt_client_connect(client, &mqtt_server_ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);

    /* For now just print the result code if something goes wrong */
    if (err != ERR_OK)
    {
        printf("mqtt_connect return %d\n", err);
    }
}

/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
    if (result != ERR_OK)
    {
        printf("Publish result: %d\n", result);
    }
}

void example_publish_str(mqtt_client_t *client, char *name, char *value, void *arg)
{
    char top[] = "stack_diag/pico2/";
    char nam[64];
    strcpy(nam, top);
    strcat(nam, name);

    err_t err;
    u8_t qos = 0;    /* 0 1 or 2, see MQTT specification */
    u8_t retain = 0; /* No don't retain such crappy payload... */
    err = mqtt_publish(client, nam, value, strlen(value), qos, retain, mqtt_pub_request_cb, arg);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

void example_publish_bint(mqtt_client_t *client, char *name, uint64_t value, void *arg)
{
    char top[] = "stack_diag/pico2/";
    char nam[64], pay[64];
    strcpy(nam, top);
    strcat(nam, name);

    sprintf(pay, "%"PRIu64"", value);

    err_t err;
    u8_t qos = 0;    /* 0 1 or 2, see MQTT specification */
    u8_t retain = 0; /* No don't retain such crappy payload... */
    err = mqtt_publish(client, nam, pay, strlen(pay), qos, retain, mqtt_pub_request_cb, arg);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

void example_publish_uint(mqtt_client_t *client, char *name, uint32_t value, void *arg)
{
    char top[] = "stack_diag/pico2/";
    char nam[64], pay[64];
    strcpy(nam, top);
    strcat(nam, name);

    sprintf(pay, "%"PRIu32"", value);

    err_t err;
    u8_t qos = 0;    /* 0 1 or 2, see MQTT specification */
    u8_t retain = 0; /* No don't retain such crappy payload... */
    err = mqtt_publish(client, nam, pay, strlen(pay), qos, retain, mqtt_pub_request_cb, arg);
    if (err != ERR_OK)
    {
        printf("Publish err: %d\n", err);
    }
}

static int inpub_id;
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

    /* Decode topic string into a user defined reference */
    if (strcmp(topic, "print_payload") == 0)
    {
        inpub_id = 0;
    }
    else if (topic[0] == 'A')
    {
        /* All topics starting with 'A' might be handled at the same way */
        inpub_id = 1;
    }
    else
    {
        /* For all other topics */
        inpub_id = 2;
    }
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

    if (flags & MQTT_DATA_FLAG_LAST)
    {
        /* Last fragment of payload received (or whole part if payload fits receive buffer
           See MQTT_VAR_HEADER_BUFFER_LEN)  */

        /* Call function or do action depending on reference, in this case inpub_id */
        if (inpub_id == 0)
        {
            /* Don't trust the publisher, check zero termination */
            if (data[len - 1] == 0)
            {
                printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
            }
        }
        else if (inpub_id == 1)
        {
            /* Call an 'A' function... */
        }
        else
        {
            printf("mqtt_incoming_data_cb: Ignoring payload...\n");
        }
    }
    else
    {
        /* Handle fragmented payload, store in buffer, write to file or whatever */
    }
}

static void mqtt_sub_request_cb(void *arg, err_t result)
{
    /* Just print the result code here for simplicity,
       normal behaviour would be to take some action if subscribe fails like
       notifying user, retry subscribe or disconnect from server */
    printf("Subscribe result: %d\n", result);
}

bool is_mqtt_online = false;
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    err_t err;
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("mqtt_connection_cb: Successfully connected\n");

        /* Setup callback for incoming publish requests */
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

        is_mqtt_online = true;

        /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
        /*
        err = mqtt_subscribe(client, "subtopic", 1, mqtt_sub_request_cb, arg);

        if (err != ERR_OK)
        {
            printf("mqtt_subscribe return: %d\n", err);
        }*/
    }
    else
    {
        is_mqtt_online = false;
        printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);

        /* Its more nice to be connected, so try to reconnect */
        example_do_connect(client);
    }
}

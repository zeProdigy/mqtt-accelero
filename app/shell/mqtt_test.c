#include <string.h>

#include "lwip/apps/mqtt.h"
#include "system/dbgio.h"


static mqtt_client_t *mqtt_client;
static uint8_t test_counter = 0;

static ip_addr_t ip_addr = IPADDR4_INIT_BYTES(192, 168, 0, 1);

static const struct mqtt_connect_client_info_t mqtt_client_info = {
    "id_1",
    "dev_board", /* user */
    NULL, /* pass */
    100,  /* keep alive */
    NULL, /* will_topic */
    NULL, /* will_msg */
    0,    /* will_qos */
    0     /* will_retain */
};


static void incoming_data_cb(void *arg,
                             const uint8_t *data, uint16_t len,
                             uint8_t flags)
{
    const struct mqtt_connect_client_info_t* ci =
        (const struct mqtt_connect_client_info_t *)arg;

    CONSOLE_MEMDUMP(data, len, "MQTT client \"%s\" in data cb: len %d, flags %d",
        ci->client_id, len, flags);
}


static void incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
    const struct mqtt_connect_client_info_t* ci =
        (const struct mqtt_connect_client_info_t *)arg;

    CONSOLE_LOG("MQTT client \"%s\" publish cb: topic %s, len %d",
        ci->client_id, topic, tot_len);
}


static void request_cb(void *arg, err_t err)
{
    const struct mqtt_connect_client_info_t* ci =
        (const struct mqtt_connect_client_info_t *)arg;

    CONSOLE_LOG("MQTT client \"%s\" request cb: err %d\n",
        ci->client_id, (int)err);
}


static void connection_cb(mqtt_client_t *client,
                          void *arg,
                          mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t* ci =
        (const struct mqtt_connect_client_info_t *)arg;

    CONSOLE_LOG("MQTT client \"%s\" connection cb: status %d\n",
        ci->client_id, (int)status);

    if (status == MQTT_CONNECT_ACCEPTED) {
        mqtt_subscribe(client,
                       "$SYS/broker/version", 1,
                       request_cb, LWIP_CONST_CAST(void*, ci));
    }
}


int mqtt_test(int argc, char *argv[])
{
    const char *cmd = argv[1];

    CONSOLE_LOG("cmd: %s", cmd);

    if (strcmp(cmd, "init") == 0) {
        mqtt_client = mqtt_client_new();
        if (mqtt_client == NULL) {
            CONSOLE_LOG("Can't alloc mqtt client struct");
            return 1;
        }

        mqtt_set_inpub_callback(mqtt_client,
                                incoming_publish_cb,
                                incoming_data_cb,
                                LWIP_CONST_CAST(void*, &mqtt_client_info));

        mqtt_client_connect(mqtt_client,
                            &ip_addr, MQTT_PORT,
                            connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),
                            &mqtt_client_info);
    }
    else if (strcmp(cmd, "pub") == 0) {
        test_counter++;
        char payload[4] = {0};
        sprintf(payload, "%3d", test_counter);
        mqtt_publish(mqtt_client, "counter",
                     &payload, strlen(payload),
                     1, 1,
                     request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info));
    } else {
        CONSOLE_LOG("Unsupported cmd");
    }

    return 0;
}

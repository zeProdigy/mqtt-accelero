#include <string.h>
#include <assert.h>

#include "lwip/apps/mqtt.h"
#include "system/accelero.h"
#include "system/dbgio.h"
#include "osal/osal.h"


static mqtt_client_t *mqtt_client;
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

static osal_semaphore_t conn_successfull;


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

    if (err) {
        CONSOLE_LOG("MQTT client \"%s\" request cb: err %d",
            ci->client_id, (int)err);
    }
}


static void connection_cb(mqtt_client_t *client,
                          void *arg,
                          mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t* ci =
        (const struct mqtt_connect_client_info_t *)arg;

    CONSOLE_LOG("MQTT client \"%s\" connection cb: status %d",
        ci->client_id, (int)status);

    if (status == MQTT_CONNECT_ACCEPTED) {
        mqtt_subscribe(client,
                       "$SYS/broker/version", 1,
                       request_cb, LWIP_CONST_CAST(void*, ci));

        osal_semaphore_release(conn_successfull);
    }
}


void mqtt_server_connection(const void *args)
{
    int ret;
    uint32_t timeout = 5000;

    conn_successfull = osal_semaphore_create();
    if (conn_successfull == NULL) {
        CONSOLE_ERROR("Can't create sem");
        goto stop;
    }

    if (accelero_init()) {
        goto stop;
    }

    mqtt_client = mqtt_client_new();
    if (mqtt_client == NULL) {
        CONSOLE_LOG("Can't alloc mqtt client struct");
        goto stop;
    }

    mqtt_set_inpub_callback(mqtt_client,
                            incoming_publish_cb,
                            incoming_data_cb,
                            LWIP_CONST_CAST(void*, &mqtt_client_info));

    mqtt_client_connect(mqtt_client,
                        &ip_addr, MQTT_PORT,
                        connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),
                        &mqtt_client_info);

    ret = osal_semaphore_wait(conn_successfull, timeout);
    if (ret) {
        CONSOLE_ERROR("Can't connect to mqtt server: timeout reached");
        goto stop;
    }

    while(true) {
        accelero_tilt_t tilt;
        char buf[64];
        
        accelero_get_tilt(&tilt);
        
        snprintf(buf,sizeof(buf), "%f;%f", tilt.pitch, tilt.roll);

        ret = mqtt_publish(mqtt_client, "telemetry/tilt",
                           &buf, strlen(buf),
                           1, 0,
                           request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info));

        if (ret) {
            CONSOLE_ERROR("mqtt_publish err: %d", ret);
            goto stop;
        }
        
        osal_thread_sleep(100);
    }

stop:
    CONSOLE_WARN("mqtt_server_connection thread terminated");
    osal_thread_stop(osal_thread_self());
}

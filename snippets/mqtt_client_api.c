/*
 * MQTT client API example with ESP device.
 *
 * Once device is connected to network,
 * it will try to connect to mosquitto test server and start the MQTT.
 *
 * If successfully connected, it will publish data to "esp8266_mqtt_topic" topic every x seconds.
 *
 * To check if data are sent, you can use mqtt-spy PC software to inspect
 * test.mosquitto.org server and subscribe to publishing topic
 */

#include "esp/apps/esp_mqtt_client_api.h"
#include "mqtt_client_api.h"
#include "esp/esp_mem.h"

/**
 * \brief           Client ID is structured from ESP station MAC address
 */
static char
mqtt_client_id[13];

/**
 * \brief           Connection information for MQTT CONNECT packet
 */
static const esp_mqtt_client_info_t
mqtt_client_info = {
    .id = mqtt_client_id,                       /* The only required field for connection! */
    
    .keep_alive = 10,
    // .user = "test_username",
    // .pass = "test_password",
};

/**
 * \brief           MQTT client API thread
 */
void
mqtt_client_api_thread(void const* arg) {
    esp_mqtt_client_api_p client;
    esp_mqtt_conn_status_t conn_status;
    esp_mqtt_client_api_buf_p buf;
    espr_t res;

    /* Create new MQTT API */
    client = esp_mqtt_client_api_new(256, 128);
    if (client == NULL) {
        goto terminate;
    }

    while (1) {
        /* Make a connection */
        printf("Joining MQTT server\r\n");

        /* Try to join */
        conn_status = esp_mqtt_client_api_connect(client, "test.mosquitto.org", 1883, &mqtt_client_info);
        if (conn_status == ESP_MQTT_CONN_STATUS_ACCEPTED) {
            printf("Connected and accepted!\r\n");
            printf("Client is ready to subscribe and publish to new messages\r\n");
        } else {
            printf("Connect API response: %d\r\n", (int)conn_status);
            esp_delay(5000);
            continue;
        }

        /* Subscribe to topics */
        if (esp_mqtt_client_api_subscribe(client, "esp8266_mqtt_topic", ESP_MQTT_QOS_AT_LEAST_ONCE) == espOK) {
            printf("Subscribed to esp8266_mqtt_topic\r\n");
        } else {
            printf("Problem subscribing to topic!\r\n");
        }

        while (1) {
            /* Receive MQTT packet with 1000ms timeout */
            res = esp_mqtt_client_api_receive(client, &buf, 1000);
            if (res == espOK) {
                if (buf != NULL) {
                    printf("Publish received!\r\n");
                    printf("Topic: %s, payload: %s\r\n", buf->topic, buf->payload);
                    esp_mqtt_client_api_buf_free(buf);
                    buf = NULL;
                }
            } else if (res == espCLOSED) {
                printf("MQTT connection closed!\r\n");
                break;
            } else if (res == espTIMEOUT) {
                /* Receive timeout */
                /* printf("Timeout\r\n"); */
            }
        }
    }

terminate: 
    esp_mqtt_client_api_delete(client);
    printf("MQTT client thread terminate\r\n");
    esp_sys_thread_terminate(NULL);
}

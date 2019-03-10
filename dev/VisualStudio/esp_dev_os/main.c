// esp_dev_os.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "esp/esp.h"
#include "esp/apps/esp_mqtt_client_api.h"
#include "esp/apps/esp_cayenne.h"

#include "mqtt_client.h"
#include "mqtt_client_api.h"
#include "http_server.h"
#include "station_manager.h"
#include "netconn_client.h"
#include "netconn_server.h"
#include "netconn_server_1thread.h"
#include "string.h"
#include "esp/esp_timeout.h"

static void main_thread(void* arg);
DWORD main_thread_id;

static espr_t esp_evt(esp_evt_t* evt);
static espr_t esp_conn_evt(esp_evt_t* evt);

esp_sta_info_ap_t connected_ap_info;

uint32_t ping_time;

#define safeprintf          printf

/**
 * \brief           MQTT client info for server
 */
const esp_mqtt_client_info_t
cayenne_mqtt_client_info = {
    .id = "408793d0-3810-11e9-86b5-4fe3d2557533",

    .user = "8a215f70-a644-11e8-ac49-e932ed599553",
    .pass = "26aa943f702e5e780f015cd048a91e8fb54cca28",

    .keep_alive = 10,
};

esp_mqtt_client_api_p cayenne_client;
esp_cayenne_t cayenne;

/**
 * \brief           Program entry point
 */
int
main() {
    printf("App start!\r\n");

    /* Create start main thread */
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main_thread, NULL, 0, &main_thread_id);

    /* Do nothing at this point but do not close the program */
	while (1) {
        esp_delay(1000);
	}
}


void
time_thread_func(void* const arg) {
    while (1) {
        uint32_t time = esp_sys_now();
        printf("Time: %u d, %u h, %u m, %u s, %u ms, Rem seconds: %u\r\n",
            time / 1000 / 3600 / 24,
            time / 1000 / 3600,
            time / 1000 / 60,
            time / 1000,
            time,
            (0xFFFFFFFF / 1000) - (time / 1000)
        );
        esp_delay(1000);
    }
}

/**
 * \brief           Main thread for init purposes
 */
static void
main_thread(void* arg) {

    /* Init ESP library */
    esp_init(esp_evt, 1);

    /* Start thread to toggle device present */
    //esp_sys_thread_create(NULL, "device_present", (esp_sys_thread_fn)esp_device_present_toggle, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "time", (esp_sys_thread_fn)time_thread_func, NULL, 0, ESP_SYS_THREAD_PRIO);

    /*
     * Try to connect to preferred access point
     *
     * Follow function implementation for more info
     * on how to setup preferred access points for fast connection
     */
    //start_access_point_scan_and_connect_procedure();
    //esp_sys_thread_terminate(NULL);
    connect_to_preferred_access_point(1);

    /*
     * Check if device has set IP address
     *
     * This should always pass
     */
    if (esp_sta_has_ip()) {
        esp_ip_t ip;
        esp_sta_copy_ip(&ip, NULL, NULL);
        printf("Connected to WIFI!\r\n");
        printf("Device IP: %d.%d.%d.%d\r\n", ip.ip[0], ip.ip[1], ip.ip[2], ip.ip[3]);
    }

    //while (1) {
        //esp_ping("majerle.eu", &ping_time, ping_fn, "majerle.eu", 0);
        //    esp_delay(2000);
    //}

    /* Start server on port 80 */
    //http_server_start();
    //esp_sys_thread_create(NULL, "netconn_client", (esp_sys_thread_fn)netconn_client_thread, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "netconn_server", (esp_sys_thread_fn)netconn_server_thread, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "netconn_server_single", (esp_sys_thread_fn)netconn_server_1thread_thread, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "mqtt_client", (esp_sys_thread_fn)mqtt_client_thread, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "mqtt_client_api", (esp_sys_thread_fn)mqtt_client_api_thread, NULL, 0, ESP_SYS_THREAD_PRIO);
    //esp_sys_thread_create(NULL, "mqtt_client_api_cayenne", (esp_sys_thread_fn)mqtt_client_api_cayenne_thread, NULL, 0, ESP_SYS_THREAD_PRIO);

    if (esp_cayenne_create(&cayenne, &cayenne_mqtt_client_info) != espOK) {
        printf("Cannot create new cayenne instance!\r\n");
    } else {
        printf("Waiting for connection to cayenne...\r\n");
        while (!esp_mqtt_client_api_is_connected(cayenne.api_c)) {
            esp_delay(1000);
        }
        printf("Connected to cayenne...\r\n");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_SYS_MODEL, ESP_CAYENNE_NO_CHANNEL, NULL, NULL, "IoT Board for CayenneAPI");

        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_SYS_VERSION, ESP_CAYENNE_NO_CHANNEL, NULL, NULL, "v1.0");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_SYS_CPU_MODEL, ESP_CAYENNE_NO_CHANNEL, NULL, NULL, "ARM Cortex-M4");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_SYS_CPU_SPEED, ESP_CAYENNE_NO_CHANNEL, NULL, NULL, "180000000");

        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 30, "temp", "c", "13");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 31, "temp", "f", "13");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 32, "voltage", "mv", "123");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 34, "soil_w_ten", "kpa", "16");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 35, "rel_hum", "p", "29");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 36, "analog_actuator", NULL, "255");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 37, "digital_actuator", "d", "1");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 38, "analog_sensor", NULL, "255");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 39, "digital_sensor", "d", "0");
        esp_cayenne_publish_data(&cayenne, ESP_CAYENNE_TOPIC_DATA, 40, "digital_sensor", "d", "1");
    }

    /* Terminate thread */
    esp_sys_thread_terminate(NULL);
}

/**
 * \brief           Global ESP event function callback
 * \param[in]       evt: Event information
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
static espr_t
esp_evt(esp_evt_t* evt) {
    switch (evt->type) {
        case ESP_EVT_INIT_FINISH: {
            /* Device is not present on init */
            //esp_device_set_present(0, NULL, NULL, 0);
            break;
        }
        case ESP_EVT_RESET: {
            if (esp_evt_reset_get_result(evt) == espOK) {
                printf("Reset sequence successful!\r\n");
            } else {
                printf("Reset sequence error!\r\n");
            }
            break;
        }
        case ESP_EVT_RESTORE: {
            if (esp_evt_restore_get_result(evt) == espOK) {
                printf("Restore sequence successful!\r\n");
            } else {
                printf("Restore sequence error!\r\n");
            }
            break;
        }
        case ESP_EVT_AT_VERSION_NOT_SUPPORTED: {
            esp_sw_version_t v_min, v_curr;

            esp_get_min_at_fw_version(&v_min);
            esp_get_current_at_fw_version(&v_curr);

            printf("Current ESP8266 AT version is not supported by library\r\n");
            printf("Minimum required AT version is: %d.%d.%d\r\n", (int)v_min.major, (int)v_min.minor, (int)v_min.patch);
            printf("Current AT version is: %d.%d.%d\r\n", (int)v_curr.major, (int)v_curr.minor, (int)v_curr.patch);
            break;
        }
        case ESP_EVT_WIFI_GOT_IP: {
            printf("WIFI GOT IP, get access point informatioN!\r\n");
            esp_sta_get_ap_info(&connected_ap_info, NULL, NULL, 0);
            break;
        }
        case ESP_EVT_WIFI_CONNECTED: {
            printf("WIFI CONNECTED!\r\n");
            break;
        }
        case ESP_EVT_WIFI_DISCONNECTED: {
            printf("WIFI DISCONNECTED!\r\n");
            break;
        }
        case ESP_EVT_STA_INFO_AP: {
            printf("SSID: %s, ch: %d, rssi: %d\r\n",
                esp_evt_sta_info_ap_get_ssid(evt),
                (int)esp_evt_sta_info_ap_get_channel(evt),
                (int)esp_evt_sta_info_ap_get_rssi(evt)
            );
            break;
        }
#if ESP_CFG_MODE_ACCESS_POINT
        case ESP_EVT_AP_CONNECTED_STA: {
            esp_mac_t* mac = esp_evt_ap_connected_sta_get_mac(evt);
            printf("New station connected to ESP's AP with MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);
            break;
        }
        case ESP_EVT_AP_DISCONNECTED_STA: {
            esp_mac_t* mac = esp_evt_ap_disconnected_sta_get_mac(evt);
            printf("Station disconnected from ESP's AP with MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);
            break;
        }
        case ESP_EVT_AP_IP_STA: {
            esp_mac_t* mac = esp_evt_ap_ip_sta_get_mac(evt);
            esp_ip_t* ip = esp_evt_ap_ip_sta_get_ip(evt);
            printf("Station received IP address from ESP's AP with MAC: %02X:%02X:%02X:%02X:%02X:%02X and IP: %d.%d.%d.%d\r\n",
                mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5],
                ip->ip[0], ip->ip[1], ip->ip[2], ip->ip[3]);
            break;
        }
#endif /* ESP_CFG_MODE_ACCESS_POINT */
        default: break;
    }
	return espOK;
}

static espr_t
esp_conn_evt(esp_evt_t* evt) {
    static char data[] = "test data string\r\n";
    esp_conn_p conn;

    conn = esp_conn_get_from_evt(evt);

    switch (evt->type) {
        case ESP_EVT_CONN_ACTIVE: {
            printf("Connection active!\r\n");
            printf("Send API call: %d\r\n", (int)esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0));
            printf("Send API call: %d\r\n", (int)esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0));
            printf("Send API call: %d\r\n", (int)esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0));
            printf("Close API call: %d\r\n", (int)esp_conn_close(conn, 0));
            printf("Send API call: %d\r\n", (int)esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0));
            printf("Close API call: %d\r\n", (int)esp_conn_close(conn, 0));

            /*
            esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0);
            esp_conn_send(conn, data, sizeof(data) - 1, NULL, 0);
            */
            break;
        }
        case ESP_EVT_CONN_SEND: {
            espr_t res = esp_evt_conn_send_get_result(evt);
            if (res == espOK) {
                printf("Connection data sent!\r\n");
            } else {
                printf("Connect data send error!\r\n");
            }
            break;
        }
        case ESP_EVT_CONN_RECV: {
            esp_pbuf_p pbuf = esp_evt_conn_recv_get_buff(evt);
            esp_conn_p conn = esp_evt_conn_recv_get_conn(evt);
            printf("\r\nConnection data received: %d / %d bytes\r\n",
                (int)esp_pbuf_length(pbuf, 1),
                (int)esp_conn_get_total_recved_count(conn)
            );
            esp_conn_recved(conn, pbuf);
            break;
        }
        case ESP_EVT_CONN_CLOSED: {
            printf("Connection closed!\r\n");
            //esp_conn_start(NULL, ESP_CONN_TYPE_TCP, "majerle.eu", 80, NULL, esp_conn_evt, 0);
            break;
        }
        case ESP_EVT_CONN_ERROR: {
            printf("Connection error!\r\n");
            break;
        }
        default: break;
    }
    return espOK;
}

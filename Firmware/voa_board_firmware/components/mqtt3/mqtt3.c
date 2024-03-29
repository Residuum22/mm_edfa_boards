#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt3.h"

#include "cJSON.h"

#include "led_indicator.h"
#include "led_indicator_blink_default.h"
#include "led_indicator_voa.h"

static const char *TAG = "MQTT_EXAMPLE";

extern QueueHandle_t voa_attenuation_queue;
#if CONFIG_USE_EPS_VAR
extern QueueHandle_t voa_eps_queue;
#endif

static cJSON *settings_json;

static esp_mqtt_client_handle_t client;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        voa_indicator_set_state(BLINK_CONNECTED);
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // send data to topic
        msg_id = esp_mqtt_client_subscribe(client, "/voa_attenuation", 2);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        voa_indicator_set_state(BLINK_CONNECTING);
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (settings_json != NULL)
        {
            cJSON_Delete(settings_json);
        }
        settings_json = cJSON_Parse(event->data);
#if CONFIG_USE_EPS_VAR
        if (cJSON_HasObjectItem(settings_json, "voa_eps"))
        {
            uint8_t voa_eps = (uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(settings_json, "voa_eps"));
            ESP_LOGI(TAG, "VOA new eps: %d", voa_eps);
            xQueueSend(voa_eps_queue, &voa_eps, 10);
        }
#endif
        if (cJSON_HasObjectItem(settings_json, "attenuation"))
        {
            uint8_t attenuation = (uint8_t)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(settings_json, "attenuation"));
            if (attenuation <= 20)
            {
                ESP_LOGI(TAG, "Attenuation: %d dB", attenuation);
                xQueueSend(voa_attenuation_queue, &attenuation, 10);
            }
            else
            {
                ESP_LOGE(TAG, "Parse JSON error or invalid attenuation value");
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_app_stop(void)
{
    esp_mqtt_client_stop(client);
}
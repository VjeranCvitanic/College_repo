#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"
#include <stdlib.h>

#define STR_SIZE 30

char *TAG = "BLE-Server";
uint8_t ble_addr_type;

static int counterBLE = 0;
static int writeBLE = 1;
static long int readBLE = 23144357;
volatile bool isNotified = false;

volatile struct ble_gatt_access_ctxt* temp_ctx;
volatile uint16_t temp_conn_handle = 0;
volatile uint16_t temp_attr_handle = 0;


void ble_app_advertise(void);
void vTasksendNotification(void);

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    int temp = *ctxt->om->om_data;
    
    if(temp > 0 && temp <= 10)
    {
        printf("Data from the client: %d\n", temp);
        writeBLE = temp;
        return 0;
    }

    printf("Data has to be in [1, 10], sent data: %d\n", temp);
    return -1;
}
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    char str[STR_SIZE] = {0};
    snprintf(str, sizeof(str), "%ld", *(long int*)arg);

    os_mbuf_append(ctxt->om, str, strlen(str));

    return 0;
}
static int device_notify(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    isNotified = true;
    printf("NOTIFY\n");

    temp_ctx = (struct ble_gatt_access_ctxt*)malloc(sizeof(struct ble_gatt_access_ctxt));
    if (temp_ctx == NULL) {
        printf("Failed to allocate memory for temp context\n");
        return NULL;
    }
    memcpy(temp_ctx, ctxt, sizeof(struct ble_gatt_access_ctxt));
    
    temp_conn_handle = con_handle;
    temp_attr_handle = attr_handle;

    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
 {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = BLE_UUID16_DECLARE(0x4357),
    .characteristics = (struct ble_gatt_chr_def[]){
    {.uuid = BLE_UUID16_DECLARE(0x4358),
    .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
    .access_cb = device_notify},
    {.uuid = BLE_UUID16_DECLARE(0x4359),
    .flags = BLE_GATT_CHR_F_READ,
    .access_cb = device_read,
    .arg = &readBLE},
    {.uuid = BLE_UUID16_DECLARE(0x4360),
    .flags = BLE_GATT_CHR_F_WRITE,
    .access_cb = device_write},
 {0}}},
 {0}};

 static int ble_gap_event(struct ble_gap_event *event, void *arg){
    switch (event->type){
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0){
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

void ble_app_advertise(void){
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}
void ble_app_on_sync(void){
    ble_hs_id_infer_auto(0, &ble_addr_type); 
    ble_app_advertise();
}
void host_task(void *param){
    nimble_port_run();
}
void app_main(){
    nvs_flash_init();
    esp_nimble_hci_and_controller_init();
    nimble_port_init();
    ble_svc_gap_device_name_set("VjeranCvitanic"); 
    ble_svc_gap_init(); 
    ble_svc_gatt_init(); 
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);

    xTaskCreate(vTasksendNotification, "vTasksendNotification", 4096, NULL, 1, NULL);
}

void vTasksendNotification()
{
    struct os_mbuf *om;
    char str[STR_SIZE] = {0};
    while(1)
    {
        if(isNotified)
        {   
            printf("Updating...\n");
            counterBLE += writeBLE;
            
            snprintf(str, sizeof(str), "%d", counterBLE);
            om = ble_hs_mbuf_from_flat(str, sizeof(str));
            if(ble_gattc_notify_custom(temp_conn_handle, temp_attr_handle, om) != 0)
                return;
            
            //ble_gatts_notify(temp_conn_handle, temp_attr_handle);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            printf("no one subscribed to notifications\n");
        }
    }

    vTaskDelete(NULL);
}
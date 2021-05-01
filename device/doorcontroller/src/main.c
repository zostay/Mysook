#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "driver/spi_master.h"
#include "driver/timer.h"
#include "driver/gpio.h"

#include <DoorlightSPI.h>

// max ESP-IDF timer divider because we don't need ticks every often with a
// timer running at 80MHz or so.
#define FLIP_TIMER_DIVIDER   65536 
#define FLIP_TIMER_SCALAR    (TIMER_BASE_CLK / FLIP_TIMER_DIVIDER)
#define FLIP_ALARM_HERTZ     30
#define FLIP_ALARM_TICK      (FLIP_TIMER_SCALAR / FLIP_ALARM_HERTZ)

#define FLIP_TIMER_GROUP    TIMER_GROUP_1
#define FLIP_TIMER_TIMER    TIMER_0

#define DOORLIGHT_HOST      HSPI_HOST
#define DOORLIGHT_DMA_CHAN  2

#define DOORLIGHT_PIN_MISO  HSPI_IOMUX_PIN_NUM_MISO
#define DOORLIGHT_PIN_MOSI  HSPI_IOMUX_PIN_NUM_MOSI
#define DOORLIGHT_PIN_CLK   HSPI_IOMUX_PIN_NUM_CLK
#define DOORLIGHT_PIN_CS    HSPI_IOMUX_PIN_NUM_CS

#define DOORLIGHT_WIDTH     32
#define DOORLIGHT_HEIGHT    16
#define DOORLIGHT_BPP       3
#define DOORLIGHT_LINE_SIZE (DOORLIGHT_WIDTH * DOORLIGHT_BPP) + 2

#define DOORLIGHT_SPI_HZ    (250*1000) // 8MHz (16MHz / 2)

ESP_EVENT_DEFINE_BASE(EVT_TX_BUFFER);

enum {
    EVT_TXB_LOAD,
    EVT_TXB_SEND,
};

struct send_line_msg {
    short kf;
    short y;
};

static const char *TAG = "DoorCtrl";
static esp_event_loop_handle_t app_loop;
 
static spi_device_handle_t doorlight_spi;
 
static volatile bool loaded_tx_buffer = false;

static FILE *bmp_file;
static char tx_buffer[DOORLIGHT_HEIGHT][DOORLIGHT_WIDTH*DOORLIGHT_BPP+2];

static short g_kf_cur = 0;

extern char color_map[256];
extern short key_frame_count;
extern short key_frames[];

// Handle flip callbacks which send updates of the display over SPI.
static bool IRAM_ATTR flip_timer_isr_callback(void *__args) {
    esp_event_post_to(app_loop, EVT_TX_BUFFER, EVT_TXB_SEND, &g_kf_cur, sizeof(g_kf_cur), 0);

    g_kf_cur++;
    if (g_kf_cur >= key_frame_count) {
        g_kf_cur = 0;
    } 

    esp_event_post_to(app_loop, EVT_TX_BUFFER, EVT_TXB_LOAD, &g_kf_cur, sizeof(g_kf_cur), 0);

    return true;
}

// Setup the interrupt that handles display updates.
void init_flip_timer(int group, int timer) {
    ESP_LOGI(TAG, "Flip timer initialization start");

    timer_config_t config = {
        .divider     = FLIP_TIMER_DIVIDER, // slow the timer counter down
        .counter_dir = TIMER_COUNT_UP,     // count up
        .counter_en  = TIMER_PAUSE,        // stopped until timer_start()
        .alarm_en    = TIMER_ALARM_EN,     // enable the alarm
        .auto_reload = 1,                  // restart the alarm after interrupt
    };
    timer_init(group, timer, &config);

    // reset the timer counter to 0
    timer_set_counter_value(group, timer, 0);

    // setup the alarm to trigger our frame flip interrupt
    timer_set_alarm_value(group, timer, FLIP_ALARM_TICK);

    // use an interrupt handler to receive the alarm events
    timer_enable_intr(group, timer);
    timer_isr_callback_add(group, timer, flip_timer_isr_callback, 0, 0);

    // now start running
    timer_start(group, timer);

    ESP_LOGI(TAG, "Flip timer initialization complete");
}

#define BMP_W 320
#define BMP_H 320

// Create a colorful gradient and key frames to slide around that gradient
void init_filesystem() {
    ESP_LOGI(TAG, "SPIFFS initialization start");

    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 12,
    };

    // register file system
    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_config);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    ESP_LOGI(TAG, "SPIFFS initialization complete");
    ESP_LOGI(TAG, "Open test-image.data start");

    bmp_file = fopen("/spiffs/test-image.data", "r");
    if (bmp_file == NULL) {
        ESP_LOGE(TAG, "Failed to open test-image.data file.");
        return;
    }

    ESP_LOGI(TAG, "Open test-image.data complete");

//    int size = BMP_W * BMP_H * DOORLIGHT_BPP;
//    bmp = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
//    if (bmp == NULL) {
//        ESP_LOGE(TAG, "Failure to allocate %d bytes for bmp", size);
//        return;
//    }
//
//    ESP_LOGI(TAG, "Allocated %d bytes for bmp", size);
//
//    for (int x = 0; x < BMP_W; x++) {
//        for (int y = 0; y < BMP_H; y++) {
//            ESP_LOGI(TAG, "Setup BMP (%d, %d)", x, y);
//            int i = (x + y * BMP_W) * DOORLIGHT_BPP;
//            bmp[i]   = 255 - y * 256 / 320;
//            int d = (x > y) ? x-y : y-x;
//            bmp[i+1] = 255 - d * 256 / 160;
//            bmp[i+2] = 255 - x * 256 / 320;
//        }
//    }
}

// Setup communication with the doorlight.
void init_doorlight() {
    ESP_LOGI(TAG, "SPI initialization start");
    esp_err_t err;

    spi_bus_config_t bus_config = {
        .miso_io_num     = DOORLIGHT_PIN_MISO,
        .mosi_io_num     = DOORLIGHT_PIN_MOSI,
        .sclk_io_num     = DOORLIGHT_PIN_CLK,
        .quadwp_io_num   = -1, // disable
        .quadhd_io_num   = -1, // disable
        .max_transfer_sz = DOORLIGHT_LINE_SIZE,
    };

    spi_device_interface_config_t dev_config = {
        .command_bits   = 0, // skip command phase
        .address_bits   = 0, // skip address phase
        .dummy_bits     = 0, // skip dummy phase
        .clock_speed_hz = DOORLIGHT_SPI_HZ,
        .mode           = 0, // must match Arduino setDataMode()
        .spics_io_num   = DOORLIGHT_PIN_CS,
        .queue_size     = DOORLIGHT_HEIGHT,
    };
    
    // Initialize SPI master on HSPI pins
    err = spi_bus_initialize(DOORLIGHT_HOST, &bus_config, DOORLIGHT_DMA_CHAN);
    ESP_ERROR_CHECK(err);

    // Tell the driver how to communicate with the Doorlight Peripheral 
    err = spi_bus_add_device(DOORLIGHT_HOST, &dev_config, &doorlight_spi);
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "SPI initialization complete");
}

void load_tx_buffer(short kf_cur) {
    //ESP_LOGI(TAG, "key_frame %d loading", kf_cur);

    for (int y = 0; y < DOORLIGHT_HEIGHT; y++) {
        short kx = key_frames[kf_cur*2];
        short ky = key_frames[kf_cur*2+1];

        fseek(bmp_file, (ky*DOORLIGHT_WIDTH+kx)*DOORLIGHT_BPP, SEEK_SET);
        size_t s = fread(tx_buffer[y]+1, DOORLIGHT_BPP, DOORLIGHT_WIDTH, bmp_file);
        if (s != DOORLIGHT_WIDTH)
            ESP_LOGE(TAG, "Tried to read %d pixels, but only got %d pixels.", DOORLIGHT_WIDTH, s);

        for (int i = 0; i < DOORLIGHT_WIDTH*DOORLIGHT_BPP; i++) {
            tx_buffer[y][i+1] = color_map[ (int) tx_buffer[y][i+1] ];
        }

        tx_buffer[y][0] = y == 0 ? VSYNC_BYTE : SYNC_BYTE;
        tx_buffer[y][DOORLIGHT_WIDTH+1] = END_BYTE;
    }

    //ESP_LOGI(TAG, "key_frame %d loaded", kf_cur);

    loaded_tx_buffer = true;
}

void send_tx_buffer(short kf_cur) {
    //ESP_LOGI(TAG, "key_frame %d sending", kf_cur);

    for (int bmp_y = 0; bmp_y < DOORLIGHT_HEIGHT; bmp_y++) {
        spi_transaction_t line_tx = {
            .length    = (DOORLIGHT_WIDTH * DOORLIGHT_BPP + 2) * 8,
            .tx_buffer = tx_buffer[bmp_y],
            .rx_buffer = NULL,
        };
        //ESP_LOGI(TAG, "sending %d bits", (DOORLIGHT_WIDTH * DOORLIGHT_BPP + 2) * 8);
        spi_device_polling_transmit(doorlight_spi, &line_tx);
        vTaskDelay(1);
    }
    //ESP_LOGI(TAG, "key_frame %d sent", kf_cur);
}

void evh_load_tx_buffer(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data) {
    short *kf_cur = (short *) event_data;
    load_tx_buffer(*kf_cur);
}

void evh_send_tx_buffer(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data) {
    short *kf_cur = (short *) event_data;
    send_tx_buffer(*kf_cur);
}

static void main_loop(void *args) {
    while (1) {
        esp_event_loop_run(app_loop, 10);
        vTaskDelay(1);
    }
}

void app_main() {
    ESP_LOGI(TAG, "App boot start");

    init_doorlight();
    init_filesystem();
    load_tx_buffer(0);
    init_flip_timer(FLIP_TIMER_GROUP, FLIP_TIMER_TIMER);

    esp_event_loop_args_t loop_config = {
        .queue_size      = 10,
        .task_name       = "main_loop_task",
        .task_priority   = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id    = tskNO_AFFINITY,
    };

    esp_event_loop_create(&loop_config, &app_loop);

    esp_event_handler_register_with(app_loop, EVT_TX_BUFFER, EVT_TXB_LOAD, evh_load_tx_buffer, NULL);
    esp_event_handler_register_with(app_loop, EVT_TX_BUFFER, EVT_TXB_SEND, evh_send_tx_buffer, NULL);

    ESP_LOGI(TAG, "App boot complete");

    xTaskCreate(main_loop, "main_loop", 2048, NULL, uxTaskPriorityGet(NULL), NULL);
}

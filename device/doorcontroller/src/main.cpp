#include <Arduino.h>

#include "esp_log.h"
#include "esp_spiffs.h"

#include "Adafruit_NeoPixel.h"

static const char *TAG = "DoorLight";

#define log_verbose(fmt, ...) ESP_LOGV(TAG, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)   ESP_LOGD(TAG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    ESP_LOGI(TAG, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   ESP_LOGE(TAG, fmt, ##__VA_ARGS__)

static FILE *bmp_file;

#define MATRIX_WIDTH  32
#define MATRIX_HEIGHT 8

#define DOORLIGHT_WIDTH     32
#define DOORLIGHT_HEIGHT    16
#define DOORLIGHT_BPP       3
#define DOORLIGHT_LINE_SIZE (DOORLIGHT_WIDTH * DOORLIGHT_BPP)

#define BMP_WIDTH     320
#define BMP_LINE_SIZE (BMP_WIDTH*DOORLIGHT_BPP)
#define BMP_HEIGHT    320

#define MATRIX_PIN 23
#define N_LEDS (DOORLIGHT_WIDTH*DOORLIGHT_HEIGHT)

#define BRIGHTNESS 10

extern short key_frame_count;
extern short key_frames[];

static int current_keyframe = 0;
static bool matrix_ready = false;
static Adafruit_NeoPixel matrix(N_LEDS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

void init_filesystem() {
    log_info("SPIFFS initialization start");

    esp_vfs_spiffs_conf_t spiffs_config = {
        base_path       : "/spiffs",
        partition_label : NULL,
        max_files       : 12,
    };
    
    // register file system
    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_config);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            log_error("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            log_error("Failed to find SPIFFS partition");
        } else {
            log_error("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    log_info("SPIFFS initialization complete");
    log_info("Open test-image.data start");

    bmp_file = fopen("/spiffs/latte.data", "r");
    if (bmp_file == NULL) {
        log_error("Failed to open test-image.data file.");
        return;
    }

    log_info("Open test-image.data complete");

    log_info("SPIFFS initialization complete");
}

// Map pixels into the matrix grid
void draw_pixel(uint8_t x, uint8_t y, uint32_t c) {
    log_verbose("(%2d, %2d) <- %06X", x, y, c);

    if (y >= 16 || x >= 32) return;

    int xi, i;
    if (y < 8) {
        xi = 256 - (x + 1) * MATRIX_HEIGHT;
    }
    else {
        xi = 256 + x * MATRIX_HEIGHT;
    }

    if (x % 2 == 0) {
        i = xi + (y % MATRIX_HEIGHT);
    }
    else {
        i = xi + (MATRIX_HEIGHT - (y % MATRIX_HEIGHT) - 1);
    }

    matrix.setPixelColor(i, c);
}

void show_next_frame(short kf_cur) {
    log_debug("key_frame %d loading", kf_cur);

    uint8_t pix[DOORLIGHT_LINE_SIZE];
    for (int y = 0; y < DOORLIGHT_HEIGHT; y++) {
        short kx = key_frames[kf_cur*2];
        short ky = key_frames[kf_cur*2+1]+y;

        int start = (ky*BMP_WIDTH+kx)*DOORLIGHT_BPP;
        log_debug("seek %d", start);
        fseek(bmp_file, start, SEEK_SET);

        size_t s = fread(pix, DOORLIGHT_BPP, DOORLIGHT_WIDTH, bmp_file);
        if (s < DOORLIGHT_WIDTH) {
            log_error("failed to read doorlight line, expected %d pixels, but got %d pixels", DOORLIGHT_WIDTH, s);
        }

        for (int x = 0; x < DOORLIGHT_LINE_SIZE; x += 3) {
            uint32_t c = matrix.Color(pix[x], pix[x+1], pix[x+2]);
            draw_pixel(x/3, y, c);
        }
    }

    log_debug("key_frame %d loaded", kf_cur);

    matrix.show();
}

void setup() {
    matrix.begin();
    matrix.setBrightness(BRIGHTNESS);

    Serial.begin(115200);

    esp_log_level_set(TAG, ESP_LOG_INFO);

    init_filesystem();
}

void loop() {
    log_debug("hello");

    show_next_frame(current_keyframe);
    current_keyframe++;
    if (current_keyframe >= key_frame_count)
        current_keyframe = 0;

    vTaskDelay(30);
}

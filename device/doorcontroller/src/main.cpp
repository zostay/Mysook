#include <Arduino.h>

#include "esp_log.h"
#include "esp_spiffs.h"

#include "FastLED.h"
#include "ESP32TimerInterrupt.h"

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

extern volatile short key_frame_count;
extern short key_frames[];

static int current_keyframe = 0;
static CRGB matrix[N_LEDS];

static volatile bool ready_to_load = true;
static volatile bool loaded = false;
static volatile bool ready_to_show = false;
static ESP32Timer flip_timer(1);

static unsigned long fps_last_millis = millis();

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
void draw_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
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

    matrix[i].setRGB(r, g, b);
}

void load_next_frame(short kf_cur) {
    ready_to_load = false;

    log_debug("key_frame %d loading", kf_cur);
    unsigned long frame_time = millis();
    unsigned long total_seek_time = 0, total_read_time = 0, total_draw_time = 0;

    uint8_t pix[DOORLIGHT_LINE_SIZE];
    for (int y = 0; y < DOORLIGHT_HEIGHT; y++) {
        short kx = key_frames[kf_cur*2];
        short ky = key_frames[kf_cur*2+1]+y;

        int start = (ky*BMP_WIDTH+kx)*DOORLIGHT_BPP;
        log_debug("seek %d", start);
        unsigned long seek_time = micros();
        lseek(fileno(bmp_file), start, SEEK_SET);
        total_seek_time += micros() - seek_time;

        unsigned long read_time = micros();
        size_t s = read(fileno(bmp_file), pix, DOORLIGHT_LINE_SIZE);
        total_read_time += micros() - read_time;
        if (s < DOORLIGHT_LINE_SIZE) {
            log_error("failed to read doorlight line, expected %d bytes, but got %d bytes", DOORLIGHT_LINE_SIZE, s);
        }

        unsigned long draw_time = micros();
        for (int x = 0; x < DOORLIGHT_LINE_SIZE; x += 3) {
            draw_pixel(x/3, y, pix[x], pix[x+1], pix[x+2]);
        }
        total_draw_time += micros() - draw_time;
    }

    loaded = true;

    log_info("key_frame %d loaded in %d ms, seeking %d μs, reading %d μs, drawing %d μs", kf_cur, millis()-frame_time, total_seek_time, total_read_time, total_draw_time);
}

void show_next_frame() {
    loaded = false;
    ready_to_show = false;

    unsigned long this_millis = millis();
    log_debug("FPS %d", 1000/(this_millis - fps_last_millis));
    fps_last_millis = this_millis;

    log_debug("key_frame show");

    FastLED.show();

    log_debug("key_frame shown");

    current_keyframe++;
    if (current_keyframe >= key_frame_count)
        current_keyframe = 0;

    ready_to_load = true;
}    

void IRAM_ATTR flip_timer_handler() {
    if (loaded) 
        ready_to_show = true;
}

void setup() {
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(matrix, N_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
    //FastLED.setBrightness(BRIGHTNESS);

    Serial.begin(115200);

    esp_log_level_set(TAG, ESP_LOG_INFO);

    if (flip_timer.attachInterruptInterval(300, flip_timer_handler)) {
        log_error("Flip timer setup failed.");
    }
    else {
        log_info("Flip timer setup complete.");
    }

    init_filesystem();
}

void loop() { 
    if (ready_to_load)
        load_next_frame(current_keyframe);

    if (ready_to_show)
        show_next_frame();
}

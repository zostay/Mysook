#define MATRIX_WIDTH  32
#define MATRIX_HEIGHT 8

#ifdef ARDUINO
#include <Arduino.h>

#include "esp_log.h"

#ifdef USE_LittleFS
  // merged into esp32 core since 1.0.6
  // #include <LITTLEFS.h>
  #include <LittleFS.h>

  // FS *filesystem = &LITTLEFS;
  #define LITTLEFS LittleFS
  #define SPIFFS LITTLEFS
  #define FileFS LITTLEFS
  #define FS_NAME "LittleFS"
#else
  #include <SPIFFS.h>
#endif

#include "FastLED.h"
#include "FastLED_NeoMatrix.h"
#include "ESP32TimerInterrupt.h"

#include <Animation.h>
#include <Firmware.h>
#include <MC_Panel.h>
#include <MC_Logger.h>

FastLED_NeoMatrix matrix(strip, MATRIX_WIDTH, MATRIX_HEIGHT, 1, 2, 
    NEO_MATRIX_BOTTOM | NEO_MATRIX_RIGHT | NEO_MATRIX_COLUMNS 
        | NEO_MATRIX_ZIGZAG | NEO_TILE_ZIGZAG);

mysook::MC_Logger<Print> logger(&micros, Serial);
#endif//ARDUINO

#include "doorlight.h"

static const char *TAG = "DoorLight";

#define log_verbose(fmt, ...) ESP_LOGV(TAG, fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)   ESP_LOGD(TAG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)    ESP_LOGI(TAG, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)   ESP_LOGE(TAG, fmt, ##__VA_ARGS__)

//static FILE *bmp_file;
static File bmp_file;

#define DOORLIGHT_WIDTH     32
#define DOORLIGHT_HEIGHT    16
#define DOORLIGHT_BPP       3
#define DOORLIGHT_LINE_SIZE (DOORLIGHT_WIDTH * DOORLIGHT_BPP)

#define BMP_WIDTH     320
#define BMP_LINE_SIZE (BMP_WIDTH*DOORLIGHT_BPP)
#define BMP_HEIGHT    320

#define MATRIX_PIN 23
#define N_LEDS (DOORLIGHT_WIDTH*DOORLIGHT_HEIGHT)

#define MATRIX_VOLTS    5
#define MAX_MATRIX_AMPS 2000
#define BRIGHTNESS      10

extern short key_frame_count;
extern short key_frames[];

static std::int64_t tick_timer = 0;
static long loaded_frame_remain = 0;
static long shown_frame_remain = 0;
static int current_keyframe = 0;
static CRGB strip[N_LEDS];

#ifdef ARDUINO
static std::unique_ptr<mysook::Animation<mysook::ArduinoFileDelegate>> animation;
#else//ARDUINO
static std::unique_ptr<mysook::Animation<std::istream>> animation;
#endif//ARDUINO

static bool ready_to_load = true;
static bool loaded = false;
static bool ready_to_show = false;

static unsigned long fps_last_millis = millis();

mysook::MC_RGBPanel<DOORLIGHT_WIDTH, DOORLIGHT_HEIGHT, FastLED_NeoMatrix> display(matrix);

#ifdef ARDUINO
Doorlight<mysook::ArduinoFileDelegate> doorlight(log, animation);
#else//ARDUINO
Doorlight<std::istream> doorlight(log, animation);
#endif//ARDUINO

void init_filesystem() {
    log_info("SPIFFS initialization start");

    if (!SPIFFS.begin()) {
        log_error("Failed to start SPIFFS.");
    }
    //esp_vfs_spiffs_conf_t spiffs_config = {
    //    base_path       : "/spiffs",
    //    partition_label : NULL,
    //    max_files       : 12,
    //};
    
    // register file system
    //esp_err_t ret = esp_vfs_spiffs_register(&spiffs_config);

    //if (ret != ESP_OK) {
    //    if (ret == ESP_FAIL) {
    //        log_error("Failed to mount or format filesystem");
    //    } else if (ret == ESP_ERR_NOT_FOUND) {
    //        log_error("Failed to find SPIFFS partition");
    //    } else {
    //        log_error("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    //    }
    //    return;
    //}

    log_info("SPIFFS initialization complete");
}

void load_animation() {
    log_info("Open bitmap file start");

    bmp_file = SPIFFS.open("/congrats-logan.manic", "r");
    if (!bmp_file) {
        log_error("Failed to open bitmap file.");
        return;
    }
    //bmp_file = fopen("/spiffs/latte.data", "r");
    //if (bmp_file == NULL) {
    //    log_error("Failed to open test-image.data file.");
    //    return;
    //}

    log_info("Parsing bitmap file.");

    animation.reset(new mysook::Animation(bmp_file));

    log_info("Open bitmap file complete, size is %d bytes", bmp_file.size());

    log_info("SPIFFS initialization complete");
}

// Map pixels into the matrix grid
// void draw_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
//     log_verbose("(%2d, %2d) <- %02X%02X%02X", x, y, r, g, b);
//     display.put_pixel(x, y, mysook::Color(r, g, b));
// }

static std::uint8_t blink = 0;
void load_next_frame() {
    log_debug("Loading frame %d", current_keyframe);
    loaded_frame_remain = animation.get()->frame_time(current_keyframe);
    current_keyframe = animation.get()->render(&display, current_keyframe, bmp_file);
    display.put_pixel(0, 0, mysook::Color(blink, 0, 0));
    blink = blink == 0 ? 255 : 0;
    log_debug("Frame is loaded.");
}

// void load_next_frame(short kf_cur) {
//     ready_to_load = false;

//     // Blink on error
//     if (!bmp_file) {
//         log_error("No file to read frame from.");
//         uint8_t r = kf_cur % 30 < 20 ? 0x99 : 0x00;
//         for (int y = 0; y < 3; y++) {
//             for (int x = 0; x < 3; x++) {
//                 draw_pixel(x, y, r, 0, 0);
//             }
//         }
//         loaded = true;
//         return;
//     }

//     log_debug("key_frame %d loading", kf_cur);
//     unsigned long frame_time = millis();
//     unsigned long total_seek_time = 0, total_read_time = 0, total_draw_time = 0;

//     uint8_t pix[DOORLIGHT_LINE_SIZE];
//     for (int y = 0; y < DOORLIGHT_HEIGHT; y++) {
//         short kx = key_frames[kf_cur*2];
//         short ky = key_frames[kf_cur*2+1]+y;

//         int start = (ky*BMP_WIDTH+kx)*DOORLIGHT_BPP;
//         log_debug("seek %d", start);
//         unsigned long seek_time = micros();
//         bmp_file.seek(start, SeekSet);
//         //lseek(fileno(bmp_file), start, SEEK_SET);
//         total_seek_time += micros() - seek_time;

//         unsigned long read_time = micros();
//         size_t s = bmp_file.read(pix, DOORLIGHT_LINE_SIZE);
//         //size_t s = read(fileno(bmp_file), pix, DOORLIGHT_LINE_SIZE);
//         total_read_time += micros() - read_time;
//         if (s < DOORLIGHT_LINE_SIZE) {
//             log_error("failed to read doorlight line, expected %d bytes, but got %d bytes", DOORLIGHT_LINE_SIZE, s);
//         }

//         unsigned long draw_time = micros();
//         for (int x = 0; x < DOORLIGHT_LINE_SIZE; x += 3) {
//             draw_pixel(x/3, y, pix[x], pix[x+1], pix[x+2]);
//         }
//         total_draw_time += micros() - draw_time;
//     }

//     loaded = true;

//     log_info("key_frame %d loaded in %d ms, seeking %d μs, reading %d μs, drawing %d μs", kf_cur, millis()-frame_time, total_seek_time, total_read_time, total_draw_time);
// }

void show_next_frame() {
    log_debug("Showing frame");

    unsigned long this_millis = millis();
    log_debug("FPS %d", 1000/(this_millis - fps_last_millis));
    fps_last_millis = this_millis;

    log_debug("key_frame show");

    //FastLED.show();
    display.draw();

    log_debug("key_frame shown");
}

void setup() {
#ifdef ARDUINO
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(strip, N_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(MATRIX_VOLTS, MAX_MATRIX_AMPS);
    //FastLED.setBrightness(BRIGHTNESS);

    matrix.begin();
    display.fill_screen(mysook::Color(0, 0, 0));
    display.put_pixel(0, 0, mysook::Color(255, 0, 0));
    display.draw();

    Serial.begin(115200);

    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    tick_timer = esp_timer_get_time();
    // if (flip_timer.attachInterruptInterval(300, flip_timer_handler)) {
    //     log_error("Flip timer setup failed.");
    // }
    // else {
    //     log_info("Flip timer setup complete.");
    // }

    init_filesystem();
    load_animation();
#endif//ARDUINO

    doorlight.setup();
}

void loop() { 
    doorlight.loop();
    std::int64_t new_tick_timer = esp_timer_get_time();
    unsigned long millis_elapsed = (new_tick_timer - tick_timer) / 1000;
    shown_frame_remain -= millis_elapsed;
    tick_timer = new_tick_timer;

    if (ready_to_load) {
        ready_to_load = false;
        load_next_frame();
        loaded = true;
    }

    if (loaded && (shown_frame_remain <= 0)) {
        ready_to_show = true;
    }

    if (ready_to_show) {
        loaded = false;
        ready_to_show = false;
        show_next_frame();
        ready_to_load = true;
        shown_frame_remain = loaded_frame_remain;
    } 

    // WHY DOES THIS CODE NEED A LOG MESSAGE TO WORK!?!??!?!?
    log_info("frame_remain = %d, ready_to_load = %d, loaded = %d, ready_to_show = %d", shown_frame_remain, ready_to_load, loaded, ready_to_show);
}

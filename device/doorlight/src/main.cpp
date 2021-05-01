#include <Adafruit_NeoPixel.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <DoorlightSPI.h>
#include <MC_Logger.h>

#define MATRIX_PIN 6

#define BRIGHTNESS 16

#define N_LEDS (MAX_X*MAX_Y)

// Setup the hardware display
Adafruit_NeoPixel matrix(N_LEDS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

// TODO Why does initializing this kill the NeoMatrix!?
//mysook::MC_Logger<Print> logger(&micros, Serial);

//    0   1   2   3
// 0  248 247 232 231
// 1  249 246 233 230
// 2  250 245 234 229
// 3  251 244 235 228
// 4  252 243 236 227
// 5  253 242 237 226
// 6  254 241 238 225
// 7  255 240 239 224
// 8
// 9
// 10
// 11
// 12
// 13
// 14
// 15
// 
// 256 - (x + 1) * 8 + y
// 256 - (x + 1) * 8 + (7 - y)
// 256 + x * 8 + y
// 256 + x * 8 + (7 - y)

void drawPixel(uint8_t x, uint8_t y, uint32_t c) {
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

    //Serial.println(i);

    // if (x == 0 && y == 0) {
    //     i = 248;
    // }
    // else if (x == 1 && y == 0) {
    //     i = 247;
    // }
    // else if (x == 0 && y == 1) {
    //     i = 249;
    // }
    // else if (x == 0 && y == 8) {
    //     i = 256;
    // }
    // else if (x == 16 && y == 0) {
    //     i = 120;
    // }
    // else {
    //     i = 0;
    // }
    matrix.setPixelColor(i, c);
}

void setup() {
    matrix.begin();
    matrix.setBrightness(BRIGHTNESS);

    // Start up the serial connection for debugging
    Serial.begin(115200);

    // Configure for SPI slave
    pinMode(MISO, OUTPUT);

    SPI.setClockDivider(SPI_CLOCK_DIV32);
    SPI.setDataMode(SPI_MODE0);

    SPCR |= _BV(SPE); // Switch to SPI slave
    SPI.attachInterrupt(); // enables SPI slave interrupt

    // for (int y = 0; y < MAX_Y; y++) {
    //     for (int x = 0; x < MAX_X; x++) {
    //         uint32_t c = Adafruit_NeoPixel::Color(0x00, 0x00, 0xFF);
    //         matrix.setPassThruColor(c);
    //         matrix.drawPixel(x, y, 0);
    //     }
    // }

    //logger.logf_ln("BOOT");
    Serial.println("BOOT");
}

static void chase(uint32_t c) {
    uint8_t lx = -1, ly = -1;
    for (uint8_t y = 0; y < 16; y++) {
        for (uint8_t x = 0; x < 32; x++) {
            if (lx >= 0 && ly >= 0)
                drawPixel(lx, ly, 0);

            drawPixel(x, y, c);
            lx = x;
            ly = y;
            matrix.show();
            delay(2);
        }
    }
    drawPixel(lx, ly, 0);
    matrix.show();
}

static void testPattern() {
    matrix.clear();
    drawPixel(0, 0, matrix.Color(0xFF, 0x00, 0x00));
    drawPixel(1, 0, matrix.Color(0xFF, 0xFF, 0x00));
    drawPixel(0, 1, matrix.Color(0x00, 0xFF, 0x00));
    drawPixel(16, 0, matrix.Color(0x00, 0xFF, 0xFF));
    drawPixel(0, 8, matrix.Color(0x00, 0x00, 0xFF));
    matrix.show();
}

void loop() {
    chase(Adafruit_NeoPixel::Color(128, 128, 128));
    //testPattern();
    //delay(2);
}


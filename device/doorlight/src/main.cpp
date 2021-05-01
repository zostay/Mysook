#include <Adafruit_NeoPixel.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <DoorlightSPI.h>
#include <MC_Logger.h>

#define MATRIX_PIN 6
#define BLINK_PIN  13

#define BRIGHTNESS 1

#define BUFFER_SIZE 50

#define N_LEDS (MAX_X*MAX_Y)

int blink_count = 0;
int blink = 0;

// current (x, y) being drawn
short x = 0;
short y = 0;

// ring buffer for the current pixel, 24-bit color in RGB order
byte pixel[3];
int pxpos = 0;

// Setup the hardware display
volatile bool matrix_ready = false;
Adafruit_NeoPixel matrix(N_LEDS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

// TODO Why does initializing this kill the NeoMatrix!?
//mysook::MC_Logger<Print> logger(&micros, Serial);

// Map pixels into the matrix grid
void drawPixel(uint8_t x, uint8_t y, uint32_t c) {
    if (y >= 16 || x >= 32) return;

    Serial.print("("); Serial.print(x, DEC); 
    Serial.print(", "); Serial.print(y, DEC); 
    Serial.print(") <- "); Serial.println(c, HEX);

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

void setup() {
    matrix.begin();
    matrix.setBrightness(BRIGHTNESS);

    // Start up the serial connection for debugging
    Serial.begin(115200);

    // Configure for SPI slave
    pinMode(MISO, OUTPUT);

    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setDataMode(SPI_MODE0);

    SPCR |= _BV(SPE); // Switch to SPI slave
    SPI.attachInterrupt(); // enables SPI slave interrupt

    // Setup the timer interrupt
    TCCR0A = 0; // set timer 0 register A to 0
    TCCR0B = 0; // set timer 0 register B to 0
    TCNT0 = 0; // set timer 0 counter to 0
    OCR0A = 156; // 100Hz, setup alarm subdivide: 16*1000*1000 / 156*1024 - 1
    TCCR0A |= (1 << WGM01); // auto reset the counter                 ▲
    TCCR0B |= (1 << CS02) | (1 << CS00); // prescalar set to 1024 ────┘
    TIMSK0 |= (1 << OCIE0A); // enable interrupt
    sei(); // allow interrupts

    //logger.logf_ln("BOOT");
    Serial.println("Boot Complete");
}

// SPI interrupt handler
ISR (SPI_STC_vect) {
    // write to the pixel buffer
    byte b = SPDR;
    if (b == SYNC_BYTE) {
        pxpos = 0;
        x = 0;
        y++;
    }
    else if (b == VSYNC_BYTE) {
        pxpos = 0;
        x = 0;
        y = 0;

        matrix_ready = true;
    }
    else if (b == END_BYTE) {
        // ignore
    }
    else {
        pixel[pxpos++] = b;

        if (pxpos >= 3) {
            uint32_t c = matrix.Color(pixel[0], pixel[1], pixel[2]);
            drawPixel(x++, y, c);
            pxpos = 0;
        }
    }
}

// simple chase function for testing
static void chase(uint32_t c) {
    static uint8_t cx = 0, cy = 0;
    static uint8_t lx = -1, ly = -1;

    if (lx >= 0 && ly >= 0)
        drawPixel(lx, ly, 0);

    drawPixel(cx, cy, c);
    lx = cx++;
    ly = cy;
    drawPixel(lx, ly, 0);

    if (cx >= MAX_X) {
        cx = 0;
        cy++;
    }

    if (cy >= MAX_Y) {
        cy = 0;
    }

    matrix_ready = true;
    //matrix.show();
}
// 
// // simple test pattern for testing
// static void testPattern() {
//     matrix.clear();
//     drawPixel(0, 0, matrix.Color(0xFF, 0x00, 0x00));
//     drawPixel(1, 0, matrix.Color(0xFF, 0xFF, 0x00));
//     drawPixel(0, 1, matrix.Color(0x00, 0xFF, 0x00));
//     drawPixel(16, 0, matrix.Color(0x00, 0xFF, 0xFF));
//     drawPixel(0, 8, matrix.Color(0x00, 0x00, 0xFF));
//     matrix.show();
// }

// Draw on the panel when ready. We use an interrupt because neopixels don't
// like having their writes interrupted.
ISR (TIMER0_COMPA_vect) { 
    if (matrix_ready) {
        Serial.println("DRAW");
        matrix.show();
        matrix_ready = false;
    }
}

void loop() {
    //Serial.println("LOOP");
    //chase(Adafruit_NeoPixel::Color(128, 128, 128));
    //Serial.println("CHASE");
    //testPattern();
}


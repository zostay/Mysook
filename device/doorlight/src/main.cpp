#include <Adafruit_NeoPixel.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <DoorlightSPI.h>
#include <MC_Logger.h>

#define MATRIX_PIN 6
#define BLINK_PIN  13

#define BRIGHTNESS 1

#define BUFFER_SIZE 200

#define N_LEDS (MAX_X*MAX_Y)

int blink_count = 0;
int blink = 0;

// current ring buffer write position
volatile short wpos = 0;

// current area ready for reading in the ring buffer:
// * rstart = rend: no data ready to read
// * rstart < rend: ready to read from rstart to rend
// * rstart > rend: ready to read from rstart to BUFFER_SIZE, then rstart := 0
volatile short rstart = 0;
volatile short rend = 0;

// ring buffer for incoming pixel data
unsigned char *buf;

// current (x, y) being drawn
short x = 0;
short y = 0;

// ring buffer for the current pixel, 24-bit color in RGB order
unsigned char pixel[3];
char pxpos = 0;

// Setup the hardware display
volatile bool matrix_ready = false;
Adafruit_NeoPixel matrix(N_LEDS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

// TODO Why does initializing this kill the NeoMatrix!?
//mysook::MC_Logger<Print> logger(&micros, Serial);

// Map pixels into the matrix grid
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

    buf = new unsigned char[BUFFER_SIZE];

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
    // write to the current position of the ring buffer
    buf[wpos] = SPDR;

    //Serial.println(buf[wpos], HEX);

    // when we receive the end byte signal, mark the buffer readable
    rend = wpos;

    // reset the write head to beginning of buffer on wraparound
    if (++wpos >= BUFFER_SIZE)
        wpos = 0;
}

// simple chase function for testing
static void chase(uint32_t c) {
    static uint8_t cx = 0, cy = 0;
    static uint8_t lx = -1, ly = -1;

    if (lx >= 0 && ly >= 0)
        drawPixel(lx, ly, 0);

    drawPixel(cx, cy, c);
    Serial.print("("); Serial.print(cx, DEC); 
    Serial.print(", "); Serial.print(cy, DEC); Serial.println(")");
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
        matrix.show();
        matrix_ready = false;
    }
}

void loop() {
    //Serial.println("LOOP");
    chase(Adafruit_NeoPixel::Color(128, 128, 128));
    //Serial.println("CHASE");
    //testPattern();
    //delay(2);

    // reset the read head to beginning of buffer on wraparound
    if (rstart >= BUFFER_SIZE) 
        rstart = 0;

    // effective read end handles wraparound in the ring buffer
    short erend = rend;
    if (erend < rstart)
        erend = BUFFER_SIZE;

    // read the data ready in the ring buffer
    while (rstart < erend) {
        // read the next ready byte
        unsigned char b = buf[rstart++];

        // end byte triggers show
        if (b == END_BYTE) {
            continue;
        }

        // perform a horizontal sync
        else if (b == SYNC_BYTE) {
            pxpos = 0;
            x = 0;
            y++;
        }
        
        // perform a vertical sync
        else if (b == VSYNC_BYTE) {
            x = 0;
            y = 0;
            pxpos = 0;

            //matrix.show();
            matrix_ready = true;
            
            if (blink_count++ % 30 == 0) {
                blink = !blink;
                Serial.println("BLINKY");
                //digitalWrite(BLINK_PIN, blink);
            }
        }

        // fill the pixel buffer
        else {
            pixel[(int) pxpos++] = b;
            if (pxpos > 3) {
                // remap 24-bit color to 16-bit 565 color and draw
                uint32_t c = matrix.Color(pixel[0], pixel[1], pixel[2]);
                drawPixel(x++, y, c);
                pxpos = 0;
            }
        }
    }
}


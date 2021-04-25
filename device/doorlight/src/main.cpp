#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <DoorlightSPI.h>

#define MATRIX_PIN 6

#define BRIGHTNESS 32

#define BUFFER_SIZE 294

// current ring buffer write position
volatile short wpos = 0;

// current area ready for reading in the ring buffer:
// * rstart = rend: no data ready to read
// * rstart < rend: ready to read from rstart to rend
// * rstart > rend: ready to read from rstart to BUFFER_SIZE, then rstart := 0
volatile short rstart = 0;
volatile short rend = 0;

// ring buffer for incoming pixel data
char buf[BUFFER_SIZE];

// current (x, y) being drawn
short x = 0;
short y = 0;

// ring buffer for the current pixel, 24-bit color in RGB order
char pixel[3];
char pxpos = 0;

// Updates the hardware display
Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(
  MATRIX_HEIGHT, MATRIX_WIDTH,
  TILE_HEIGHT, TILE_WIDTH,
  MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

void setup() {
    // Setup the hardware display
    matrix->begin();
    matrix->setBrightness(BRIGHTNESS);

    // Start up the serial connection for debugging
    Serial.begin(115200);

    // Configure for SPI slave
    pinMode(MISO, OUTPUT);

    SPCR |= _BV(SPE); // Switch to SPI slave
    SPI.attachInterrupt(); // enables SPI slave interrupt
}

// SPI interrupt handler
ISR (SPI_STC_vect) {
    // write to the current position of the ring buffer
    buf[wpos] = SPDR;

    // when we receive the end byte signal, mark the buffer readable
    if (buf[wpos] == END_BYTE) {
        rend = wpos;
    }

    // reset the write head to beginning of buffer on wraparound
    if (++wpos >= BUFFER_SIZE)
        wpos = 0;
}

void loop() {
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
        char b = buf[rstart++];

        // end byte is ignored by us
        if (b == END_BYTE)
            continue;

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
        }

        // fill the pixel buffer
        else {
            pixel[(int) pxpos++] = b;
            if (pxpos > 3) {
                // remap 24-bit color to 16-bit 565 color and draw
                uint16_t c = matrix->Color(pixel[0], pixel[1], pixel[2]);
                matrix->drawPixel(x++, y, c);
                pxpos = 0;
            }
        }
    }
}

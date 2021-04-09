#ifndef __DOORLIGHT_SPI_H
#define __DOORLIGHT_SPI_H

#define MATRIX_HEIGHT 8
#define MATRIX_WIDTH  32

#define TILE_HEIGHT 1
#define TILE_WIDTH  1

// pixels may not contain these values, so pixel intensity is clipped to 0xFC
#define END_BYTE   0xFF
#define VSYNC_BYTE 0xFE
#define SYNC_BYTE  0xFD

#define MAX_Y MATRIX_HEIGHT * TILE_HEIGHT
#define MAX_X MATRIX_WIDTH  * TILE_WIDTH

#endif//__DOORLIGHT_SPI_H

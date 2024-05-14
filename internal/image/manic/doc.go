// Package manic contains the definitions and tools to make use of the
// micro-animation imagge foramt I designed for use with my micro-controller
// projects.
//
// A manic animation file has the following format:
//
//	+------------------------+---------------------+
//	| Magic Number ("MANIC") | 5 bytes             |
//	| Version ('0')          | 1 byte              |
//	| Frame Width            | 2 bytes (uint16)    |
//	| Frame Height           | 2 bytes (uint16)    |
//	| Keyframe Count         | 2 bytes (uint16)    |
//	| Image Count            | 2 bytes (uint16)    |
//	+------------------------+---------------------+
//	| Key Frames             | 12 bytes per frame  |
//	| - Image Index          | 2 bytes (uint16)    |
//	| - X, Y Origin          | 8 bytes (2x int32)  |
//	| - Milliseconds         | 2 bytes (uint16)    |
//	+------------------------+---------------------+
//	| Image Index            | 20 bytes per image  |
//	| - OriginX, OriginY     | 8 bytes (2x int32)  |
//	| - ExtentX, ExtentY     | 8 bytes (2x int32)  |
//	| - Seek Position        | 4 bytes (int32)     |
//	+------------------------+---------------------+
//	| Images                 | N bytes per image   |
//	| - []PixelData          | 3-bytes per pixel   |
//	+------------------------+---------------------+
//
// In Version 0 of the format, the interpretation of the file is as follows.
// Each file contains 1 or more images from which the keyframes are selected.
// These images are encoded at the very end of the file. As these images
// could be located anywhere in the file at arbitrary locations, an index is
// provided to speed up seeking within the file.
//
// From there, playing the animation requires iterating through the key frames
// in order, first to last. This format is very simple and provides no
// compression. Each key frame is the full frame size and is expected to
// overwrite the entire canvas of Frame Width/Height size each time. The key
// frame selects its pixels from a clipped window taken from one of the images.
// The image is selected by teh Image Index and the window is bounded by the key
// frame's (x, y) origin with the frame (width x height). The frame should be
// held for approximately the given milliseconds encoded on the frame.
//
// Bytes are always ordered little endian.
//
// The image data is encoded in two parts. The first parts contains the image
// index, which describes the bounds of the image and the seek position that
// points to the position in the file where the pixel data starts. The second
// section is frames of pixel data. The pixel data is stored in byte triplets,
// three bytes per pixel. Calculating the position in the pixel data for a given
// (x, y) is performed by calculating ((y-OriginY)*stride+(x-OriginX))*3 where
// stride is equal to ExtentX-OriginX.
//
// The pixel triples are stored in RGB order. Each unsigned byte representing the
// intensity of each color, from 0 to 255.
package manic

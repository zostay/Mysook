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
//	| Key Frames             | 8 bytes per frame   |
//	| - Image Index          | 2 bytes (uint16)    |
//	| - X, Y Origin          | 4 bytes (2x uint16) |
//	| - Milliseconds         | 2 bytes (uint16)    |
//	+------------------------+---------------------+
//	| Image Index            | 8 bytes per image   |
//	| - Seek Position        | 8 bytes (int64)     |
//	+------------------------+---------------------+
//	| Images                 | N bytes per image   |
//	| - OriginX              | 8 bytes (int64)     |
//	| - OriginY              | 8 bytes (int64)     |
//	| - ExtentX              | 8 bytes (int64)     |
//	| - ExtentY              | 8 bytes (int64)     |
//	| - []PixelData          | Arbitrary Length    |
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
package manic

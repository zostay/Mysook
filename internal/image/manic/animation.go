package manic

// Version0 is the version number of the current animation format.
const Version0 = '0'

// Rect is a rectangle in 2D space. The Min point is the inclusive origin of the
// rectangel and the Max point is the exclusive end of the rectangle.
type Rect struct {
	Min, Max Point
}

// Point is a single 2D point in an image.
type Point struct {
	X, Y int32
}

// Animation is the internal representation of a manic animation.
type Animation struct {
	Version        byte
	FrameW, FrameH uint16
	Keyframes      []Keyframe
	Images         []Image
}

// Keyframe represents a single frame to use to fill the entire display
type Keyframe struct {
	ImgIndex uint16
	Origin   Point
	Millis   uint16
}

// New creates a new, empty manic animation.
func New(w, h uint16) *Animation {
	return &Animation{
		Version: Version0,
		FrameW:  w,
		FrameH:  h,
	}
}

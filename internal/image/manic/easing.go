package manic

import "math"

// Easing is a function that takes a float64 between 0 and 1 and returns a
// float64 between 0 and 1 (or perhaps ever so slightly beyond these extents for
// certain easing types). The input is the progress of the animation and the
// output is the progress of the easing function.
type Easing func(x float64) float64

// EaseInSine is an easing function that starts slow and speeds up, following a
// sine curve.
func EaseInSine(x float64) float64 {
	return 1 - math.Cos((x*math.Pi)/2)
}

// EaseOutSine is an easing function that starts fast and slows down, following
// a sine curve.
func EaseOutSine(x float64) float64 {
	return math.Sin((x * math.Pi) / 2)
}

// EaseInOutSine is an easing function that starts slow, speeds up, then slows
// down again, following a sine curve.
func EaseInOutSine(x float64) float64 {
	return -(math.Cos(math.Pi*x) - 1) / 2
}

// EaseInQuad is an easing function that starts slow and speeds up, following a
// parabolic curve.
func EaseInQuad(x float64) float64 {
	return x * x
}

// EaseOutQuad is an easing function that starts fast and slows down, following
// a parabolic curve.
func EaseOutQuad(x float64) float64 {
	return 1 - (1-x)*(1-x)
}

// EaseInOutQuad is an easing function that starts slow, speeds up, then slows
// down again, following a parabolic curve.
func EaseInOutQuad(x float64) float64 {
	if x < 0.5 {
		return 2 * x * x
	}
	return 1 - math.Pow(-2*x+2, 2)/2
}

// EaseInCubic is an easing function that starts slow and speeds up, following a
// cubic curve.
func EaseInCubic(x float64) float64 {
	return x * x * x
}

// EaseOutCubic is an easing function that starts fast and slows down, following
// a cubic curve.
func EaseOutCubic(x float64) float64 {
	return 1 - math.Pow(1-x, 3)
}

// EaseInOutCubic is an easing function that starts slow, speeds up, then slows
// down again, following a cubic curve.
func EaseInOutCubic(x float64) float64 {
	if x < 0.5 {
		return 4 * x * x * x
	}
	return 1 - math.Pow(-2*x+2, 3)/2
}

// EaseInQuart is an easing function that starts slow and speeds up, following a
// quartic curve.
func EaseInQuart(x float64) float64 {
	return x * x * x * x
}

// EaseOutQuart is an easing function that starts fast and slows down, following
// a quartic curve.
func EaseOutQuart(x float64) float64 {
	return 1 - math.Pow(1-x, 4)
}

// EaseInOutQuart is an easing function that starts slow, speeds up, then slows
// down again, following a quartic curve.
func EaseInOutQuart(x float64) float64 {
	if x < 0.5 {
		return 8 * x * x * x * x
	}
	return 1 - math.Pow(-2*x+2, 4)/2
}

// EaseInQuint is an easing function that starts slow and speeds up, following a
// quintic curve.
func EaseInQuint(x float64) float64 {
	return x * x * x * x * x
}

// EaseOutQuint is an easing function that starts fast and slows down, following
// a quintic curve.
func EaseOutQuint(x float64) float64 {
	return 1 - math.Pow(1-x, 5)
}

// EaseInOutQuint is an easing function that starts slow, speeds up, then slows
// down again, following a quintic curve.
func EaseInOutQuint(x float64) float64 {
	if x < 0.5 {
		return 16 * x * x * x * x * x
	}
	return 1 - math.Pow(-2*x+2, 5)/2
}

// EaseInExpo is an easing function that starts slow and speeds up, following an
// exponential curve.
func EaseInExpo(x float64) float64 {
	if x == 0 {
		return 0
	}
	return math.Pow(2, 10*(x-1))
}

// EaseOutExpo is an easing function that starts fast and slows down, following
// an exponential curve.
func EaseOutExpo(x float64) float64 {
	if x == 1 {
		return 1
	}
	return 1 - math.Pow(2, -10*x)
}

// EaseInOutExpo is an easing function that starts slow, speeds up, then slows
// down again, following an exponential curve.
func EaseInOutExpo(x float64) float64 {
	if x == 0 {
		return 0
	}
	if x == 1 {
		return 1
	}
	if x < 0.5 {
		return math.Pow(2, 20*x-10) / 2
	}
	return (2 - math.Pow(2, -20*x+10)) / 2
}

// EaseInCirc is an easing function that starts slow and speeds up, following a
// circular curve.
func EaseInCirc(x float64) float64 {
	return 1 - math.Sqrt(1-math.Pow(x, 2))
}

// EaseOutCirc is an easing function that starts fast and slows down, following a
// circular curve.
func EaseOutCirc(x float64) float64 {
	return math.Sqrt(1 - math.Pow(x-1, 2))
}

// EaseInOutCirc is an easing function that starts slow, speeds up, then slows
// down again, following a circular curve.
func EaseInOutCirc(x float64) float64 {
	if x < 0.5 {
		return (1 - math.Sqrt(1-math.Pow(2*x, 2))) / 2
	}
	return (math.Sqrt(1-math.Pow(-2*x+2, 2)) + 1) / 2
}

// EaseInElastic is an easing function that starts slow and speeds up, with an
// elastic bounce.
func EaseInElastic(x float64) float64 {
	c4 := (2 * math.Pi) / 3

	if x == 0 {
		return 0
	}
	if x == 1 {
		return 1
	}
	return -math.Pow(2, 10*(x-1)) * math.Sin((x-1.1)*c4)
}

// EaseOutElastic is an easing function that starts fast and slows down, with an
// elastic bounce.
func EaseOutElastic(x float64) float64 {
	c4 := (2 * math.Pi) / 3

	if x == 0 {
		return 0
	}
	if x == 1 {
		return 1
	}
	return math.Pow(2, -10*x)*math.Sin((x-0.1)*c4) + 1
}

// EaseInOutElastic is an easing function that starts slow, speeds up, then slows
// down again, with an elastic bounce.
func EaseInOutElastic(x float64) float64 {
	c5 := (2 * math.Pi) / 4.5

	if x == 0 {
		return 0
	}
	if x == 1 {
		return 1
	}
	if x < 0.5 {
		return -(math.Pow(2, 20*x-10) * math.Sin((20*x-11.125)*c5)) / 2
	}
	return (math.Pow(2, -20*x+10) * math.Sin((20*x-11.125)*c5)) / 2
}

// EaseInBack is an easing function that starts slow and speeds up, overshooting
// the target and then settling back with a bouncing motion.
func EaseInBounce(x float64) float64 {
	return 1 - EaseOutBounce(1-x)
}

// EaseOutBack is an easing function that starts fast and slows down, overshooting
// the target and then settling back with a bouncing motion.
func EaseOutBounce(x float64) float64 {
	n1 := 7.5625
	d1 := 2.75

	switch {
	case x < 1/d1:
		return n1 * x * x
	case x < 2/d1:
		return n1*(x-1.5/d1)*(x-1.5/d1) + 0.75
	case x < 2.5/d1:
		return n1*(x-2.25/d1)*(x-2.25/d1) + 0.9375
	}
	return n1*(x-2.625/d1)*(x-2.625/d1) + 0.984375
}

// EaseInOutBack is an easing function that starts slow, speeds up, then slows
// down again, overshooting the target and then settling back with a bouncing
// motion.
func EaseInOutBounce(x float64) float64 {
	if x < 0.5 {
		return (1 - EaseOutBounce(1-2*x)) / 2
	}
	return (1 + EaseOutBounce(2*x-1)) / 2
}

// Linear is an easing function that is a straight line, meaning no easing at
// all. When no easing function is specified, this is the default.
func Linear(x float64) float64 {
	return x
}

package main

import (
	"fmt"
	"math"
)

const (
	imageWidth  float64 = 320
	imageHeight float64 = 320
	panelWidth  float64 = 32
	panelHeight float64 = 16
	inc         float64 = 360 / (10 * 30)
)

var (
	imageCenterX float64 = imageWidth / 2
	imageCenterY float64 = imageHeight / 2

	panelCenterX float64 = panelWidth / 2
	panelCenterY float64 = panelHeight / 2

	radiusX float64 = imageWidth/2 - panelWidth/2
	radiusY float64 = imageHeight/2 - panelHeight/2
)

func main() {
	fmt.Println("uint16_t key_frames[] = {")
	var theta float64
	k := 0
	for theta = 0; theta < 359.99; theta += inc {
		k++
		x := math.Cos(toRad(theta))*radiusX + imageCenterX - panelCenterX
		y := math.Sin(toRad(theta))*radiusY + imageCenterY - panelCenterY
		fmt.Printf("    %.0f, %.0f, // k=%d, Θ=%0.1f°\n", math.Round(x), math.Round(y), k, theta)
	}
	fmt.Println("};")
}

func toRad(deg float64) float64 {
	return math.Pi * 2 * deg / 360
}

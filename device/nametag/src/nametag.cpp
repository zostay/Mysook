#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO
#include <Color.h>
#include "nametag.h"

char message[] = "Hello my name is Sterling!";

const mysook::Color primaryColors[] = {
    mysook::Color(0, 0, 255),
    mysook::Color(0, 255, 0), 
    mysook::Color(255, 0, 0), 
};

const mysook::Color colors[] = {
    mysook::Color(255, 0, 0),   //S red
    mysook::Color(255, 125, 0), //t orange
    mysook::Color(200, 255, 0), //e yellowish
    mysook::Color(0, 255, 0),   //r green
    mysook::Color(0, 255, 225), //l blue
    mysook::Color(150, 0, 255), //i purple
    mysook::Color(255, 0, 220), //n pink
    mysook::Color(255, 65, 0),  //g reddish
    mysook::Color(255, 220, 0)  //! orange/yellow
};

#define BRIGHTNESS 20

void NameTag::start() {
    matrix.set_brightness(BRIGHTNESS);

    for (byte i = 0; i < 3; i++) {
        matrix.set_fg(primaryColors[i % 9]);
        matrix.fill_screen();
        matrix.draw();
        delay(500);
    }
}

void NameTag::tick() {
    matrix.fill_screen_bg();
    matrix.set_cursor(x, 5);
    for (byte i = 0; i < sizeof(message); i++) {
        // set the color
        matrix.set_fg(colors[i % 9]);
        // write the letter
        matrix.put_char(message[i]);
    }

    if (--x < -100) {
        x = matrix.panel_width();
    }

    matrix.draw();
}

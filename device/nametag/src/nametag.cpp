#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO
#include <Color.h>
#include "nametag.h"
#include "ops.h"

char message[] = "Hello my name is Sterling!";

uint32_t basic_program[] = {
    OP_SUB, PROGRAM_MAIN,
    OP_PUSH, 10, OP_URGENCY,
    OP_PUSH, 100, OP_BRIGHTNESS,
    OP_PUSH, 0xFFFF00, OP_MASKGROUND,
    OP_PUSH, 0xFF0000, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, 0x00FF00, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, 0x0000FF, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, PROGRAM_MAIN, OP_GOTO,
    OP_HALT, OP_HALT,
};

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
//    matrix.set_brightness(BRIGHTNESS);
//
//    for (byte i = 0; i < 3; i++) {
//        matrix.set_fg(primaryColors[i % 9]);
//        matrix.fill_screen();
//        matrix.draw();
//        delay(500);
//    }

    web_server.begin();
}

void NameTag::tick() {
//    matrix.fill_screen_bg();
//    matrix.set_cursor(x, 5);
//    for (byte i = 0; i < sizeof(message); i++) {
//        // set the color
//        matrix.set_fg(colors[i % 9]);
//        // write the letter
//        matrix.put_char(message[i]);
//    }
//
//    if (--x < -100) {
//        x = matrix.panel_width();
//    }
//
//    matrix.draw();
}

#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO
#include <Color.h>
#include "nametag.h"
#include "ops.h"

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
//    matrix.set_brightness(BRIGHTNESS);
//
//    for (byte i = 0; i < 3; i++) {
//        matrix.set_fg(primaryColors[i % 9]);
//        matrix.fill_screen();
//        matrix.draw();
//        delay(500);
//    }

    web_server.begin();
    vm->begin();
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
    //logf_ln("D [nametag] TICK");

    if (vm->has_halted()) {
        logf_ln("W [nametag] VM halt detected, returning to built-in program");

        if (web_binary) {
            delete web_binary;
            web_binary = 0;
            this->remove_post_ticker(vm);
            delete vm;
        }

        vm = &builtin;
        vm->begin();
        this->add_post_ticker(vm);
    }
}

bool NameTag::handle_web_request(mysook::Request &req, mysook::Response &res) {
    const String &body = req.body();
    if (body.length() < 16) {
        res.status(400, "Bad Request");
        res.header("Content_Type", "text/plain");
        res.body("Program descriptors must be at least 16 bytes long.");
        return true;
    }

    if (body.length() % 4 != 0) {
        res.status(400, "Bad Request");
        res.header("Content-Type", "text/plain");
        res.body("Program descriptors must be provided in 4 byte increments.");
        return true;
    }

    uint32_t *binary = new uint32_t[ 2 + body.length() / 4 ];

    int i = 0;
    for (int c = 0; c < body.length(); ++c) {
        char ch = body.charAt(c);
        //logf_ln("READ %02X", ch);
        switch (c % 4) {
        case 0:
            binary[i] = ch << 0x18;
            break;
        case 1:
            binary[i] |= ch << 0x10;
            break;
        case 2:
            binary[i] |= ch << 0x08;
            break;
        case 3:
            binary[i] |= ch;
            //logf_ln("0x%08X", binary[i]);
            ++i;
            break;
        }
    }

    binary[i++] = OP_HALT;
    binary[i++] = OP_HALT;

    this->remove_post_ticker(vm);

    if (web_binary) {
        logf_ln("I [nametag] stopping existng web program");

        delete vm;
        delete web_binary;
    }

    logf_ln("I [nametag] starting new web program");

    uint32_t start    = binary[0];
    uint32_t *program = binary + 1;

    web_binary  = binary;
    vm = new VM<12,6>(this->log, matrix, program, start);
    vm->begin();
    this->add_post_ticker(vm);

    String OK("OK");
    res.status(200, OK);
    
    return true;
}

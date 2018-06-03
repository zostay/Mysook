#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO
#include <Color.h>
#include "nametag.h"
#include "ops.h"

char message[] = "Hello my name is Sterling!";

#define REG_USER(n)  (REG_USER0 + n)

uint32_t basic_program[] = {
    OP_SUB, PROGRAM_MAIN,
    OP_PUSH, 10, OP_URGENCY,
    OP_PUSH, 20, OP_BRIGHTNESS,
    OP_PUSH, 0xFFFF00, OP_MASKGROUND,
    OP_PUSH, 0xFF0000, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, 0x00FF00, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, 0x0000FF, OP_FOREGROUND, OP_FILL, OP_TICK,
    OP_PUSH, PROGRAM_MAIN, OP_GOTO,
    OP_HALT, OP_HALT,
//    OP_PUSH, PROGRAM_PILES_INIT, OP_GOTO,

    OP_SUB, PROGRAM_PILES_INIT,

    // R10 <- 0
    OP_PUSH, 0, 
    OP_PUSH, REG_USER(10), 
    OP_SET,

    // BEGIN LOOP
    OP_SUB, LOOP_START_PILES_INIT,
    
    // IF R10 >= WIDTH * HEIGHT: JUMP TO END LOOP
    OP_PUSH, REG_USER(10), 
    OP_GET, 
    OP_WIDTH,
    OP_HEIGHT,
    OP_MUL,
    OP_GE,
    OP_PUSH, LOOP_END_PILES_INIT, 
    OP_CMP,

    // R[R10 + 100] <- 0
    OP_PUSH, 0, 
    OP_PUSH, REG_USER(10), 
    OP_GET, 
    OP_PUSH, 100, 
    OP_ADD, 
    OP_SET,                

    // R10 <- R10 + 1
    OP_PUSH, REG_USER(10), 
    OP_GET, 
    OP_PUSH, 1, 
    OP_ADD, 
    OP_PUSH, REG_USER(10), 
    OP_SET,                  

    // JUMP TO BEGIN LOOP
    OP_PUSH, LOOP_START_PILES_INIT, 
    OP_GOTO,

    // END LOOP
    OP_SUB, LOOP_END_PILES_INIT,
    OP_RETURN,

    OP_SUB, PROGRAM_PILES,

    // URGENCY <- 2
    OP_PUSH, 2, OP_URGENCY,

    // CALL PILES_INIT
    OP_PUSH, PROGRAM_PILES_INIT, 
    OP_GOSUB,

    // R10(X) <- rand() % WIDTH
    OP_RAND, 
    OP_WIDTH,
    OP_MOD, 
    OP_PUSH, REG_USER(10), 
    OP_SET, 

    // R11(Y) <- rand() % HEIGHT
    OP_RAND, 
    OP_HEIGHT,
    OP_MOD, 
    OP_PUSH, REG_USER(11), 
    OP_SET,

    // R12(X,Y) <- R10(X) + R11(Y) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_WIDTH,
    OP_MUL,
    OP_ADD,
    OP_PUSH, REG_USER(12),
    OP_SET,

    // R[R12(X,Y)] <- R[R12(X,Y)] + 1
    OP_PUSH, REG_USER(12), 
    OP_GET, 
    OP_GET, 
    OP_PUSH, 1, 
    OP_ADD, 
    OP_PUSH, REG_USER(12), 
    OP_GET, 
    OP_SET,

    // R11(Y) <- 0
    OP_PUSH, 0,
    OP_PUSH, REG_USER(11),
    OP_SET,

    // BEGIN LOOP Y
    OP_SUB, LOOP_START_PILES_Y,

    // R11(Y) < HEIGHT OR JUMP TO END LOOP Y
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_HEIGHT,
    OP_LT,
    OP_PUSH, LOOP_END_PILES_Y,
    OP_CMP,

    // R10(X) <- 0
    OP_PUSH, 0,
    OP_PUSH, REG_USER(10),
    OP_SET,

    // BEGIN LOOP X
    OP_SUB, LOOP_START_PILES_X,

    // R10(X) < WIDTH OR JUMP TO END LOOP X
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_WIDTH,
    OP_LT,
    OP_PUSH, LOOP_END_PILES_X,
    OP_CMP,

    // R12(X,Y) <- R10(X) + R11(Y) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_WIDTH,
    OP_MUL,
    OP_ADD,
    OP_PUSH, REG_USER(12),
    OP_SET,

    // IF R[R12(X,Y)] >= MAX_HEIGHT(4): CALL PROGRAM_TOPPLE
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_GET,
    OP_PUSH, 4,
    OP_GE,
    OP_PUSH, PROGRAM_TOPPLE,
    OP_CMPSUB,

    // IF R[R12(X,Y)] == 0: DRAW BLACK
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_GET,
    OP_PUSH, 0,
    OP_NE,
    OP_PUSH, SKIP_BLACK,
    OP_CMP,
    OP_PUSH, 0x000000, OP_FOREGROUND,
    OP_PUSH, SKIP_RED, OP_GOTO,
    OP_SUB, SKIP_BLACK,

    // IF R[R12(X,Y)] == 1: DRAW BLUE
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_NE,
    OP_PUSH, SKIP_BLUE,
    OP_CMP,
    OP_PUSH, 0x0000FF, OP_FOREGROUND,
    OP_PUSH, SKIP_RED, OP_GOTO,
    OP_SUB, SKIP_BLUE,

    // IF R[R12(X,Y)] == 2: DRAW YELLOW
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_NE,
    OP_PUSH, SKIP_YELLOW,
    OP_CMP,
    OP_PUSH, 0xFFFF00, OP_FOREGROUND,
    OP_PUSH, SKIP_RED, OP_GOTO,
    OP_SUB, SKIP_YELLOW,

    // IF R[R12(X,Y)] == 3: DRAW RED
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_NE,
    OP_PUSH, SKIP_RED,
    OP_CMP,
    OP_PUSH, 0xFF0000, OP_FOREGROUND,
    OP_SUB, SKIP_RED,

    // DRAW PIXEL
    OP_PUSH, REG_USER(10),
    OP_PUSH, REG_USER(11),
    OP_PIXEL,

    // R10(X) = R10(X) + 1
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(10),
    OP_SET,

    // CONTINUE LOOP X
    OP_GOTO, LOOP_START_PILES_X,

    // END LOOP X
    OP_SUB, LOOP_END_PILES_X,

    // R11(Y) = R11(Y) + 1
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(11),
    OP_SET,

    // CONTINUE LOOP Y
    OP_GOTO, LOOP_START_PILES_Y,

    // END LOOP Y
    OP_SUB, LOOP_END_PILES_Y,

    // TICK
    OP_TICK,

    // DO IT AGAIN
    OP_PUSH, PROGRAM_PILES,
    OP_GOTO,

    OP_SUB, PROGRAM_TOPPLE,

    // R[R12(X,Y)] <- 0
    OP_PUSH, 0,
    OP_PUSH, REG_USER(12),
    OP_GET,
    OP_SET,

    // IF R10(X) > 0:
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, 0,
    OP_LE,
    OP_PUSH, SKIP_RAISE_X_MINUS_1,
    OP_CMP,

    // R13(X-1,Y) = R10(X) - 1 + R11(Y) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, 1,
    OP_MIN,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_WIDTH,
    OP_MUL,
    OP_PUSH, REG_USER(13),
    OP_SET,

    // R[R13(X-1,Y)] <- R[R13(X-1,Y)] + 1
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_X_MINUS_1,
    
    // IF R11(Y) > 0:
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_PUSH, 0,
    OP_LE,
    OP_PUSH, SKIP_RAISE_Y_MINUS_1,
    OP_CMP,

    // R13(X,Y-1) = R10(X) + (R11(Y) - 1) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_PUSH, 1,
    OP_MIN,
    OP_WIDTH,
    OP_MUL,
    OP_PUSH, REG_USER(13),
    OP_SET,

    // R[R13(X,Y-1)] <- R[R13(X,Y-1)] + 1
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_Y_MINUS_1,

    // IF R10(X) < WIDTH - 1:
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_WIDTH,
    OP_PUSH, 1,
    OP_MIN,
    OP_GE,
    OP_PUSH, SKIP_RAISE_X_PLUS_1,
    OP_CMP,

    // R13(X+1,Y) = R10(X) + 1 + R11(Y) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_WIDTH,
    OP_MUL,
    OP_PUSH, REG_USER(13),
    OP_SET,

    // R[R13(X+1,Y)] <- R[R13(X+1,Y)] + 1
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_X_PLUS_1,
    
    // IF R11(Y) < HEIGHT - 1:
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_HEIGHT,
    OP_PUSH, 1,
    OP_MIN,
    OP_GE,
    OP_PUSH, SKIP_RAISE_Y_PLUS_1,
    OP_CMP,

    // R13(X,Y+1) = R10(X) + (R11(Y) + 1) * WIDTH
    OP_PUSH, REG_USER(10),
    OP_GET,
    OP_PUSH, REG_USER(11),
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_WIDTH,
    OP_MUL,
    OP_PUSH, REG_USER(13),
    OP_SET,

    // R[R13(X,Y+1)] <- R[R13(X,Y+1)] + 1
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, REG_USER(13),
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_Y_PLUS_1,

    // RETURN
    OP_RETURN,

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

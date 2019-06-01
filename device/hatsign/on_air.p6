use v6;

.sub: "mask-A", {
    .data:
        0x18, # ...XX...
        0x3c, # ..XXXX..
        0x66, # .XX..XX.
        0x66, # .XX..XX.
        0x7e, # .XXXXXX.
        0x66, # .XX..XX.
        0x66, # .XX..XX.
        ;
};

.sub: "mask-I", {
    .data:
        0x3c, # ..XXXX..
        0x3c, # ..XXXX..
        0x18, # ...XX...
        0x18, # ...XX...
        0x18, # ...XX...
        0x18, # ...XX...
        0x3c, # ..XXXX..
        0x3c, # ..XXXX..
        ;
}

.sub: "mask-N", {
    .data:
        0xc3, # XX....XX
        0xe3, # XXX...XX
        0xf3, # XXXX..XX
        0xdb, # XX.XX.XX
        0xcf, # XX..XXXX
        0xc7, # XX...XXX
        0xc3, # XX....XX
        0xc3, # XX....XX
        ;
}

.sub: "mask-O", {
    .data:
        0x3c, # ..XXXX..
        0x66, # .XX..XX.
        0xc3, # XX....XX
        0xc3, # XX....XX
        0xc3, # XX....XX
        0xc3, # XX....XX
        0x66, # .XX..XX.
        0x3c, # ..XXXX..
        ;
}

.sub: "mask-R", {
    .data:
        0xfc, # XXXXXX..
        0x63, # .XX...XX
        0x63, # .XX...XX
        0x7e, # .XXXXXX.
        0x7e, # .XXXXXX.
        0x63, # .XX...XX
        0x63, # .XX...XX
        0xe3, # XXX...XX
        ;
}

.sub: "throb-red", {
    .urgency: 1;
    .foreground: 0xFF0000; # red
    .fill;
    .tick-mode: MODE_BRIGHTNESS, {
        .data:
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
            0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
            0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
            ;
    }
}

.sub: "spell-ONAIR", {
    .urgency: 10;
    .brightness: ON_AIR_BRIGHTNESS;
    .foreground: 0xFF0000; # red
    .fill;
    .tick-mode: MODE_MASK_BITS, {
        .call: "mask-O";
        .call: "mask-N";
        .call: "mask-A";
        .call: "mask-I";
        .call: "mask-R";
    }
}

.sub: "police-vert", {
    .urgency: 1;
    .brightness: ON_AIR_BRIGHTNESS;
    .foreground: 0xFF0000; # red
    .fill-columns: 0xF0;
    .foreground: 0x0000FF; # blue
    .fill-columns: 0x0F;
}

.sub: "police-horz", {
    .urgency: 1;
    .brightness: ON_AIR_BRIGHTNESS;
    .foreground: 0xFF0000; # red
    .fill-rows: 0xF0;
    .foreground: 0x0000FF; # blue
    .fill-rows: 0x0F;
}

.sub "triple-flash", {
    .data: MODE_MASK;
    .stack-or;
    .tick-mode:
    PROGRAM_TRIPLE_FLASH, OP_SUB,
    MODE_MASK, OP_OR, OP_TICK_MODE_BEGIN,
    0x00, 0xf0, 0x00, 0xf0, 0x00, 0xf0,
    0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f,
    0x00, 0xf0, 0x00, 0xf0, 0x00, 0xf0,
    0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f,
    0x00, 0xf0, 0x00, 0xf0, 0x00, 0xf0,
    0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f,
    OP_TICK_MODE_END,
    OP_RETURN,

    PROGRAM_POLICE_VERT_TRIPLE_FLASH, OP_SUB,
    PROGRAM_POLICE_VERT, OP_GOSUB,
    PROGRAM_TRIPLE_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_POLICE_HORZ_TRIPLE_FLASH, OP_SUB,
    PROGRAM_POLICE_HORZ, OP_GOSUB,
    PROGRAM_TRIPLE_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_PARLLEL_FLASH, OP_SUB,
    MOD_MASK, OP_OR, OP_TICK_MODE_BEGIN,
    0x00, 0xcc, 0x00, 0xcc, 0x00, 0xcc,
    0x00, 0x33, 0x00, 0x33, 0x00, 0x33,
    0x00, 0xcc, 0x00, 0xcc, 0x00, 0xcc,
    0x00, 0x33, 0x00, 0x33, 0x00, 0x33,
    0x00, 0xcc, 0x00, 0xcc, 0x00, 0xcc,
    0x00, 0x33, 0x00, 0x33, 0x00, 0x33,
    OP_TICK_MODE_END,
    OP_RETURN,

    PROGRAM_POLICE_VERT_PARALLEL_FLASH, OP_SUB,
    PROGRAM_POLICE_VERT, OP_GOSUB,
    PROGRAM_PARALLEL_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_POLICE_HORZ_PARALLEL_FLASH, OP_SUB,
    PROGRAM_POLICE_HORZ, OP_GOSUB,
    PROGRAM_PARALLEL_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_BEBUBABO_FLASH, OP_SUB,
    MOD_MASK, OP_OR, OP_TICK_MODE_BEGIN,
    0x00, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10, 0x08,
    0x00, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10, 0x08,
    0x00, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10, 0x08,
    0x00, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10, 0x08,
    0x00, 0x80, 0x02, 0x20, 0x04, 0x40, 0x01, 0x10, 0x08,
    OP_TICK_MODE_END,
    OP_RETURN,

    PROGRAM_POLICE_VERT_BEBUBABO_FLASH, OP_SUB,
    PROGRAM_POLICE_VERT, OP_GOSUB,
    PROGRAM_BEBUBABO_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_POLICE_HORZ_BEBUBABO_FLASH, OP_SUB,
    PROGRAM_POLICE_HORZ, OP_GOSUB,
    PROGRAM_BEBUBABO_FLASH, OP_GOSUB,
    OP_RETURN,

    PROGRAM_EMPTY, OP_SUB,
    0, OP_BRIGHTNESS,
    0, 0, 0, OP_FOREGROUND,
    OP_FILL,
    OP_TICK,
    PROGRAM_EMPTY, OP_GOTO,

    PROGRAM_ON_AIR, OP_SUB,
    PROGRAM_RED_THROB, OP_GOSUB,
    PROGRAM_SPELL_ONAIR, OP_GOSUB,
    PROGRAM_RED_THROB, OP_GOSUB,
    PROGRAM_ON_AIR, OP_GOTO,

    PROGRAM_HEY_THERE, OP_SUB,
    PROGRAM_POLICE_VERT_TRIPLE_FLASH, OP_GOSUB,
    PROGRAM_POLICE_HORZ_TRIPLE_FLASH, OP_GOSUB,
    PROGRAM_SPELL_ONAIR, OP_GOSUB,
    PROGRAM_POLICE_VERT_PARALLEL_FLASH, OP_GOSUB,
    PROGRAM_POLICE_HORZ_PARALLEL_FLASH, OP_GOSUB,
    PROGRAM_SPELL_ONAIR, OP_GOSUB,
    PROGRAM_POLICE_VERT_BEBUBABO_FLASH, OP_GOSUB,
    PROGRAM_POLICE_HORZ_BEBUBABO_FLASH, OP_GOSUB,
    PROGRAM_SPELL_ONAIR, OP_GOSUB,
    PROGRAM_HEY_THERE, OP_GOTO,

    /* IT BECOMES CLEAR THAT I NEED SOME RANDOM STACK ACCESS
    PROGRAM_PILES_GET, OP_SUB,
    8, OP_MUL, OP_ADD, 100, OP_ADD, OP_GET,
    OP_RETURN,

    PROGRAM_PILES_ADD, OP_SUB,
    8, OP_MUL, OP_ADD, 100, OP_ADD, OP_DUP, OP_GET,



    PROGRAM_PILES_INIT, OP_SUB,
    10, 0, OP_SET,                                      // R10 <- 0
    LOOP_START_PILES_INIT, OP_SUB,                      // BEGIN LOOP
    10, OP_GET, 64, OP_LT, LOOP_END_PILES_INIT, OP_CMP, // R10 < 64 OR END LOOP
    10, OP_GET, 100, OP_ADD, 0, OP_SET,                 // R[R10 + 100] <- 0
    10, OP_GET, 1, OP_ADD, 10, OP_SET,                  // R10 <- R10 + 1
    LOOP_START_PILES_INIT, OP_GOTO,                     // CONTINUE LOOP
    LOOP_END_PILES_INIT, OP_SUB,                        // END LOOP
    OP_RETURN,

    PROGRAM_PILES, OP_SUB,
    2, OP_URGENCY, // URGENCY <- 2
    PROGRAM_PILES_INIT, OP_GOSUB, // CALL PILES_INIT
    OP_RAND, 8, OP_MOD, 10, OP_SET, // R10/X <- rand() % 8
    OP_RAND, 8, OP_MOD, 11, OP_SET, // R11/Y <- rand() % 8

    10, OP_GET, OP_GET, 1, OP_ADD, 10, OP_GET, OP_SET, // R[R10] <- R[R10] + 1
    10, 0, OP_SET, // R10 <- 0
    LOOP_START_PILES_Y, OP_SUB, // BEGIN LOOP Y
    10, OP_GET, 8, OP_LT, LOOP_END_PILES_Y, OP_CMP, // R10 < 8 OR END LOOP Y
    11, 0, OP_SET, // R11 <- 0
    LOOP_START_PILES_X, OP_SUB, // BEGIN LOOP X
    11, OP_GET, 8, OP_LT, LOOP_END_PILES_X, OP_CMP, // R11 < 8 OR END LOOP X
    10, OP_GET, 8, OP_MUL, 11, OP_GET, OP_ADD, 12, OP_SET // R12 <- R10 * 8 + R11
    12, OP_GET, 100, OP_ADD, OP_GET, OP_LE, 3, LOOP_START_PILES_MAX_HEIGHT, OP_CMP, // IF R[R12 + 100] > MAX_HEIGHT THEN
    12, OP_GET, 100, OP_ADD, 12, OP_GET, 100, OP_ADD, OP_GET, 4, OP_MIN, OP_SET, // R[R12 + 100] <- R[R12 + 100] - (MAX_HEIGHT + 1)
    11, OP_GET, 0, OP_LE, COND_END_PILES_GY_GT_0, OP_CMP, // IF R11 > 0 THEN
    11, OP_GET, 1, OP_MIN, 8, OP_MUL, 12, OP_GET, OP_ADD, OP_GET, 1, OP_ADD
    11, OP_GET, 1, OP_MIN, 8, OP_MUL, 12, OP_GET, OP_ADD, OP_GET, 1, OP_ADD
    COND_END_PILES_MAX_HEIGHT, OP_GOTO,
    COND_END_PILES_GY_GT_0, OP_SUB,
    COND_END_PILES_GY_LT_7, OP_SUB,
    COND_END_PILES_GX_GT_0, OP_SUB,
    COND_END_PILES_GX_LT_Y, OP_SUB,
    COND_END_PILES_MAX_HEIGHT, OP_SUB, // END IF MAX_HEIGHT
    LOOP_START_PILES_X, OP_GOTO, // CONTINUE LOOP X
    LOOP_END_PILES_X, OP_SUB, // END LOOP X
    LOOP_START_PILES_Y, OP_GOTO, // CONTINUE LOOP Y
    LOOP_END_PILES_Y, OP_SUB, // END LOOP Y
    PROGRAM_PILES, OP_GOTO,
    */

    OP_STOP
};

OnAirGrid::OnAirGrid(Panel *panel, int program_selector)
: Grid(panel) {

    unsigned char *selected_program;
    int length;

    switch (program_selector) {

    case ON_AIR_PROGRAM:
        selected_program = on_air_program;
        length = sizeof(on_air_program) / sizeof(unsigned char);
        break;

    case EMERGENCY_PROGRAM:
        selected_program = emergency_program;
        length = sizeof(emergency_program) / sizeof(unsigned char);
        break;

    default:
        selected_program = on_air_program;
        length = sizeof(on_air_program) / sizeof(unsigned char);
        break;

    }

    init(selected_program, length);
}

OnAirGrid::OnAirGrid(Panel *panel, const unsigned char *program, int length)
: Grid(panel) {
    init(program, length);
}

void OnAirGrid::init(const unsigned char *program, int length) {
    current_program = CUSTOM_PROGRAM;

    this->program = new unsigned char[length];
    memcpy(
        reinterpret_cast<void*>(this->program),
        reinterpret_cast<const void*>(program),
        sizeof(unsigned char) * length
    );

    set_brightness(BRIGHTNESS);
}

OnAirGrid::~OnAirGrid() { }

void OnAirGrid::loop(long tick) {
    if (on_air_mode & FLAG_URGENT)
        urgency = 1;
    else
        urgency = 10;

    if (tick % urgency != 0) return;

    if (program[on_air_ptr] == OP_STOP
            && program[on_air_ptr+1] == OP_STOP
            && program[on_air_ptr+2] == OP_STOP) {
        on_air_ptr = 0;
    }

    if (on_air_ptr >= program_length) {
        on_air_ptr = 0;
    }

    if (program[on_air_ptr] == OP_STOP
            && program[on_air_ptr+1] == OP_STOP) {
        previous_mode_flags = current_mode_flags;
        current_mod_flags = program[on_air_ptr+2];
        on_air_ptr += 3;

        set_brightness(BRIGHTNESS);
    }

    unsigned char op = program[on_air_ptr++];
    unsigned char mode_type = on_air_mode & FLAG_TYPE;
    if (mode_type == FLAG_VERT_RED_BLUE) { // vert red/blue
        for (int gy = 0; gy < 8; ++gy) {
            for (unsigned int gx = 0, mask = 0x80; gx < 8; ++gx, mask >>= 1) {
                //printf("%02x & %02x\n", op, mask);
                if (op & mask && gx < 4)
                    set_color(gx, gy, color_red);
                else if (op & mask)
                    set_color(gx, gy, color_blue);
                else
                    set_color(gx, gy, color_black);
            }
        }
    }

    else if (mode_type == 0x02) { // horz red/blue
        for (int gx = 0; gx < 8; ++gx) {
            for (unsigned int gy = 0, mask = 0x80; gy < 8; ++gy, mask >>= 1) {
                if (op & mask && gy < 4)
                    set_color(gx, gy, color_red);
                else if (op & mask)
                    set_color(gx, gy, color_blue);
                else
                    set_color(gx, gy, color_black);
            }
        }
    }

    else if (mode_type == 0x03) { // red bitmap
        for (int gy = 0; gy < 8; ++gy) {
            for (unsigned int gx = 0, mask = 0x80; gx < 8; ++gx, mask >>= 1) {
                if (op & mask) {
                    set_color(gx, gy, color_red);
                    //printf("#");
                }
                else {
                    set_color(gx, gy, color_black);
                    //printf(" ");
                }
            }
            //printf("\n");

            if (gy < 7)
                op = program[on_air_ptr++];
        }
        //printf("\n");
    }

    else if (mode_type == 0x04) { // red square, adjust brightness
        set_brightness(BRIGHTNESS * op / 255);

        for (int gy = 0; gy < 8; ++gy) {
            for (int gx = 0; gx < 8; ++gx) {
                set_color(gx, gy, color_red);
            }
        }
    }

    else {
        for (int gy = 0; gy < 8; ++gy) {
            for (int gx = 0; gx < 8; ++gx) {
                set_color(gx, gy, color_red);
            }
        }
    }
}

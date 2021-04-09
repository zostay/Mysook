#define ON_AIR_BRIGHTNESS 255

uint32_t built_in_programs[] = {
    OP_SUB, PROGRAM_MASK_A, 
    OP_PUSH, 0x18, // ...XX...
    OP_PUSH, 0x3c, // ..XXXX..
    OP_PUSH, 0x66, // .XX..XX.
    OP_PUSH, 0x66, // .XX..XX.
    OP_PUSH, 0x7e, // .XXXXXX.
    OP_PUSH, 0x66, // .XX..XX.
    OP_PUSH, 0x66, // .XX..XX.
    OP_RETURN,

    OP_SUB, PROGRAM_MASK_I,
    OP_PUSH, 0x3c, // ..XXXX..
    OP_PUSH, 0x3c, // ..XXXX..
    OP_PUSH, 0x18, // ...XX...
    OP_PUSH, 0x18, // ...XX...
    OP_PUSH, 0x18, // ...XX...
    OP_PUSH, 0x18, // ...XX...
    OP_PUSH, 0x3c, // ..XXXX..
    OP_PUSH, 0x3c, // ..XXXX..
    OP_RETURN,

    OP_SUB, PROGRAM_MASK_N,
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0xe3, // XXX...XX
    OP_PUSH, 0xf3, // XXXX..XX
    OP_PUSH, 0xdb, // XX.XX.XX
    OP_PUSH, 0xcf, // XX..XXXX
    OP_PUSH, 0xc7, // XX...XXX
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0xc3, // XX....XX
    OP_RETURN,

    OP_SUB, PROGRAM_MASK_O,
    OP_PUSH, 0x3c, // ..XXXX..
    OP_PUSH, 0x66, // .XX..XX.
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0xc3, // XX....XX
    OP_PUSH, 0x66, // .XX..XX.
    OP_PUSH, 0x3c, // ..XXXX..
    OP_RETURN,

    OP_SUB, PROGRAM_MASK_R,
    OP_PUSH, 0xfc, // XXXXXX..
    OP_PUSH, 0x63, // .XX...XX
    OP_PUSH, 0x63, // .XX...XX
    OP_PUSH, 0x7e, // .XXXXXX.
    OP_PUSH, 0x7e, // .XXXXXX.
    OP_PUSH, 0x63, // .XX...XX
    OP_PUSH, 0x63, // .XX...XX
    OP_PUSH, 0xe3, // XXX...XX
    OP_RETURN,

    OP_SUB, PROGRAM_THROB_RED,
    OP_PUSH, 1,
    OP_URGENCY,
    OP_PUSH, 0xFF0000, OP_FOREGROUND, // red
    OP_FILL,
    OP_MARK,
    OP_PUSH, 0x00, OP_PUSH, 0x11, OP_PUSH, 0x22, OP_PUSH, 0x33, 
    OP_PUSH, 0x44, OP_PUSH, 0x55, OP_PUSH, 0x66, OP_PUSH, 0x77, 
    OP_PUSH, 0x88, OP_PUSH, 0x99, OP_PUSH, 0xAA, OP_PUSH, 0xBB, 
    OP_PUSH, 0xCC, OP_PUSH, 0xDD, OP_PUSH, 0xEE, OP_PUSH, 0xFF,
    OP_PUSH, 0xFF, OP_PUSH, 0xEE, OP_PUSH, 0xDD, OP_PUSH, 0xCC, 
    OP_PUSH, 0xBB, OP_PUSH, 0xAA, OP_PUSH, 0x99, OP_PUSH, 0x88,
    OP_PUSH, 0x77, OP_PUSH, 0x66, OP_PUSH, 0x55, OP_PUSH, 0x44, 
    OP_PUSH, 0x33, OP_PUSH, 0x22, OP_PUSH, 0x11, 
    OP_PUSH, MODE_BRIGHTNESS, OP_TICK_MODE,
    OP_RETURN,

    OP_SUB, PROGRAM_SPELL_ONAIR,
    OP_PUSH, 10, OP_URGENCY,
    OP_PUSH, ON_AIR_BRIGHTNESS, OP_BRIGHTNESS,
    OP_PUSH, 0xFF0000, OP_FOREGROUND, // red
    OP_FILL,
    OP_MARK,
    OP_PUSH, PROGRAM_MASK_O, OP_GOSUB, 
    OP_PUSH, PROGRAM_MASK_O, OP_GOSUB, 
    OP_PUSH, PROGRAM_MASK_N, OP_GOSUB, 
    OP_PUSH, PROGRAM_MASK_A, OP_GOSUB, 
    OP_PUSH, PROGRAM_MASK_I, OP_GOSUB, 
    OP_PUSH, PROGRAM_MASK_R, OP_GOSUB,
    OP_PUSH, MOD_MASK_BITS, OP_TICK_MODE,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_VERT,
    OP_PUSH, 1, OP_URGENCY,
    OP_PUSH, ON_AIR_BRIGHTNESS, OP_BRIGHTNESS,
    OP_PUSH, 0xFF0000, OP_FOREGROUND,
    OP_PUSH, 0xF0, OP_FILL_COLUMNS,
    OP_PUSH, 0x0000FF, OP_FOREGROUND,
    OP_PUSH, 0x0F, OP_FILL_COLUMNS,
    OP_PUSH, MODE_COLUMNS,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_HORZ,
    OP_PUSH, 1, OP_URGENCY,
    OP_PUSH, ON_AIR_BRIGHTNESS, OP_BRIGHTNESS,
    OP_PUSH, 0xFF0000, OP_FOREGROUND,
    OP_PUSH, 0x0000FF, OP_BACKGROUND,
    OP_PUSH, 0xF0, OP_FILL_ROWS,
    OP_PUSH, MODE_ROWS, 
    OP_RETURN,

    OP_SUB, PROGRAM_TRIPLE_FLASH,
    OP_PUSH, MODE_MASK, OP_OR, OP_PUSH, 11, OP_SET,
    OP_MARK,
    OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, 
    OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f,
    OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, 
    OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f,
    OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, OP_PUSH, 0x00, OP_PUSH, 0xf0, 
    OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f, OP_PUSH, 0x00, OP_PUSH, 0x0f,
    OP_PUSH, 11, OP_GET, OP_TICK_MODE,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_VERT_TRIPLE_FLASH,
    OP_PUSH, PROGRAM_POLICE_VERT, OP_GOSUB,
    OP_PUSH, PROGRAM_TRIPLE_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_HORZ_TRIPLE_FLASH,
    OP_PUSH, PROGRAM_POLICE_HORZ, OP_GOSUB,
    OP_PUSH, PROGRAM_TRIPLE_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_PARLLEL_FLASH, 
    OP_PUSH, MODE_MASK, OP_OR, OP_PUSH, 11, OP_SET,
    OP_MARK,
    OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc,
    OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33,
    OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc,
    OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33,
    OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc, OP_PUSH, 0x00, OP_PUSH, 0xcc,
    OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33, OP_PUSH, 0x00, OP_PUSH, 0x33,
    OP_PUSH, 11, OP_GET, OP_TICK_MODE,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_VERT_PARALLEL_FLASH,
    OP_PUSH, PROGRAM_POLICE_VERT, OP_GOSUB,
    OP_PUSH, PROGRAM_PARALLEL_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_HORZ_PARALLEL_FLASH,
    OP_PUSH, PROGRAM_POLICE_HORZ, OP_GOSUB,
    OP_PUSH, PROGRAM_PARALLEL_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_BEBUBABO_FLASH,
    OP_PUSH, MODE_MASK, OP_OR, OP_PUSH, 11, OP_SET,
    OP_MARK,
    OP_PUSH, 0x00, OP_PUSH, 0x80, OP_PUSH, 0x02, OP_PUSH, 0x20, OP_PUSH, 0x04, OP_PUSH, 0x40, OP_PUSH, 0x01, OP_PUSH, 0x10, OP_PUSH, 0x08,
    OP_PUSH, 0x00, OP_PUSH, 0x80, OP_PUSH, 0x02, OP_PUSH, 0x20, OP_PUSH, 0x04, OP_PUSH, 0x40, OP_PUSH, 0x01, OP_PUSH, 0x10, OP_PUSH, 0x08,
    OP_PUSH, 0x00, OP_PUSH, 0x80, OP_PUSH, 0x02, OP_PUSH, 0x20, OP_PUSH, 0x04, OP_PUSH, 0x40, OP_PUSH, 0x01, OP_PUSH, 0x10, OP_PUSH, 0x08,
    OP_PUSH, 0x00, OP_PUSH, 0x80, OP_PUSH, 0x02, OP_PUSH, 0x20, OP_PUSH, 0x04, OP_PUSH, 0x40, OP_PUSH, 0x01, OP_PUSH, 0x10, OP_PUSH, 0x08,
    OP_PUSH, 0x00, OP_PUSH, 0x80, OP_PUSH, 0x02, OP_PUSH, 0x20, OP_PUSH, 0x04, OP_PUSH, 0x40, OP_PUSH, 0x01, OP_PUSH, 0x10, OP_PUSH, 0x08,
    OP_PUSH, 11, OP_GET, OP_TICK_MODE,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_VERT_BEBUBABO_FLASH,
    OP_PUSH, PROGRAM_POLICE_VERT, OP_GOSUB,
    OP_PUSH, PROGRAM_BEBUBABO_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_POLICE_HORZ_BEBUBABO_FLASH,
    OP_PUSH, PROGRAM_POLICE_HORZ, OP_GOSUB,
    OP_PUSH, PROGRAM_BEBUBABO_FLASH, OP_GOSUB,
    OP_RETURN,

    OP_SUB, PROGRAM_EMPTY,
    OP_PUSH, 0, OP_BRIGHTNESS, 
    OP_PUSH, 0x000000, OP_FOREGROUND, 
    OP_FILL, 
    OP_TICK,
    OP_SUB, PROGRAM_EMPTY, OP_GOTO,

    OP_SUB, PROGRAM_ON_AIR,
    OP_PUSH, PROGRAM_RED_THROB, OP_GOSUB,
    OP_PUSH, PROGRAM_SPELL_ONAIR, OP_GOSUB,
    OP_PUSH, PROGRAM_RED_THROB, OP_GOSUB,
    OP_PUSH, PROGRAM_ON_AIR, OP_GOTO,

    OP_SUB, PROGRAM_HEY_THERE, 
    OP_PUSH, PROGRAM_POLICE_VERT_TRIPLE_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_POLICE_HORZ_TRIPLE_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_SPELL_ONAIR, OP_GOSUB,
    OP_PUSH, PROGRAM_POLICE_VERT_PARALLEL_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_POLICE_HORZ_PARALLEL_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_SPELL_ONAIR, OP_GOSUB,
    OP_PUSH, PROGRAM_POLICE_VERT_BEBUBABO_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_POLICE_HORZ_BEBUBABO_FLASH, OP_GOSUB,
    OP_PUSH, PROGRAM_SPELL_ONAIR, OP_GOSUB,
    OP_PUSH, PROGRAM_HEY_THERE, OP_GOTO,

    OP_SUB, PROGRAM_PILES_INIT,

    // R10 <- 0
    OP_PUSH, 0, 
    OP_PUSH, 10, 
    OP_SET,

    // BEGIN LOOP
    OP_SUB, LOOP_START_PILES_INIT,
    
    // R10 < 64 OR JUMP TO END LOOP
    OP_PUSH, 10, 
    OP_GET, 
    OP_PUSH, 64, 
    OP_LT,
    OP_PUSH, LOOP_END_PILES_INIT, 
    OP_CMP,

    // R[R10 + 100] <- 0
    OP_PUSH, 0, 
    OP_PUSH, 10, 
    OP_GET, 
    OP_PUSH, 100, 
    OP_ADD, 
    OP_SET,                

    // R10 <- R10 + 1
    OP_PUSH, 10, 
    OP_GET, 
    OP_PUSH, 1, 
    OP_ADD, 
    OP_PUSH, 10, 
    OP_SET,                  

    // JUMP TO BEGIN LOOP
    OP_PUSH, LOOP_START_PILES_INIT, 
    OP_GOTO,

    // END LOOP
    OP_SUB, LOOP_END_PILES_INIT,
    OP_RETURN,

    OP_SUB, PROGRAM_PILES,

    // URGENCY <- 2
    OP_PUSH, 2, OP_URGENCY

    // CALL PILES_INIT
    OP_PUSH, PROGRAM_PILES_INIT, 
    OP_GOSUB,

    // R10(X) <- rand() % 8
    OP_RAND, 
    OP_PUSH, 8, 
    OP_MOD, 
    OP_PUSH, 10, 
    OP_SET, 

    // R11(Y) <- rand() % 8
    OP_RAND, 
    OP_PUSH, 8, 
    OP_MOD, 
    OP_PUSH, 11, 
    OP_SET,

    // R12(X,Y) <- R10(X) + R11(Y) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 8
    OP_MUL,
    OP_ADD,
    OP_PUSH, 12,
    OP_SET,

    // R[R12(X,Y)] <- R[R12(X,Y)] + 1
    OP_PUSH, 12, 
    OP_GET, 
    OP_GET, 
    OP_PUSH, 1, 
    OP_ADD, 
    OP_PUSH, 12, 
    OP_GET, 
    OP_SET,

    // R11(Y) <- 0
    OP_PUSH, 0,
    OP_PUSH, 11,
    OP_SET,

    // BEGIN LOOP Y
    OP_SUB, LOOP_START_PILES_Y,

    // R11(Y) < 8 OR JUMP TO END LOOP Y
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 8,
    OP_LT,
    OP_PUSH, LOOP_END_PILES_Y,
    OP_CMP,

    // R10(X) <- 0
    OP_PUSH, 0,
    OP_PUSH, 10,
    OP_SET,

    // BEGIN LOOP X
    OP_SUB, LOOP_START_PILES_X,

    // R10(X) < 8 OR JUMP TO END LOOP X
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 8,
    OP_LT,
    OP_PUSH, LOOP_END_PILES_X,
    OP_CMP,

    // R12(X,Y) <- R10(X) + R11(Y) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 8,
    OP_MUL,
    OP_ADD,
    OP_PUSH, 12,
    OP_SET,

    // IF R[R12(X,Y)] >= MAX_HEIGHT(4): CALL PROGRAM_TOPPLE
    OP_PUSH, 12,
    OP_GET,
    OP_GET,
    OP_PUSH, 4,
    OP_GE,
    OP_PUSH, PROGRAM_TOPPLE,
    OP_CMPSUB,

    // R10(X) = R10(X) + 1
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 10,
    OP_SET,

    // CONTINUE LOOP X
    OP_GOTO, LOOP_START_PILES_X,

    // END LOOP X
    OP_SUB, LOOP_END_PILES_X,

    // R11(Y) = R11(Y) + 1
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 11,
    OP_SET,

    // CONTINUE LOOP Y
    OP_GOTO, LOOP_START_PILES_Y,

    // END LOOP Y
    OP_SUB, LOOP_END_PILES_Y,

    // DO IT AGAIN
    OP_PUSH, PROGRAM_PILES,
    OP_GOTO,

    OP_SUB, PROGRAM_TOPPLE,

    // R[R12(X,Y)] <- 0
    OP_PUSH, 0,
    OP_PUSH, 12,
    OP_GET,
    OP_SET,

    // IF R10(X) > 0:
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 0,
    OP_LE,
    OP_PUSH, SKIP_RAISE_X_MINUS_1,
    OP_CMP,

    // R13(X-1,Y) = R10(X) - 1 + R11(Y) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 1,
    OP_MIN,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 8,
    OP_MUL,
    OP_PUSH, 13,
    OP_SET,

    // R[R13(X-1,Y)] <- R[R13(X-1,Y)] + 1
    OP_PUSH, 13,
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 13,
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_X_MINUS_1,
    
    // IF R11(Y) > 0:
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 0,
    OP_LE,
    OP_PUSH, SKIP_RAISE_Y_MINUS_1,
    OP_CMP,

    // R13(X,Y-1) = R10(X) + (R11(Y) - 1) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 1,
    OP_MIN,
    OP_PUSH, 8,
    OP_MUL,
    OP_PUSH, 13,
    OP_SET,

    // R[R13(X,Y-1)] <- R[R13(X,Y-1)] + 1
    OP_PUSH, 13,
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 13,
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_Y_MINUS_1,

    // IF R10(X) < 7:
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 7,
    OP_GE,
    OP_PUSH, SKIP_RAISE_X_PLUS_1,
    OP_CMP,

    // R13(X+1,Y) = R10(X) + 1 + R11(Y) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 8,
    OP_MUL,
    OP_PUSH, 13,
    OP_SET,

    // R[R13(X+1,Y)] <- R[R13(X+1,Y)] + 1
    OP_PUSH, 13,
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 13,
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_X_PLUS_1,
    
    // IF R11(Y) < 7:
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 7,
    OP_GE,
    OP_PUSH, SKIP_RAISE_Y_PLUS_1,
    OP_CMP,

    // R13(X,Y+1) = R10(X) + (R11(Y) + 1) * 8
    OP_PUSH, 10,
    OP_GET,
    OP_PUSH, 11,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 8,
    OP_MUL,
    OP_PUSH, 13,
    OP_SET,

    // R[R13(X,Y+1)] <- R[R13(X,Y+1)] + 1
    OP_PUSH, 13,
    OP_GET,
    OP_GET,
    OP_PUSH, 1,
    OP_ADD,
    OP_PUSH, 13,
    OP_GET,
    OP_SET,

    // END IF
    OP_SUB, SKIP_RAISE_Y_PLUS_1,

    // RETURN
    OP_RETURN,

    OP_STOP,
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

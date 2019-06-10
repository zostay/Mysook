unit module NameTag;

enum VMTickMode is export (
    MODE_NONE         => 0x0000,
    MODE_BRIGHTNESS   => 0x0001,

    MODE_MASK         => 0x00B1,
    MODE_MASK_ROWS    => 0x20B1,
    MODE_MASK_COLUMNS => 0x40B1,
    MODE_MASK_BITS    => 0x80B1,

    MODE_ROWS         => 0x2000,
    MODE_COLUMNS      => 0x4000,
    MODE_BITS         => 0x8000,
);

enum VMOp is export (
    OP_HALT             => 0xDEAD,
    OP_SUB              => 0x0001,
    OP_RETURN           => 0x0002,
    OP_GOTO             => 0x0003,
    OP_GOSUB            => 0x0004,
    OP_TICK             => 0x0005,
    OP_NOOP             => 0x0008,

    OP_CMP              => 0x0011,
    OP_CMPSUB           => 0x0012,

    OP_EQ               => 0x0021,
    OP_LT               => 0x0022,
    OP_LE               => 0x0023,
    OP_GT               => 0x0024,
    OP_GE               => 0x0025,
    OP_NE               => 0x0026,

    OP_ADD              => 0x0031,
    OP_MIN              => 0x0032,
    OP_MUL              => 0x0033,
    OP_DIV              => 0x0034,
    OP_MOD              => 0x0035,

    OP_AND              => 0x0041,
    OP_OR               => 0x0042,
    OP_XOR              => 0x0043,
    OP_NOT              => 0x0044,
    OP_BSL              => 0x0045,
    OP_BSR              => 0x0046,
    OP_COMP             => 0x0047,

    OP_RAND             => 0x0051,
    OP_WIDTH            => 0x0052,
    OP_HEIGHT           => 0x0053,

    OP_SET              => 0x0081,
    OP_GET              => 0x0082,
    # OP_UNARY_SET        => 0x0083,
    # OP_UNARY_GET        => 0x0084,
    OP_TICK_MODE        => 0x0083,
    OP_URGENCY          => 0x0183,
    OP_BRIGHTNESS       => 0x0283,
    OP_FOREGROUND       => 0x0383,
    OP_BACKGROUND       => 0x0483,
    OP_MASKGROUND       => 0x0583,
    OP_MARK             => 0x0683,

    OP_FILL             => 0x00A1,
    OP_FILL_ROWS        => 0x20A1,
    OP_FILL_COLUMNS     => 0x40A1,
    OP_FILL_BITS        => 0x80A1,
    OP_PIXEL            => 0x00A2,

    OP_MASK_ROWS        => 0x20B1,
    OP_MASK_COLUMNS     => 0x40B1,
    OP_MASK_BITS        => 0x80B1,

    OP_PUSH             => 0x00C1,
    OP_POP              => 0x00C2,
    OP_DUP              => 0x00C3,
    OP_SWAP             => 0x00C5,
    OP_READ             => 0x80C8,
    OP_WRITE            => 0x80C9,
    OP_ALLOC            => 0x00CA,
    OP_READARG          => 0x80CB,

    OP_PUTCHAR          => 0x00D1,
    OP_SET_CURSOR       => 0x00D2,
);

enum VMRegister is export (
    REG_TICK_MODE          => 0,
    REG_URGENCY            => 1,
    REG_BRIGHTNESS         => 2,
    REG_FOREGROUND_COLOR   => 3,
    REG_BACKGROUND_COLOR   => 4,
    REG_MASKGROUND_COLOR   => 5,
    REG_MARK               => 6,
    REG_STACK_SEGMENT      => 7,
    REG_CURSOR_X           => 8,
    REG_CURSOR_Y           => 9,
    REG_USER0              => 16,
    REG_USER239            => 255,
);


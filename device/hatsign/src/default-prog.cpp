#include <inttypes.h>
#include "ops.h"

// Sterling - ZipRecruiter
// uint32_t PROGRAM_MAIN = 1;
// uint32_t default_program[] = {
//     OP_SUB, 1, OP_PUSH, 16777215, OP_PUSH, 0, OP_PUSH, 0, OP_PUSH, 2, OP_URGENCY,
//     OP_PUSH, 75, OP_BRIGHTNESS, OP_SUB, 2, OP_RAND, OP_PUSH, 12632256, OP_OR,
//     OP_PUSH, 0, OP_WRITE, OP_WIDTH, OP_PUSH, 2, OP_WRITE, OP_SUB, 4, OP_PUSH, 0,
//     OP_FOREGROUND, OP_FILL, OP_PUSH, 0, OP_READ, OP_FOREGROUND, OP_PUSH, 2,
//     OP_READ, OP_PUSH, 5, OP_SET_CURSOR, OP_PUSH, 83, OP_PUTCHAR, OP_PUSH, 116,
//     OP_PUTCHAR, OP_PUSH, 101, OP_PUTCHAR, OP_PUSH, 114, OP_PUTCHAR, OP_PUSH, 108,
//     OP_PUTCHAR, OP_PUSH, 105, OP_PUTCHAR, OP_PUSH, 110, OP_PUTCHAR, OP_PUSH, 103,
//     OP_PUTCHAR, OP_PUSH, 2, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 2, OP_WRITE,
//     OP_TICK, OP_PUSH, REG_CURSOR_X, OP_GET, OP_PUSH, 2147483648, OP_GT, OP_NOT,
//     OP_PUSH, 6, OP_CMP, OP_PUSH, 5, OP_GOTO, OP_SUB, 6, OP_PUSH, 4, OP_GOTO,
//     OP_SUB, 5, OP_TICK, OP_TICK, OP_PUSH, 13434777, OP_PUSH, 0, OP_WRITE,
//     OP_WIDTH, OP_PUSH, 2, OP_WRITE, OP_SUB, 7, OP_PUSH, 0, OP_FOREGROUND, OP_FILL,
//     OP_PUSH, 0, OP_READ, OP_FOREGROUND, OP_PUSH, 2, OP_READ, OP_PUSH, 5,
//     OP_SET_CURSOR, OP_PUSH, 90, OP_PUTCHAR, OP_PUSH, 105, OP_PUTCHAR, OP_PUSH,
//     112, OP_PUTCHAR, OP_PUSH, 82, OP_PUTCHAR, OP_PUSH, 101, OP_PUTCHAR, OP_PUSH,
//     99, OP_PUTCHAR, OP_PUSH, 114, OP_PUTCHAR, OP_PUSH, 117, OP_PUTCHAR, OP_PUSH,
//     105, OP_PUTCHAR, OP_PUSH, 116, OP_PUTCHAR, OP_PUSH, 101, OP_PUTCHAR, OP_PUSH,
//     114, OP_PUTCHAR, OP_PUSH, 2, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 2,
//     OP_WRITE, OP_TICK, OP_PUSH, REG_CURSOR_X, OP_GET, OP_PUSH, 2147483648, OP_GT,
//     OP_NOT, OP_PUSH, 9, OP_CMP, OP_PUSH, 8, OP_GOTO, OP_SUB, 9, OP_PUSH, 7,
//     OP_GOTO, OP_SUB, 8, OP_TICK, OP_TICK, OP_PUSH, 2, OP_GOTO, OP_SUB, 3,
//     OP_RETURN, OP_HALT, OP_HALT
// };

// Async Programming
// uint32_t PROGRAM_MAIN = 1;
// uint32_t default_program[] = {
//     OP_SUB, 1, OP_PUSH, 16777215, OP_PUSH, 0, OP_PUSH, 2, OP_URGENCY, OP_PUSH,
//     75, OP_BRIGHTNESS, OP_WIDTH, OP_PUSH, 1, OP_WRITE, OP_SUB, 2, OP_PUSH, 0,
//     OP_FOREGROUND, OP_FILL, OP_PUSH, 0, OP_READ, OP_FOREGROUND, OP_PUSH, 1,
//     OP_READ, OP_PUSH, 6, OP_SET_CURSOR, OP_PUSH, 65, OP_PUTCHAR, OP_PUSH, 83,
//     OP_PUTCHAR, OP_PUSH, 89, OP_PUTCHAR, OP_PUSH, 78, OP_PUTCHAR, OP_PUSH, 67,
//     OP_PUTCHAR, OP_PUSH, 32, OP_PUTCHAR, OP_PUSH, 80, OP_PUTCHAR, OP_PUSH, 82,
//     OP_PUTCHAR, OP_PUSH, 79, OP_PUTCHAR, OP_PUSH, 71, OP_PUTCHAR, OP_PUSH, 82,
//     OP_PUTCHAR, OP_PUSH, 65, OP_PUTCHAR, OP_PUSH, 77, OP_PUTCHAR, OP_PUSH, 77,
//     OP_PUTCHAR, OP_PUSH, 73, OP_PUTCHAR, OP_PUSH, 78, OP_PUTCHAR, OP_PUSH, 71,
//     OP_PUTCHAR, OP_PUSH, 32, OP_PUTCHAR, OP_PUSH, 73, OP_PUTCHAR, OP_PUSH, 78,
//     OP_PUTCHAR, OP_PUSH, 32, OP_PUTCHAR, OP_PUSH, 16711680, OP_FOREGROUND,
//     OP_PUSH, 80, OP_PUTCHAR, OP_PUSH, 16776960, OP_FOREGROUND, OP_PUSH, 69,
//     OP_PUTCHAR, OP_PUSH, 65280, OP_FOREGROUND, OP_PUSH, 82, OP_PUTCHAR,
//     OP_PUSH, 65535, OP_FOREGROUND, OP_PUSH, 76, OP_PUTCHAR, OP_PUSH, 32,
//     OP_PUTCHAR, OP_PUSH, 255, OP_FOREGROUND, OP_PUSH, 54, OP_PUTCHAR, OP_PUSH,
//     1, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 1, OP_WRITE, OP_TICK, OP_PUSH,
//     REG_CURSOR_X, OP_GET, OP_PUSH, 2147483648, OP_GT, OP_NOT, OP_PUSH, 4,
//     OP_CMP, OP_WIDTH, OP_PUSH, 1, OP_WRITE, OP_SUB, 4, OP_PUSH, 2, OP_GOTO,
//     OP_SUB, 3, OP_RETURN, OP_HALT, OP_HALT
// };

// Sterling's Awful Soldering
// uint32_t PROGRAM_MAIN = 1;
// uint32_t default_program[] = {
//     OP_SUB, 1, OP_PUSH, 16777215, OP_PUSH, 0, OP_PUSH, 2, OP_URGENCY, OP_PUSH,
//     75, OP_BRIGHTNESS, OP_WIDTH, OP_PUSH, 1, OP_WRITE, OP_SUB, 2, OP_PUSH, 0,
//     OP_FOREGROUND, OP_FILL, OP_PUSH, 0, OP_READ, OP_FOREGROUND, OP_PUSH, 1,
//     OP_READ, OP_PUSH, 6, OP_SET_CURSOR, OP_PUSH, 97, OP_PUTCHAR, OP_PUSH, 108,
//     OP_PUTCHAR, OP_PUSH, 115, OP_PUTCHAR, OP_PUSH, 111, OP_PUTCHAR, OP_PUSH,
//     32, OP_PUTCHAR, OP_PUSH, 83, OP_PUTCHAR, OP_PUSH, 116, OP_PUTCHAR,
//     OP_PUSH, 101, OP_PUTCHAR, OP_PUSH, 114, OP_PUTCHAR, OP_PUSH, 108,
//     OP_PUTCHAR, OP_PUSH, 105, OP_PUTCHAR, OP_PUSH, 110, OP_PUTCHAR, OP_PUSH,
//     103, OP_PUTCHAR, OP_PUSH, 39, OP_PUTCHAR, OP_PUSH, 115, OP_PUTCHAR,
//     OP_PUSH, 32, OP_PUTCHAR, OP_PUSH, REG_CURSOR_X, OP_GET, OP_PUSH, 5,
//     OP_RAND, OP_PUSH, 3, OP_MOD, OP_ADD, OP_SET_CURSOR, OP_PUSH, 16711680,
//     OP_FOREGROUND, OP_PUSH, 65, OP_PUTCHAR, OP_PUSH, REG_CURSOR_X, OP_GET,
//     OP_PUSH, 5, OP_RAND, OP_PUSH, 3, OP_MOD, OP_ADD, OP_SET_CURSOR, OP_PUSH,
//     16711680, OP_FOREGROUND, OP_PUSH, 87, OP_PUTCHAR, OP_PUSH, REG_CURSOR_X,
//     OP_GET, OP_PUSH, 5, OP_RAND, OP_PUSH, 3, OP_MOD, OP_ADD, OP_SET_CURSOR,
//     OP_PUSH, 16711680, OP_FOREGROUND, OP_PUSH, 70, OP_PUTCHAR, OP_PUSH,
//     REG_CURSOR_X, OP_GET, OP_PUSH, 5, OP_RAND, OP_PUSH, 3, OP_MOD, OP_ADD,
//     OP_SET_CURSOR, OP_PUSH, 16711680, OP_FOREGROUND, OP_PUSH, 85, OP_PUTCHAR,
//     OP_PUSH, 32, OP_PUTCHAR, OP_PUSH, REG_CURSOR_X, OP_GET, OP_PUSH, 5,
//     OP_RAND, OP_PUSH, 3, OP_MOD, OP_ADD, OP_SET_CURSOR, OP_PUSH, 16711680,
//     OP_FOREGROUND, OP_PUSH, 76, OP_PUTCHAR, OP_PUSH, REG_CURSOR_X, OP_GET,
//     OP_PUSH, 6, OP_SET_CURSOR, OP_PUSH, 0, OP_READ, OP_FOREGROUND, OP_PUSH,
//     32, OP_PUTCHAR, OP_PUSH, 115, OP_PUTCHAR, OP_PUSH, 111, OP_PUTCHAR,
//     OP_PUSH, 108, OP_PUTCHAR, OP_PUSH, 100, OP_PUTCHAR, OP_PUSH, 101,
//     OP_PUTCHAR, OP_PUSH, 114, OP_PUTCHAR, OP_PUSH, 105, OP_PUTCHAR, OP_PUSH,
//     110, OP_PUTCHAR, OP_PUSH, 103, OP_PUTCHAR, OP_PUSH, 1, OP_READ, OP_PUSH,
//     1, OP_MIN, OP_PUSH, 1, OP_WRITE, OP_TICK, OP_PUSH, REG_CURSOR_X, OP_GET,
//     OP_PUSH, 2147483648, OP_GT, OP_NOT, OP_PUSH, 4, OP_CMP, OP_WIDTH, OP_PUSH,
//     1, OP_WRITE, OP_SUB, 4, OP_PUSH, 2, OP_GOTO, OP_SUB, 3, OP_RETURN,
//     OP_HALT, OP_HALT
// };

// Piles
uint32_t PROGRAM_MAIN = 6;
uint32_t default_program[] = {
    OP_SUB, 1, OP_PUSH, 3, OP_READARG, OP_PUSH, 0, OP_EQ, OP_NOT, OP_PUSH, 2,
    OP_CMP, OP_PUSH, 0, OP_FOREGROUND, OP_SUB, 2, OP_PUSH, 3, OP_READARG,
    OP_PUSH, 1, OP_EQ, OP_NOT, OP_PUSH, 3, OP_CMP, OP_PUSH, 255,
    OP_FOREGROUND, OP_SUB, 3, OP_PUSH, 3, OP_READARG, OP_PUSH, 2, OP_EQ,
    OP_NOT, OP_PUSH, 4, OP_CMP, OP_PUSH, 16776960, OP_FOREGROUND, OP_SUB, 4,
    OP_PUSH, 3, OP_READARG, OP_PUSH, 3, OP_EQ, OP_NOT, OP_PUSH, 5, OP_CMP,
    OP_PUSH, 16711680, OP_FOREGROUND, OP_SUB, 5, OP_PUSH, 1, OP_READARG,
    OP_PUSH, 2, OP_READARG, OP_PIXEL, OP_RETURN, OP_SUB, 6, OP_PUSH, 0,
    OP_PUSH, 0, OP_PUSH, 0, OP_WIDTH, OP_HEIGHT, OP_MUL, OP_PUSH, 0, OP_SWAP,
    OP_ALLOC, OP_PUSH, 10, OP_BRIGHTNESS, OP_PUSH, 1, OP_URGENCY, OP_PUSH, 0,
    OP_FOREGROUND, OP_FILL, OP_TICK, OP_SUB, 7, OP_RAND, OP_WIDTH, OP_MOD,
    OP_PUSH, 0, OP_WRITE, OP_RAND, OP_HEIGHT, OP_MOD, OP_PUSH, 1, OP_WRITE,
    OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_READ, OP_WIDTH, OP_MUL, OP_ADD,
    OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ, OP_PUSH,
    3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3, OP_ADD,
    OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1,
    OP_READ, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_GOSUB, OP_POP, OP_POP,
    OP_POP, OP_PUSH, 0, OP_PUSH, 1, OP_WRITE, OP_SUB, 9, OP_PUSH, 1, OP_READ,
    OP_HEIGHT, OP_GE, OP_NOT, OP_PUSH, 11, OP_CMP, OP_PUSH, 10, OP_GOTO,
    OP_SUB, 11, OP_PUSH, 0, OP_PUSH, 0, OP_WRITE, OP_SUB, 12, OP_PUSH, 0,
    OP_READ, OP_WIDTH, OP_GE, OP_NOT, OP_PUSH, 14, OP_CMP, OP_PUSH, 13,
    OP_GOTO, OP_SUB, 14, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_READ, OP_WIDTH,
    OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3,
    OP_ADD, OP_READ, OP_PUSH, 4, OP_GE, OP_NOT, OP_PUSH, 15, OP_CMP, OP_PUSH,
    2, OP_READ, OP_PUSH, 0, OP_SWAP, OP_PUSH, 3, OP_ADD, OP_WRITE, OP_PUSH, 0,
    OP_READ, OP_PUSH, 0, OP_GT, OP_NOT, OP_PUSH, 16, OP_CMP, OP_PUSH, 0,
    OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 1, OP_READ, OP_WIDTH, OP_MUL,
    OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ,
    OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3,
    OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ,
    OP_PUSH, 1, OP_READ, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 1,
    OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_SUB, 16, OP_PUSH, 1, OP_READ,
    OP_PUSH, 0, OP_GT, OP_NOT, OP_PUSH, 17, OP_CMP, OP_PUSH, 0, OP_READ,
    OP_PUSH, 1, OP_READ, OP_PUSH, 1, OP_MIN, OP_WIDTH, OP_MUL, OP_ADD,
    OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ, OP_PUSH,
    3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3, OP_ADD,
    OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1,
    OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_GOSUB,
    OP_POP, OP_POP, OP_POP, OP_SUB, 17, OP_PUSH, 0, OP_READ, OP_WIDTH,
    OP_PUSH, 1, OP_MIN, OP_LT, OP_NOT, OP_PUSH, 18, OP_CMP, OP_PUSH, 0,
    OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 1, OP_READ, OP_WIDTH, OP_MUL,
    OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ,
    OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3,
    OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ,
    OP_PUSH, 1, OP_READ, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 1,
    OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_SUB, 18, OP_PUSH, 1, OP_READ,
    OP_HEIGHT, OP_PUSH, 1, OP_MIN, OP_LT, OP_NOT, OP_PUSH, 19, OP_CMP,
    OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_READ, OP_PUSH, 1, OP_ADD, OP_WIDTH,
    OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2,
    OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP,
    OP_PUSH, 3, OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD,
    OP_READ, OP_PUSH, 1, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 0, OP_READ,
    OP_PUSH, 1, OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_SUB, 19, OP_SUB, 15,
    OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 0, OP_WRITE, OP_PUSH,
    12, OP_GOTO, OP_SUB, 13, OP_PUSH, 1, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH,
    1, OP_WRITE, OP_PUSH, 9, OP_GOTO, OP_SUB, 10, OP_TICK, OP_PUSH, 7,
    OP_GOTO, OP_SUB, 8, OP_RETURN, OP_HALT, OP_HALT
};

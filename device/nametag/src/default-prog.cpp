#include <inttypes.h>
#include "ops.h"

uint32_t default_program[] = {
    OP_SUB, 1, OP_PUSH, 3, OP_READARG, OP_PUSH, 0, OP_EQ, OP_NOT, OP_PUSH, 2,
    OP_CMP, OP_PUSH, 0, OP_FOREGROUND, OP_SUB, 2, OP_PUSH, 3, OP_READARG, OP_PUSH,
    1, OP_EQ, OP_NOT, OP_PUSH, 3, OP_CMP, OP_PUSH, 255, OP_FOREGROUND, OP_SUB, 3,
    OP_PUSH, 3, OP_READARG, OP_PUSH, 2, OP_EQ, OP_NOT, OP_PUSH, 4, OP_CMP,
    OP_PUSH, 16776960, OP_FOREGROUND, OP_SUB, 4, OP_PUSH, 3, OP_READARG, OP_PUSH,
    3, OP_EQ, OP_NOT, OP_PUSH, 5, OP_CMP, OP_PUSH, 16711680, OP_FOREGROUND,
    OP_SUB, 5, OP_PUSH, 1, OP_READARG, OP_PUSH, 2, OP_READARG, OP_PIXEL,
    OP_RETURN, OP_SUB, 6, OP_PUSH, 0, OP_PUSH, 0, OP_PUSH, 0, OP_WIDTH, OP_HEIGHT,
    OP_MUL, OP_PUSH, 0, OP_SWAP, OP_ALLOC, OP_PUSH, 10, OP_BRIGHTNESS, OP_PUSH, 1,
    OP_URGENCY, OP_PUSH, 0, OP_FOREGROUND, OP_FILL, OP_SUB, 7, OP_RAND, OP_WIDTH,
    OP_MOD, OP_PUSH, 0, OP_WRITE, OP_RAND, OP_HEIGHT, OP_MOD, OP_PUSH, 1,
    OP_WRITE, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_READ, OP_WIDTH, OP_MUL, OP_ADD,
    OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ, OP_PUSH, 3,
    OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3, OP_ADD, OP_WRITE,
    OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_READ,
    OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_PUSH, 0,
    OP_PUSH, 1, OP_WRITE, OP_SUB, 9, OP_PUSH, 1, OP_READ, OP_HEIGHT, OP_GE,
    OP_NOT, OP_PUSH, 11, OP_CMP, OP_PUSH, 10, OP_GOTO, OP_SUB, 11, OP_PUSH, 0,
    OP_PUSH, 0, OP_WRITE, OP_SUB, 12, OP_PUSH, 0, OP_READ, OP_WIDTH, OP_GE,
    OP_NOT, OP_PUSH, 14, OP_CMP, OP_PUSH, 13, OP_GOTO, OP_SUB, 14, OP_PUSH, 0,
    OP_READ, OP_PUSH, 1, OP_READ, OP_WIDTH, OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE,
    OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 4, OP_GE, OP_NOT,
    OP_PUSH, 15, OP_CMP, OP_PUSH, 2, OP_READ, OP_PUSH, 0, OP_SWAP, OP_PUSH, 3,
    OP_ADD, OP_WRITE, OP_PUSH, 0, OP_READ, OP_PUSH, 0, OP_GT, OP_NOT, OP_PUSH, 16,
    OP_CMP, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 1, OP_READ,
    OP_WIDTH, OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH,
    2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH,
    3, OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ,
    OP_PUSH, 1, OP_READ, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 1,
    OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_SUB, 16, OP_PUSH, 1, OP_READ, OP_PUSH, 0,
    OP_GT, OP_NOT, OP_PUSH, 17, OP_CMP, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_READ,
    OP_PUSH, 1, OP_MIN, OP_WIDTH, OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH,
    2, OP_READ, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1,
    OP_ADD, OP_SWAP, OP_PUSH, 3, OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH,
    3, OP_ADD, OP_READ, OP_PUSH, 1, OP_READ, OP_PUSH, 1, OP_MIN, OP_PUSH, 0,
    OP_READ, OP_PUSH, 1, OP_GOSUB, OP_POP, OP_POP, OP_POP, OP_SUB, 17, OP_PUSH, 0,
    OP_READ, OP_WIDTH, OP_PUSH, 1, OP_MIN, OP_LT, OP_NOT, OP_PUSH, 18, OP_CMP,
    OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 1, OP_READ, OP_WIDTH,
    OP_MUL, OP_ADD, OP_PUSH, 2, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2,
    OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3,
    OP_ADD, OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH,
    1, OP_READ, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 1, OP_GOSUB,
    OP_POP, OP_POP, OP_POP, OP_SUB, 18, OP_PUSH, 1, OP_READ, OP_HEIGHT, OP_PUSH,
    1, OP_MIN, OP_LT, OP_NOT, OP_PUSH, 19, OP_CMP, OP_PUSH, 0, OP_READ, OP_PUSH,
    1, OP_READ, OP_PUSH, 1, OP_ADD, OP_WIDTH, OP_MUL, OP_ADD, OP_PUSH, 2,
    OP_WRITE, OP_PUSH, 2, OP_READ, OP_PUSH, 2, OP_READ, OP_PUSH, 3, OP_ADD,
    OP_READ, OP_PUSH, 1, OP_ADD, OP_SWAP, OP_PUSH, 3, OP_ADD, OP_WRITE, OP_PUSH,
    2, OP_READ, OP_PUSH, 3, OP_ADD, OP_READ, OP_PUSH, 1, OP_READ, OP_PUSH, 1,
    OP_ADD, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_GOSUB, OP_POP, OP_POP, OP_POP,
    OP_SUB, 19, OP_SUB, 15, OP_PUSH, 0, OP_READ, OP_PUSH, 1, OP_ADD, OP_PUSH, 0,
    OP_WRITE, OP_PUSH, 12, OP_GOTO, OP_SUB, 13, OP_PUSH, 1, OP_READ, OP_PUSH, 1,
    OP_ADD, OP_PUSH, 1, OP_WRITE, OP_PUSH, 9, OP_GOTO, OP_SUB, 10, OP_TICK,
    OP_PUSH, 7, OP_GOTO, OP_SUB, 8, OP_RETURN, OP_HALT, OP_HALT
};
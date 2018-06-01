#include "vm.h"

#define OP_BINARY_EXPR(name, op) \
template <int W, int H, class P> \
void op_##name(VM<W,H,P> &p) { \
    uint32_t a = p.pop(); \
    uint32_t b = p.pop(); \
    p.push(op); \
}

#define OP_BINARY_STMT(name, op) \
template <int W, int H, class P> \
void op_##name(VM<W,H,P> &p) { \
    uint32_t a = p.pop(); \
    uint32_t b = p.pop(); \
    op; \
}

#define OP_UNARY_EXPR(name, op) \
template <int W, int, H, class P> \
void op_##name(VM<W,H,P> &p) { \
    uint32_t a = p.pop(); \
    p.push(op); \
}

#define OP_UNARY_STMT(name, op) \
template <int W, int H, class P> \
void op_##name(VM<W,H,P> &p) { \
    uint32_t a = p.pop(); \
    op; \
}

#define OP_NULLARY_EXPR(name, op) \
template <int W, int H, class P> \
void op_##name(VM<W,H,P> &p) { \
    p.push(op); \
}

#define OP_NULLARY_STMT(name, op) \
template <int W, int H, class P> \
void op_##name(VM<W,H,P> &p) { \
    op; \
}

OP_NULLARY_STMT(halt, p.halt());
OP_NULLARY_STMT(sub, p.step());
OP_UNARY_STMT(return, p.jump(a));
OP_UNARY_STMT(goto, p.jump(p.get_sub(a)));
OP_UNARY_STMT(gosub, p.jump_sub(p.get_sub(a)));
OP_NULLARY_STMT(tick, p.next_tick());
OP_UNARY_STMT(tick_mode, p.set_register(REG_TICK_MODE, a));
OP_NULLARY_STMT(tick, return);

OP_BINARY_STMT(cmp, if (!a) p.jump(p.get_sub(b)));
OP_BINARY_STMT(cmpsub, if (!a) p.jump_sub(p.get_sub(b)));

OP_BINARY_EXPR(eq, !(a == b));
OP_BINARY_EXPR(lt, !(a < b));
OP_BINARY_EXPR(le, !(a <= b));
OP_BINARY_EXPR(gt, !(a > b));
OP_BINARY_EXPR(ge, !(a >= b));
OP_BINARY_EXPR(ne, !(a != b));

OP_BINARY_EXPR(add, a + b);
OP_BINARY_EXPR(min, a - b);
OP_BINARY_EXPR(mul, a * b);
OP_BINARY_EXPR(div, a / b);
OP_BINARY_EXPR(mod, a % b);

OP_BINARY_EXPR(and, a & b);
OP_BINARY_EXPR(or, a | b);
OP_UNARY_EXPR(not, !a);

OP_NULLARY_EXPR(rand, rand());
OP_NULLARY_EXPR(width, W);
OP_NULLARY_EXPR(height, H);

OP_BINARY_STMT(set, p.set_register(a, b));
OP_UNARY_EXPR(get, p.get_register(a));
OP_UNARY_STMT(urgency, p.set_register(REG_URGENCY, a));
OP_UNARY_STMT(brightness, p.set_register(REG_BRIGHTNESS, a));
OP_UNARY_STMT(foreground, p.set_register(REG_FOREGROUND_COLOR, a));

OP_NULLARY_STMT(fill, p.fill());

template <int W, int H, class P>
void op_fill_rows(VM<W,H,P> &p) {
    uint32_t mask = p.pop();

    for (int y = 0; y < H; ++y) {
        uint32_t row_mask = mask & (0x01u << y);

        if (row_mask)
            for (int x = 0; x < W; ++x)
                p.set_pixel(x, y);
    }
}

template <int W, int H, class P>
void op_fill_columns(VM<W,H,P> &p) {
    uint32_t mask = p.pop();

    for (int x = 0; x < W; ++x) {
        uint32_t col_mask = mask & (0x01u << y);

        if (col_mask)
            for (int y = 0; y < H; ++y)
                p.set_pixel(x, y);
    }
}

template <int W, int H, class P>
void op_fill_bits(VM<W,H,P> &p) {
    uint32_t mask[H];
    for (int mp = 0; mp < H; ++mp)
        mask[mp] = p.pop();

    for (int y = 0; y < H; ++y) {
        uint32_t row_mask = mask[y];

        for (int x = 0; x < W; ++x) {
            if (row_mask & (0x01u << x)) p.set_pixel(x, y);
        }
    }
}

OP_BINARY_STMT(pixel, p.set_pixel(a, b));

template <int W, int H, class P>
void op_mask_rows(VM<W,H,P> &p) {
    uint32_t mask = p.pop();

    for (int y = 0; y < H; ++y) {
        if (mask & (0x01 << y))
            panel_bitmask[y] = 0xFFFFFFFFu;
        else
            panel_bitmask[y] = 0x00000000u;
    }
}

template <int W, int H, class P>
void op_mask_columns(VM<W,H,P> &p) {
    uint32_t mask = p.pop();

    for (int x = 0; x < p.widht(); ++x) {
        uint32_t sel_mask = 0x01 << x;
        uint32_t col_mask = mask & sel_mask;

        if (col_mask)
            for (int mp = 0; mp < H; ++mp) panel_bitmask[mp] |= col_mask;
        else
            for (int mp = 0; mp < H; ++mp) panel_bitmask[mp] &= ~sel_mask;
    }
}

template <int W, int H, class P>
void op_mask_bits(VM<W,H,P> &p) {
    for (int mp = 0; mp < H; ++mp) {
        panel_bitmask[mp] = p.pop();
    }
}

OP_NULLARY_EXPR(push, p.step());
OP_NULLARY_STMT(pop, p.pop());

template <int W, int H, class P>
void op_dup(VM<W,H,P> &p) {
    uint32_t a = p.pop();
    p.push(a);
    p.push(a);
}

template <int W, int H, class P>
void op_swap(VM<W,H,P> &p) {
    uint32_t a = p.pop();
    uint32_t b = p.pop();
    p.push(a);
    p.push(b);
}

OP_NULLARY_STMT(mark, p.set_register(REG_MARK, 0));

template <int W, int H, class P>
void VM<W,H,P>::scan_subs() {
    int i = 0;
    while (program[i] != OP_HALT) {
        if (program[i] == OP_SUB) {
            call_index[ program[i + 1] ] = i;
        }

        switch (program[i]) {
        case OP_PUSH:
        case OP_SUB:
            i += 2;
            break;

        default:
            ++i;
        }
    }
}

template <int W, int H, class P>
VM<W,H,P>::VM(uint32_t *program, P &panel) 
: panel(panel), program(program), program_ptr(pptr) { 

    scan_subs();

    for (int bmp = 0; bmp < H; ++bmp) {
        panel_bitmask[bmp] = 0xFFFFFFFF;
    }

    registers[REG_TICK_MODE]        = MODE_NONE;

    registers[REG_URGENCY]          = 10;
    registers[REG_BRIGHTNESS]       = 30;

    registers[REG_FOREGROUND_COLOR] = 0xFFFFFF; // white

    registers[REG_MARK]             = 0;
}

template <int W, int H, class P>
void VM<W,H,P>::draw() {
    for (int y = 0; y < H; ++y) {
        uint32_t mask = panel_bitmask[y];

        for (int x = 0; x < W; ++x) {
            uint32_t bitmask = mask & (0x01 << x);
            if (bitmask)
                panel->set_pixel(x, y, grid[x][y])
        }
    }

    panel.draw();
}

template <int W, int H, class P>
uint32_t VM<W,H,P>::pop() {
    if (stack_ptr <= 0) {
        // PANIC!
        p.halt();
        return 0;
    }
    else {
        registers[REG_MARK]--;
        return stack[ --stack_ptr ];
    }
}

template <int W, int H, class P>
void VM<W,H,P>::push(uint32_t v) {
    if (stack_ptr >= STACK_SIZE) {
        // PANIC!
        p.halt();
    }
    else {
        registers[REG_MARK]++;
        stack[ stack_ptr++ ] = v;
    }
}

template <int W, int H, class P>
Color VM<W,H,P>::get_register_color(int reg) {
    if (reg < 0 || reg > 255) {
        // PANIC!
        p.halt();
        return Color(0, 0, 0);
    }

    uint32_t color = registers[reg];

    uint8_t r = (color & 0xFF0000) >> 16;
    uint8_t g = (color & 0x00FF00) >> 8;
    uint8_t b = (color & 0x0000FF);

    return Color(r, g, b);
}

template <int W, int H, class P>
void VM<W,H,P>::set_register_color(int reg, Color val) {
    if (reg < 0 || reg > 255) {
        // PANIC!
        p.halt();
        return; 
    }

    uint32_t color = 0;
    color |= val.r() << 16;
    color |= val.g() << 8;
    color |= val.b();

    registers[reg] = color;
}

template <int W, int H, class P>
void VM<W,H,P>::step_exec() {
    if (halted) return;

    uint32_t mode = get_register(REG_TICK_MODE);

    OpCode *tick_mode_op = 0;

    switch (mode) {
    case MODE_NONE:
        uint32_t s = step();

        try {
            OpCodes::op_code *op_code = op_codes.at(s);
            op_code(*this);
        }
        catch (std::out_of_range e) {
            // PANIC!
            p.halt();
        }
        break;

    case MODE_BRIGHTNESS:
        tick_mode_op = op_brightness;
        break;

    case MODE_MASK_ROWS:
        tick_mode_op = op_mask_rows;
        break;

    case MODE_MASK_COLUMNS:
        tick_mode_op = op_mask_columns;
        break;

    case MODE_MASK_BITS:
        tick_mode_op = op_mask_bits;
        break;

    default:
        // PANIC!
        p.halt();
        return;
    }

    if (tick_mode_op) {
        if (get_register(REG_MARK) > 0) {
            tick_mode_op(*this);
            next_tick();
        }
        else {
            set_register(REG_TICK_MODE, MODE_NONE);
        }
    }
}

void VM::tick_exec() {
    if (halted) return;

    reset_tick();
    while (!has_ticked()) step_exec();
}

void VM::draw() {
    for (int x 


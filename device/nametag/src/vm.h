#ifndef __VM_H
#define __VM_H

#include <map>
#include <stdexcept>

#include <Color.h>
#include <Firmware.h>
#include <Panel.h>

#include "ops.h"

#define STACK_SIZE 256
#define HEAP_SIZE  256

#define PANEL_WIDTH 12
#define PANEL_HEIGHT 6

template <int W, int H>
void op_mask_columns(VM<W,H> &);
template <int W, int H>
void op_mask_rows(VM<W,H> &);

template <int W, int H>
class VM : public mysook::RGBPanel<W, H>, public mysook::Ticking {
protected:
    const uint32_t *program;
    uint32_t stack[STACK_SIZE];
    uint32_t registers[HEAP_SIZE];

    std::map<uint32_t, int> call_index;
    OpCodes<W,H> op_codes;

    mysook::RGBPanel<W, H> &panel;
    uint32_t _panel_bitmask[H];

    int program_ptr = 0;
    int stack_ptr = 0;

    int _tick = 0;

    void scan_subs();

    bool halted = false;

    friend void op_mask_columns<W,H>(VM<W,H> &);
    friend void op_mask_rows<W,H>(VM<W,H> &);
    friend void op_mask_bits<W,H>(VM<W,H> &);

public:
    VM(mysook::RGBPanel<W, H> &panel, const uint32_t *program, uint32_t start);

    const int width() const { return W; }
    const int height() const { return H; }

    virtual bool ready_for_tick(unsigned long now) { return true; }
    virtual void tick() { tick_exec(); }

    uint32_t pop();
    void push(uint32_t v);
    
    void halt() { halted = true; }

    int get_sub(uint32_t sub) { 
        try {
            return call_index.at(sub);
        }
        catch (std::out_of_range e) {
            return 0;
        }
    }

    uint16_t step() { 
        if (halted) return OP_HALT;
        return program[ program_ptr++ ]; 
    }

    void step_exec();
    void tick_exec();

    void jump_sub(int program_ptr) {
        push(program_ptr);
        jump(program_ptr);
    }

    void jump(int program_ptr) { this->program_ptr = program_ptr; }

    void reset_tick() { _tick = 0; }
    void next_tick() { _tick++; draw(); }
    bool has_ticked() { return _tick; }

    uint32_t get_register(int reg) {
        return registers[reg];
    }

    void set_register(int reg, uint32_t val) {
        registers[reg] = val;

        switch (reg) {
        case REG_BRIGHTNESS: 
            set_brightness(val); 
            break;

        case REG_FOREGROUND_COLOR: 
            this->set_fg(get_register_color(REG_FOREGROUND_COLOR)); 
            break;

        case REG_BACKGROUND_COLOR:
            this->set_bg(get_register_color(REG_BACKGROUND_COLOR));
            break;
        }
    }

    mysook::Color get_register_color(int reg);
    void set_register_color(int reg, mysook::Color val);

public:
    using mysook::RGBPanel<W,H>::put_pixel;
    using mysook::RGBPanel<W,H>::fill_screen;
    using mysook::RGBPanel<W,H>::put_char;
    using mysook::RGBPanel<W,H>::put_text;

    virtual void put_pixel(int x, int y, mysook::Color c) { panel.put_pixel(x, y, c); }
    virtual void set_brightness(uint8_t brightness) { panel.set_brightness(brightness); }
    virtual void fill_screen(mysook::Color c) { panel.fill_screen(c); }
    virtual void put_char(unsigned char c, int x, int y, mysook::Color fg, mysook::Color bg) { panel.put_char(c, x, y, fg, bg); }
    virtual void put_text(const char *t, int x, int y, mysook::Color fg, mysook::Color bg) { panel.put_text(t, x, y, fg, bg); }
    virtual void draw();
};

template <int W, int H>
VM<W,H>::VM(mysook::RGBPanel<W,H> &panel, const uint32_t *program, uint32_t start) 
: panel(panel), program(program) { 

    scan_subs();

    program_ptr = get_sub(start);

    for (int bmp = 0; bmp < H; ++bmp) {
        _panel_bitmask[bmp] = (0x01 << W) - 1;
    }

    registers[REG_TICK_MODE]        = MODE_NONE;

    registers[REG_URGENCY]          = 10;
    registers[REG_BRIGHTNESS]       = 30;

    registers[REG_FOREGROUND_COLOR] = 0xFFFFFF; // white

    registers[REG_MARK]             = 0;
}

template <int W, int H>
void VM<W,H>::scan_subs() {
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

template <int W, int H>
void VM<W,H>::step_exec() {
    if (halted) return;

    uint32_t mode = get_register(REG_TICK_MODE);

    auto tick_mode_op = op_codes.at(OP_TICK);
    bool in_tick_mode = false;

    switch (mode) {
    case MODE_NONE:
        {
            uint32_t s = step();

            try {
                auto op_code = op_codes.at(s);
                op_code(*this);
            }
            catch (std::out_of_range e) {
                // PANIC!
                halt();
            }
        }
        break;

    case MODE_BRIGHTNESS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_BRIGHTNESS);
        break;

    case MODE_MASK_ROWS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_ROWS);
        break;

    case MODE_MASK_COLUMNS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_COLUMNS);
        break;

    case MODE_MASK_BITS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_BITS);
        break;

    default:
        // PANIC!
        halt();
        return;
    }

    if (in_tick_mode) {
        if (get_register(REG_MARK) > 0) {
            tick_mode_op(*this);
            next_tick();
        }
        else {
            set_register(REG_TICK_MODE, MODE_NONE);
        }
    }
}

template <int W, int H>
void VM<W,H>::tick_exec() {
    if (halted) return;

    reset_tick();
    while (!has_ticked()) step_exec();
}

template <int W, int H>
uint32_t VM<W,H>::pop() {
    if (stack_ptr <= 0) {
        // PANIC!
        halt();
        return 0;
    }
    else {
        registers[REG_MARK]--;
        return stack[ --stack_ptr ];
    }
}

template <int W, int H>
void VM<W,H>::push(uint32_t v) {
    if (stack_ptr >= STACK_SIZE) {
        // PANIC!
        halt();
    }
    else {
        registers[REG_MARK]++;
        stack[ stack_ptr++ ] = v;
    }
}

template <int W, int H>
mysook::Color VM<W,H>::get_register_color(int reg) {
    if (reg < 0 || reg > 255) {
        // PANIC!
        halt();
        return mysook::Color(0, 0, 0);
    }

    uint32_t color = registers[reg];

    uint8_t r = (color & 0xFF0000) >> 16;
    uint8_t g = (color & 0x00FF00) >> 8;
    uint8_t b = (color & 0x0000FF);

    return mysook::Color(r, g, b);
}

template <int W, int H>
void VM<W,H>::set_register_color(int reg, mysook::Color val) {
    if (reg < 0 || reg > 255) {
        // PANIC!
        halt();
        return; 
    }

    uint32_t color = 0;
    color |= val.r() << 16;
    color |= val.g() << 8;
    color |= val.b();

    set_register(reg, color);
}

template <int W, int H>
void VM<W,H>::draw() { 
    mysook::Color mask_color = get_register_color(REG_MASK_COLOR);

    for (int y = 0; y < H; y++) {
        uint32_t row_mask = _panel_bitmask[y];

        for (int x = 0; x < W; x++) {
            uint32_t bit_mask = 0x01 << W;

            if (!(bit_mask & row_mask)) {
                put_pixel(x, y, mask_color);
            }
        }
    }

    panel.draw(); 
}

#define OP_BINARY_EXPR(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    uint32_t a = p.pop(); \
    uint32_t b = p.pop(); \
    uint32_t r = op; \
    p.push(r); \
}

#define OP_BINARY_STMT(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    uint32_t a = p.pop(); \
    uint32_t b = p.pop(); \
    op; \
}

#define OP_UNARY_EXPR(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    uint32_t a = p.pop(); \
    uint32_t r = op; \
    p.push(r); \
}

#define OP_UNARY_STMT(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    uint32_t a = p.pop(); \
    op; \
}

#define OP_NULLARY_EXPR(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    uint32_t r = op; \
    p.push(r); \
}

#define OP_NULLARY_STMT(name, op) \
template <int W, int H> \
void op_##name(VM<W,H> &p) { \
    op; \
}

OP_NULLARY_STMT(halt, p.halt())
OP_NULLARY_STMT(sub, p.step())
OP_UNARY_STMT(return, p.jump(a))
OP_UNARY_STMT(goto, p.jump(p.get_sub(a)))
OP_UNARY_STMT(gosub, p.jump_sub(p.get_sub(a)))
OP_NULLARY_STMT(tick, p.next_tick())
OP_UNARY_STMT(tick_mode, p.set_register(REG_TICK_MODE, a))
OP_NULLARY_STMT(noop, return);

OP_BINARY_STMT(cmp, if (!a) p.jump(p.get_sub(b)))
OP_BINARY_STMT(cmpsub, if (!a) p.jump_sub(p.get_sub(b)))

OP_BINARY_EXPR(eq, !(a == b))
OP_BINARY_EXPR(lt, !(a < b))
OP_BINARY_EXPR(le, !(a <= b))
OP_BINARY_EXPR(gt, !(a > b))
OP_BINARY_EXPR(ge, !(a >= b))
OP_BINARY_EXPR(ne, !(a != b))

OP_BINARY_EXPR(add, a + b)
OP_BINARY_EXPR(min, a - b)
OP_BINARY_EXPR(mul, a * b)
OP_BINARY_EXPR(div, a / b)
OP_BINARY_EXPR(mod, a % b)

OP_BINARY_EXPR(and, a & b)
OP_BINARY_EXPR(or, a | b)
OP_UNARY_EXPR(not, !a)

OP_NULLARY_EXPR(rand, rand())
OP_NULLARY_EXPR(width, W)
OP_NULLARY_EXPR(height, H)

OP_BINARY_STMT(set, p.set_register(a, b))
OP_UNARY_EXPR(get, p.get_register(a))
OP_UNARY_STMT(urgency, p.set_register(REG_URGENCY, a))
OP_UNARY_STMT(brightness, p.set_register(REG_BRIGHTNESS, a))
OP_UNARY_STMT(foreground, p.set_register(REG_FOREGROUND_COLOR, a))

OP_NULLARY_STMT(fill, p.fill_screen())

template <int W, int H>
void op_fill_rows(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << H;
    for (int y = 0; y < H; ++y) {
        uint32_t row_mask = mask & (bit_mask >> y);

        if (row_mask)
            for (int x = 0; x < W; ++x)
                p.put_pixel(x, y);
    }
}

template <int W, int H>
void op_fill_columns(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << W;
    for (int x = 0; x < W; ++x) {
        uint32_t col_mask = mask & (bit_mask >> x);

        if (col_mask)
            for (int y = 0; y < H; ++y)
                p.put_pixel(x, y);
    }
}

template <int W, int H>
void op_fill_bits(VM<W,H> &p) {
    uint32_t mask[H];
    for (int mp = 0; mp < H; ++mp)
        mask[mp] = p.pop();

    uint32_t bit_mask = 0x01 << W;
    for (int y = 0; y < H; ++y) {
        uint32_t row_mask = mask[y];

        for (int x = 0; x < W; ++x) {
            if (row_mask & (bit_mask >> x)) p.put_pixel(x, y);
        }
    }
}

OP_BINARY_STMT(pixel, p.put_pixel(a, b));

template <int W, int H>
void op_mask_rows(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << H;
    for (int y = 0; y < H; ++y) {
        if (mask & (bit_mask >> y))
            p._panel_bitmask[y] = 0xFFFFFFFFu;
        else
            p._panel_bitmask[y] = 0x00000000u;
    }
}

template <int W, int H>
void op_mask_columns(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << W;
    for (int x = 0; x < W; ++x) {
        uint32_t sel_mask = bit_mask >> x;
        uint32_t col_mask = mask & sel_mask;

        if (col_mask)
            for (int mp = 0; mp < H; ++mp) p._panel_bitmask[mp] |= col_mask;
        else
            for (int mp = 0; mp < H; ++mp) p._panel_bitmask[mp] &= ~sel_mask;
    }
}

template <int W, int H>
void op_mask_bits(VM<W,H> &p) {
    for (int mp = 0; mp < H; ++mp) {
        p._panel_bitmask[mp] = p.pop();
    }
}

OP_NULLARY_EXPR(push, p.step());
OP_NULLARY_STMT(pop, p.pop());

template <int W, int H>
void op_dup(VM<W,H> &p) {
    uint32_t a = p.pop();
    p.push(a);
    p.push(a);
}

template <int W, int H>
void op_swap(VM<W,H> &p) {
    uint32_t a = p.pop();
    uint32_t b = p.pop();
    p.push(a);
    p.push(b);
}

OP_NULLARY_STMT(mark, p.set_register(REG_MARK, 0));

#endif//__VM_H

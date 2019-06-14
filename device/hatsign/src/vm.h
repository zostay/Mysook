#ifndef __VM_H
#define __VM_H

#include <map>
#include <stdexcept>

#include <Color.h>
#include <Firmware.h>
#include <Logger.h>
#include <Panel.h>

#include "ops.h"
#include "TomThumb.h"

#ifndef ARDUINO
#include <stdlib.h>
#endif//ARDUINO

#define MAX_TIME_SLICE 50000ul

#define STACK_SIZE 4096
#define HEAP_SIZE  256

#define PANEL_WIDTH 12
#define PANEL_HEIGHT 6

#define MAX_BRIGHTNESS 20

//#define ENABLE_VM_DEBUGGING

#ifdef ENABLE_VM_DEBUGGING
#define DEBUG(fmt, ...) log.logf_ln(fmt, __VA_ARGS__)
#define TRACE(fmt, ...) log.logf_ln(fmt, __VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#define TRACE(fmd, ...)
#endif

template <int W, int H>
void op_mask_columns(VM<W,H> &);
template <int W, int H>
void op_mask_rows(VM<W,H> &);

template <int W, int H>
class VM : public mysook::RGBPanelGrid<W, H>, public mysook::TickingVariableTimer {
protected:
    const uint32_t *program;
    uint32_t stack[STACK_SIZE];
    uint32_t registers[HEAP_SIZE];

    std::map<uint32_t, int> call_index;
    OpCodes<W,H> op_codes;

    mysook::Logger &log;
    mysook::RGBPanel<W, H> &panel;
    uint32_t _panel_bitmask[H];

    int _start;

    int max_ptr = 0;
    int program_ptr = 0;
    int stack_ptr = 0;

    int _tick = 0;

    void scan_subs();

    bool halted = false;

    friend void op_halt<W,H>(VM<W,H> &);
    friend void op_mask_rows<W,H>(VM<W,H> &);
    friend void op_mask_bits<W,H>(VM<W,H> &);

    unsigned long hertz(uint32_t urgency) { return 50000ul * urgency; }
    unsigned long _tick_speed = 50000ul;

public:
    VM(mysook::Logger &log, mysook::RGBPanel<W, H> &panel, const uint32_t *program, uint32_t start);

    const int width() const { return W; }
    const int height() const { return H; }

    void begin();

    virtual unsigned long tick_speed() { return _tick_speed; }
    virtual void tick() { tick_exec(); }

    uint32_t pop();
    void push(uint32_t v);
    
    void halt() { 
        log.logf_ln("W [vm] <PP:%08X> HALT!", program_ptr);
        halted = true; 
    }

    bool has_halted() { return halted; }

    int get_sub(uint32_t sub) { 
        try {
            return call_index.at(sub);
        }
        catch (std::out_of_range e) {
            return 0;
        }
    }

    uint32_t step() { 
        if (halted) return OP_HALT;
        return program[ program_ptr++ ]; 
    }

    uint32_t peek_step() {
        if (halted) return OP_HALT;
        return program[ program_ptr ]; 
    }

    void step_exec();
    void tick_exec();

    void jump_sub(int new_program_ptr) {
        TRACE("T [vm] JUMP <PP:%08X>", new_program_ptr);
        DEBUG("D [vm] save <PP:%08X>", program_ptr);
        push(program_ptr);
        push(get_register(REG_STACK_SEGMENT));
        set_register(REG_STACK_SEGMENT, stack_ptr);
        jump(new_program_ptr);
    }

    void return_sub() {
        int new_stack_ptr = get_register(REG_STACK_SEGMENT);
        int return_ptr = read_stack(new_stack_ptr - 2);
        uint32_t segment_ptr = read_stack(new_stack_ptr - 1);

        TRACE("T [vm] RETURN <PP:%08X> TO <PP:%08X> STACK %d SS %lu", program_ptr, return_ptr, stack_ptr, segment_ptr);

        stack_ptr = new_stack_ptr - 2;
        jump(return_ptr);
        set_register(REG_STACK_SEGMENT, segment_ptr);
    }

    void jump(int new_program_ptr) { 
        if (new_program_ptr > max_ptr || new_program_ptr < 0) {
            // PANIC!
            log.logf_ln("E [vm] jump <PP:%08X> is out of the program (max <PP:%08X>)", new_program_ptr, max_ptr);
            halt();
            return;
        }

        DEBUG("D [vm] jump <PP:%08X>", new_program_ptr);
        program_ptr = new_program_ptr; 
    }

    void reset_tick() { _tick = 0; }
    void next_tick() { _tick++; draw(); }
    bool has_ticked() { return _tick; }

    uint32_t get_register(int reg) {
        if (reg < 0 || reg >= HEAP_SIZE) {
            // PANIC!
            log.logf_ln("E [vm] get illegal register %d", reg);
            halt();
            return 0;
        }

        DEBUG("D [vm] get register %d -> %d", reg, registers[reg]);

        return registers[reg];
    }

    void set_register(int reg, uint32_t val) {
        if (reg < 0 || reg >= HEAP_SIZE) {
            // PANIC!
            log.logf_ln("E [vm] set illegal register %d", reg);
            halt();
            return;
        }

        DEBUG("D [vm] set register %d <- %u", reg, val);
        registers[reg] = val;

        switch (reg) {
        case REG_URGENCY:
            _tick_speed = hertz(val);
            this->nudge_tick();
            //log.logf_ln("I [vm] tick_speed = %ld", _tick_speed);
            break;

        case REG_BRIGHTNESS: 
            {
                uint8_t capped = val > MAX_BRIGHTNESS ? MAX_BRIGHTNESS : val;
                set_brightness(capped); 
            }
            break;

        case REG_FOREGROUND_COLOR: 
            this->set_fg(get_register_color(REG_FOREGROUND_COLOR)); 
            break;

        case REG_BACKGROUND_COLOR:
            this->set_bg(get_register_color(REG_BACKGROUND_COLOR));
            break;

        case REG_STACK_SEGMENT:
            if (val >= STACK_SIZE) {
                // PANIC!
                log.logf_ln("E [vm] invalid stack segment %u", val);
                halt();
            };
            break;

        case REG_CURSOR_X:
            this->mysook::RGBPanel<W,H>::set_cursor(val, get_register(REG_CURSOR_Y));
            break;

        case REG_CURSOR_Y:
            this->mysook::RGBPanel<W,H>::set_cursor(get_register(REG_CURSOR_X), val);
            break;
        }
    }

    mysook::Color get_register_color(int reg);
    void set_register_color(int reg, mysook::Color val);

    void clear_bitmask() {
        for (int bmp = 0; bmp < H; ++bmp) {
            _panel_bitmask[bmp] = 0;
        }
    }

    void fill_bitmask() {
        for (int bmp = 0; bmp < H; ++bmp) {
            _panel_bitmask[bmp] = (0x01 << W) - 1;
        }
    }

    void apply_column_mask(uint32_t mask);

    uint32_t read_stack(int stack_ptr) {
        if (stack_ptr < 0 || stack_ptr >= STACK_SIZE) {
            // PANIC!
            log.logf_ln("E [vm] <PP:%08X> segmentation fault reading %d", program_ptr, stack_ptr);
            halt();
            return 0;
        }

        TRACE("T [vm] <PP:%08X> READ %d -> %d", program_ptr, stack_ptr, stack[stack_ptr]);
        return stack[stack_ptr];
    }

    void write_stack(int stack_ptr, uint32_t val) {
        if (stack_ptr < 0 || stack_ptr >= STACK_SIZE) {
            // PANIC!
            log.logf_ln("E [vm] <PP:%08X> segmentation fault writing %d", program_ptr, stack_ptr);
            halt();
            return;
        }

        TRACE("T [vm] <PP:%08X> WRITE %d <- %d", program_ptr, stack_ptr, val);
        stack[stack_ptr] = val;
    }

    void allocate_stack(int amount, uint32_t initializer) {
        for (int i = 0; i < amount; ++i) {
            push(initializer);
        }
    }

public:
    using mysook::RGBPanelGrid<W,H>::put_pixel;
    using mysook::RGBPanelGrid<W,H>::fill_screen;
    using mysook::RGBPanelGrid<W,H>::put_char;
    using mysook::RGBPanelGrid<W,H>::put_text;

    using mysook::RGBPanelGrid<W,H>::set_fg;
    using mysook::RGBPanelGrid<W,H>::set_bg;
    using mysook::RGBPanelGrid<W,H>::set_brightness;

    virtual void set_cursor(int x, int y) {
        mysook::RGBPanel<W,H>::set_cursor(x, y);
        set_register(REG_CURSOR_X, x);
        set_register(REG_CURSOR_Y, y);
    }

    virtual void draw();
};

template <int W, int H>
VM<W,H>::VM(mysook::Logger &log, mysook::RGBPanel<W,H> &panel, const uint32_t *program, uint32_t start) 
: log(log), panel(panel), program(program), _start(start) { }

template <int W, int H>
void VM<W,H>::begin() {
    this->grid.setFont(&TomThumb);
    this->grid.setTextWrap(false);

    halted = false;

    call_index.clear();

    scan_subs();

    program_ptr = get_sub(_start);
    stack_ptr = 0;

    log.logf_ln("I [vm] <PP:%08X> Starting VM at sub %u", program_ptr, _start);

    fill_bitmask();

    registers[REG_TICK_MODE]        = MODE_NONE;

    registers[REG_URGENCY]          = 10;
    registers[REG_BRIGHTNESS]       = 30;

    _tick_speed = hertz(10);
    set_brightness(30);

    registers[REG_FOREGROUND_COLOR] = 0xFFFFFF; // white
    registers[REG_BACKGROUND_COLOR] = 0x000000; // black
    registers[REG_MASKGROUND_COLOR] = 0x000000; // black

    set_fg(mysook::Color(255, 255, 255));
    set_bg(mysook::Color(0, 0, 0));

    // Make it so OP_RETURN in main will halt
    push(max_ptr - 1);
    push(0);

    registers[REG_STACK_SEGMENT]    = stack_ptr;
    registers[REG_MARK]             = stack_ptr;

}

template <int W, int H>
void VM<W,H>::scan_subs() {
    int i = 0;
    while (program[i] != OP_HALT) {
        if (program[i] == OP_SUB) {
            log.logf_ln("I [vm] call_index %d -> <PP:%08X>", program[i + 1], i);
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

    max_ptr = i;
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

            //if (s == OP_PUSH || s == OP_SUB) {
            //    log.logf_ln("T [vm] <PP:%08X> EXEC %08X %08X", program_ptr, s, peek_step());
            //}
            //else {
            //    log.logf_ln("T [vm] <PP:%08X> EXEC %08X", program_ptr, s);
            //}

            try {
                auto op_code = op_codes.at(s);
                op_code(*this);
            }
            catch (std::out_of_range e) {
                // PANIC!
                log.logf_ln("E [vm] <PP:%08X> invalid opcode %d", program_ptr, s);
                halt();
            }
        }
        break;

    case MODE_BRIGHTNESS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_BRIGHTNESS);
        TRACE("T [vm] <PP:%08X> EXEC %08X", program_ptr, OP_BRIGHTNESS);
        break;

    case MODE_MASK_ROWS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_ROWS);
        TRACE("T [vm] <PP:%08X> EXEC %08X", program_ptr, OP_MASK_ROWS);
        break;

    case MODE_MASK_COLUMNS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_COLUMNS);
        TRACE("T [vm] <PP:%08X> EXEC %08X", program_ptr, OP_MASK_COLUMNS);
        break;

    case MODE_MASK_BITS:
        in_tick_mode = true;
        tick_mode_op = op_codes.at(OP_MASK_BITS);
        TRACE("T [vm] <PP:%08X> EXEC %08X", program_ptr, OP_MASK_BITS);
        break;

    default:
        // PANIC!
        log.logf_ln("E [vm] invalid tick mode %08x", mode);
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

    //String stack_str;
    //bool first = true;
    //for (int i = 0; i < stack_ptr; ++i) {
    //    if (!first) stack_str += ", ";
    //    stack_str += String(stack[i]);
    //    first = false;
    //}
    //TRACE("T [vm] <PP:%08X> STACK: %s", stack_str.c_str());
}

template <int W, int H>
void VM<W,H>::tick_exec() {
    if (halted) return;

    reset_tick();
    unsigned long tick_start = mysook::get_micros();
    while (!has_ticked() && !has_halted()) {
        step_exec();

        // a tick is only permitted to be MAX_TIME_SLICE long
        if (tick_start + MAX_TIME_SLICE < mysook::get_micros()) {
            log.logf_ln("W [vm] <PP:%08X> Skipping tick: the program took too long.", program_ptr);
            break;
        }
    }

    //log.logf_ln("I [vm] Tick!");
}

template <int W, int H>
uint32_t VM<W,H>::pop() {
    if (stack_ptr <= 0) {
        // PANIC!
        log.logf_ln("E [vm] stack underflow");
        halt();
        return 0;
    }
    else {
        registers[REG_MARK]--;
        TRACE("T [vm] <PP:%08X> POP %08X", program_ptr, stack[ stack_ptr - 1 ]);
        return stack[ --stack_ptr ];
    }
}

template <int W, int H>
void VM<W,H>::push(uint32_t v) {
    if (stack_ptr >= STACK_SIZE) {
        // PANIC!
        log.logf_ln("E [vm] stack overflow");
        halt();
    }
    else {
        registers[REG_MARK]++;
        TRACE("T [vm] <PP:%08X> PUSH %08X", program_ptr, v);
        stack[ stack_ptr++ ] = v;
    }
}

template <int W, int H>
mysook::Color VM<W,H>::get_register_color(int reg) {
    if (reg < 0 || reg >= HEAP_SIZE) {
        // PANIC!
        log.logf_ln("E [vm] get illegal color register %d", reg);
        halt();
        return mysook::Color(0, 0, 0);
    }

    mysook::Color color(registers[reg]);

    DEBUG("D [vm] 0x%06X -> (%d, %d, %d)", registers[reg], color.r, color.g, color.b);

    return color;
}

template <int W, int H>
void VM<W,H>::set_register_color(int reg, mysook::Color val) {
    if (reg < 0 || reg >= HEAP_SIZE) {
        // PANIC!
        log.logf_ln("E [vm] set illegal color register %d", reg);
        halt();
        return; 
    }

    DEBUG("D [vm] (%d, %d, %d) -> 0x%06X", val.r, val.g, val.b, val.truecolor());

    set_register(reg, val.truecolor());
}

template <int W, int H>
void VM<W,H>::apply_column_mask(uint32_t mask) {
    DEBUG("D [vm] <PP:%08X> COLUMN MASK %08X", program_ptr, mask);

    clear_bitmask();

    uint32_t bit_mask = 0x01u << (W - 1);
    for (int x = 0; x < W; ++x) {
        uint32_t sel_mask = bit_mask >> x;
        uint32_t col_mask = mask & sel_mask;

        DEBUG("D [vm] <PP:%08X> SET BIT %08X", program_ptr, col_mask);

        for (int mp = 0; mp < H; ++mp) _panel_bitmask[mp] |= col_mask;
    }
}

template <int W, int H>
void VM<W,H>::draw() { 
    uint32_t *buf = this->get_buffer();
    mysook::Color mask_color = get_register_color(REG_MASKGROUND_COLOR);

    int i = 0;
    uint32_t base_mask = 0x01 << (W - 1);
    for (int y = 0; y < H; y++) {
        uint32_t row_mask = _panel_bitmask[y];
        DEBUG("D [vm] row %d mask 0x%03X", y, row_mask);

        for (int x = 0; x < W; x++) {
            uint32_t bit_mask = base_mask >> x;
            DEBUG("D [vm] bit %d mask 0x%03X", x, bit_mask);

            if (bit_mask & row_mask) {
                mysook::Color c(buf[i]);
                DEBUG("D [vm] unmasking (%d, %d) <- (%d, %d, %d)", x, y, c.r, c.g, c.b);
                panel.put_pixel(x, y, c);
            }
            else {
                DEBUG("D [vm] masking (%d, %d)", x, y);
                panel.put_pixel(x, y, mask_color);
            }

            ++i;
        }
    }

    panel.set_brightness(this->brightness > MAX_BRIGHTNESS ? MAX_BRIGHTNESS : this->brightness);
    delay(1); // See https://github.com/adafruit/Adafruit_NeoPixel/issues/139
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

template <int W, int H> \
void op_halt(VM<W,H> &p) { \
    p.log.logf_ln("I [vm] halt instruction");
    p.halt();
}

OP_NULLARY_STMT(sub, p.step())
OP_NULLARY_STMT(return, p.return_sub())
OP_UNARY_STMT(goto, p.jump(p.get_sub(a)))
OP_UNARY_STMT(gosub, p.jump_sub(p.get_sub(a)))
OP_NULLARY_STMT(tick, p.next_tick())
OP_NULLARY_STMT(noop, return);

OP_BINARY_STMT(cmp, if (b) p.jump(p.get_sub(a)))
OP_BINARY_STMT(cmpsub, if (b) p.jump_sub(p.get_sub(a)))

OP_BINARY_EXPR(eq, b == a)
OP_BINARY_EXPR(lt, b <  a)
OP_BINARY_EXPR(le, b <= a)
OP_BINARY_EXPR(gt, b >  a)
OP_BINARY_EXPR(ge, b >= a)
OP_BINARY_EXPR(ne, b != a)

OP_BINARY_EXPR(add, b + a)
OP_BINARY_EXPR(min, b - a)
OP_BINARY_EXPR(mul, b * a)
OP_BINARY_EXPR(div, b / a)
OP_BINARY_EXPR(mod, b % a)

OP_BINARY_EXPR(and, b & a)
OP_BINARY_EXPR(or,  b | a)
OP_BINARY_EXPR(xor, b ^ a)
OP_UNARY_EXPR(not, !a)
OP_BINARY_EXPR(bsl, b << a)
OP_BINARY_EXPR(bsr, b >> a)
OP_UNARY_EXPR(comp, ~a)

OP_NULLARY_EXPR(rand, rand())
OP_NULLARY_EXPR(width, W)
OP_NULLARY_EXPR(height, H)

OP_BINARY_STMT(set, p.set_register(a, b))
OP_UNARY_EXPR(get, p.get_register(a))
OP_UNARY_STMT(tick_mode, p.set_register(REG_TICK_MODE, a))
OP_UNARY_STMT(urgency, p.set_register(REG_URGENCY, a))
OP_UNARY_STMT(brightness, p.set_register(REG_BRIGHTNESS, a))
OP_UNARY_STMT(foreground, p.set_register(REG_FOREGROUND_COLOR, a))
OP_UNARY_STMT(background, p.set_register(REG_BACKGROUND_COLOR, a))
OP_UNARY_STMT(maskground, p.set_register(REG_MASKGROUND_COLOR, a))
OP_NULLARY_STMT(mark, p.set_register(REG_MARK, 0));

OP_NULLARY_STMT(fill, p.fill_screen())

template <int W, int H>
void op_fill_rows(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << (H - 1);
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

    uint32_t bit_mask = 0x01u << (W - 1);
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

    uint32_t bit_mask = 0x01u << (W - 1);
    for (int y = 0; y < H; ++y) {
        uint32_t row_mask = mask[y];

        for (int x = 0; x < W; ++x) {
            if (row_mask & (bit_mask >> x)) p.put_pixel(x, y);
        }
    }
}

OP_BINARY_STMT(pixel, p.put_pixel(b, a));

template <int W, int H>
void op_mask_rows(VM<W,H> &p) {
    uint32_t mask = p.pop();

    uint32_t bit_mask = 0x01u << (H - 1);
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
    p.apply_column_mask(mask);
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

OP_UNARY_EXPR(read, p.read_stack(p.get_register(REG_STACK_SEGMENT) + a))
OP_BINARY_STMT(write, p.write_stack(p.get_register(REG_STACK_SEGMENT) + a, b))
OP_BINARY_STMT(alloc, p.allocate_stack(a, b))
OP_UNARY_EXPR(readarg, p.read_stack(p.get_register(REG_STACK_SEGMENT) - a - 2))

OP_UNARY_STMT(putchar, p.put_char(a));

template <int W, int H>
void op_set_cursor(VM<W,H> &p) {
    uint32_t a = p.pop();
    uint32_t b = p.pop();
    p.set_cursor(b, a);
}

#endif//__VM_H

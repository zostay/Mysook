#ifndef __VM_H
#define __VM_H

#include <map>

#include <Firmware.h>

#include "ops.h"

#define STACK_SIZE 256
#define HEAP_SIZE  256

#define PANEL_WIDTH 12
#define PANEL_HEIGHT 6

template <int W, int H, class P = RGBPanel<W, H> >
class VM<W,H,P> : public Ticking {
    protected:
        const uint32_t *program;
        uint32_t stack[STACK_SIZE];
        uint32_t registers[HEAP_SIZE];

        std::map<uint32_t, int> call_index;
        OpCodes<W,H,P> op_codes;

        P &panel;

        Color grid[W][H];
        uint32_t panel_bitmask[H];

        int program_ptr = 0;
        int stack_ptr = 0;

        int tick = 0;

        void scan_subs();

        bool halted = false;

    public:
        VM(P &panel, const uint32_t *program, uint32_t start) 

        const int width() const { return W; }
        const int height() const { return H; }

        virtual bool ready_for_tick(unsigned long now) { return true; }
        void tick() { tick_exec(); }

        void set_pixel(int x, int y) { 
            grid[x][y] = get_register_color(REG_FOREGROUND_COLOR);
        }

        void draw();

        uint16_t pop();
        void push(uint32_t v);
        
        void halt() { halted = true; }

        void get_sub(uint32_t sub) { return call_index[sub]; }

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

        void reset_tick() { tick = 0; }
        void next_tick() { tick++; draw(); }
        void has_ticked() { return tick; }

        uint32_t get_register(int reg) {
            return registers[reg];
        }

        void set_register(int reg, uint32_t val) {
            registers[reg] = val;
        }

        Color get_register_color(int reg);
        void set_register_color(int reg, Color val);
}

#endif//__VM_H

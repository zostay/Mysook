#!/usr/bin/env perl6
use v6;

enum VMTickMode (
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

enum VMOp (
    OP_HALT            => 0xDEAD,
    OP_SUB             => 0x0001,
    OP_RETURN          => 0x0002,
    OP_GOTO            => 0x0003,
    OP_GOSUB           => 0x0004,
    OP_TICK            => 0x0005,
    OP_TICK_MODE       => 0x0007,
    OP_NOOP            => 0x0008,

    OP_CMP             => 0x0011,
    OP_CMPSUB          => 0x0012,

    OP_EQ              => 0x0021,
    OP_LT              => 0x0022,
    OP_LE              => 0x0023,
    OP_GT              => 0x0024,
    OP_GE              => 0x0025,
    OP_NE              => 0x0026,

    OP_ADD             => 0x0031,
    OP_MIN             => 0x0032,
    OP_MUL             => 0x0033,
    OP_DIV             => 0x0034,
    OP_MOD             => 0x0035,

    OP_AND             => 0x0041,
    OP_OR              => 0x0042,
    OP_NOT             => 0x0043,

    OP_RAND            => 0x0051,
    OP_WIDTH           => 0x0052,
    OP_HEIGHT          => 0x0053,

    OP_SET             => 0x0081,
    OP_GET             => 0x0082,
    OP_URGENCY         => 0x0091,
    OP_BRIGHTNESS      => 0x0092,
    OP_FOREGROUND      => 0x0093,

    OP_FILL            => 0x00A1,
    OP_FILL_ROWS       => 0x20A1,
    OP_FILL_COLUMNS    => 0x40A1,
    OP_FILL_BITS       => 0x80A1,
    OP_PIXEL           => 0x00A2,

    OP_MASK_ROWS       => 0x20B1,
    OP_MASK_COLUMNS    => 0x40B1,
    OP_MASK_BITS       => 0x80B1,

    OP_PUSH            => 0x00C1,
    OP_POP             => 0x00C2,
    OP_DUP             => 0x00C3,
    OP_SWAP            => 0x00C5,
    OP_MARK            => 0x00C7,
);

enum VMRegister (
    REG_TICK_MODE          => 0,
    REG_URGENCY            => 1,
    REG_BRIGHTNESS         => 2,
    REG_FOREGROUND_COLOR   => 3,
    REG_MARK               => 5,
);

sub update-ops-h(IO::Path $ops-h) {
    with $ops-h.open(:w) {
        .say(q:to/END_PREAMBLE/);
        // AUTOGENERATED FILE!!! DO NOT EDIT!!! See ops.p6
        #ifndef __VM_OPS_H
        #define __VM_OPS_H

        #include <functional>
        #include <map>

        template <int W, int H, class P>
        class OpCodes<W,H,P> {
        private:
            typedef std::function<void(VM<W,H,P>)> op_code;
            std::map<uint32_t, op_code*> ops;

        public:
            OpCodes();

            op_code *operator[] (uint32_t op) {
                try {
                    return ops.at(op);
                }
                catch (out_of_bounds e) {
                    return 0;
                }
            }
        };
        END_PREAMBLE

        .say: make-cpp-enum('VMTickMode', VMTickMode);
        .say: make-cpp-enum('VMOp', VMOp);
        .say: make-cpp-enum('VMRegister', VMRegister);

        .say(q:to/END_POSTAMBLE/);
        #endif//__VM_OPS_H
        END_POSTAMBLE
    }
}

sub update-ops-cpp(IO::Path $ops-cpp) {
    with $ops-cpp.open(:w) {
        .say: q:to/END_PREAMBLE/;
        // AUTOGENERATED FILE!!! DO NOT EDIT!!! See ops.p6

        template <int W, int H, class P>
        OpCodes<W,H,P>::OpCodes() {
        END_PREAMBLE

        .say: make-cpp-ops(VMOp);

        .say: q:to/END_POSTAMBLE/;
        }
        END_POSTAMBLE
    }
}

sub make-cpp-enum($name, $enum) {
    my $s = "enum $name \{\n";

    my $width = [max] $enum.enums.keys».chars;
    $s ~= $enum.enums.sort(*.value <=> *.value).map({
        sprintf "%-{$width}s = 0x%04X,", .key, .value;
    }).join("\n").indent(4);

    $s ~= "\n};\n";
    $s;
}

sub make-cpp-ops($enum) {
    my $width = 5 + [max] $enum.enums.keys».chars;
    $enum.enums.sort(*.value <=> *.value).map({
        sprintf "%-{$width}s = %s;", "ops[{.key}]", .key.lc;
    }).join("\n").indent(4);
}

sub MAIN() {
    update-ops-cpp("src/ops.cpp".IO);
    update-ops-h("src/ops.h".IO);
}

#!/usr/bin/env perl6
use v6;

use NameTag;

sub update-ops-h(IO::Path $ops-h) {
    $ops-h.chmod(0o644);
    with $ops-h.open(:w) {
        .say(q:to/END_PREAMBLE/);
        // AUTOGENERATED FILE!!! DO NOT EDIT!!! See ops.p6
        #ifndef __VM_OPS_H
        #define __VM_OPS_H

        #include <functional>
        #include <map>
        #include <stdexcept>

        template <int W, int H>
        class VM;

        template <int W, int H>
        class OpCodes {
        public:
            typedef VM<W,H> vm_type;
            typedef std::function<void(vm_type&)> op_code;

        private:
            std::map<uint32_t, op_code> ops;

        public:
            OpCodes();

            op_code at(uint32_t op) {
                return ops.at(op);
            }

            op_code operator[] (uint32_t op) {
                try {
                    return ops.at(op);
                }
                catch (std::out_of_range e) {
                    return 0;
                }
            }
        };
        END_PREAMBLE

        .say: make-cpp-enum('VMTickMode', VMTickMode);
        .say: make-cpp-enum('VMOp', VMOp);
        .say: make-cpp-enum('VMRegister', VMRegister);
        .say: make-cpp-op-decls(VMOp);

        .say: q:to/END_MIDAMBLE/;

        template <int W, int H>
        OpCodes<W,H>::OpCodes() {
        END_MIDAMBLE

        .say: make-cpp-ops(VMOp);

        .say: q:to/END_POSTAMBLE/;
        }

        #endif//__VM_OPS_H
        END_POSTAMBLE
    }
    $ops-h.chmod(0o444);
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
        sprintf "%-{$width}s = %s\<W,H>;", "ops[{.key}]", .key.lc;
    }).join("\n").indent(4);
}

sub make-cpp-op-decls($enum) {
    $enum.enums.sort(*.value <=> *.value).map({
        "template <int W, int H> void {.key.lc}(VM<W,H> &);";
    }).join("\n");
}

sub MAIN() {
    update-ops-h("src/ops.h".IO);
}

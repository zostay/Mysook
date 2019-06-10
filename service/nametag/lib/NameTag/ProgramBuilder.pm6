use v6;

use NameTag;

class NameTag::Codex {
    has $.call-count = 1;
    has %.call-index;

    method jump-point() { $!call-count++; }

    method declare(Str $name) {
        die "function named $name is already defined"
            with %!call-index{ $name };
        return %!call-index{ $name } = $!call-count++;
    }

    method lookup(Str $name) {
        die "function named $name is not defined"
            without %!call-index{ $name };
        return %!call-index{ $name };
    }
}

class NameTag::Heap {
    has $.register-count = REG_USER0;
    has %.register-index;

    method allocate(Str $name) {
        die "global named $name is already defined"
            with %!register-index{ $name };
        return %!register-index{ $name } = $!register-count++;
    }

    method lookup(Str $name) {
        die "global named $name is not defined"
            without %!register-index{ $name };
        return %!register-index{ $name };
    }
}

class NameTag::Frame {
    has %.locals;
    has $.offset = 0;
    has Bool $.in-decls = True;

    method allocate(Str $name) {
        die "local named $name is already defined"
            with %!locals{ $name };

        die "all locals must be declared first"
            unless $!in-decls;

        return %!locals{ $name } = $!offset++;
    }

    method lookup(Str $name) {
        die "local named $name is not defined"
            without %!locals{ $name };

        return %!locals{ $name };
    }

    method end-decls() { $!in-decls = False; }
}

class NameTag::FrameStack {
    has NameTag::Frame @.frames = NameTag::Frame.new;

    method push() { @!frames.push: NameTag::Frame.new; return; }
    method pop() { @!frames.pop; return; }

    method push-arg() { @!frames[*-1].push-arg() }
    method allocate(Str $name) { @!frames[*-1].allocate($name) }
    method lookup(Str $name) { @!frames[*-1].lookup($name) }
    method end-decls() { @!frames[*-1].end-decls() }
}

class NameTag::Expression { }

class NameTag::ProgramBuilder {
    has @!program;
    has $!start = 1;

    has NameTag::Codex $.codex .= new;
    has NameTag::Heap $.heap .= new;
    has NameTag::FrameStack $.frame .= new;

    method ops(*@ops) { $!frame.end-decls; @!program.append: @ops; }
    method decls(*@ops) { @!program.append: @ops; }

    method start(Str $name) {
        $!start = $!codex.lookup($name);
    }

    method fun(Str $name, &block) {
        self.ops: OP_SUB, $!codex.declare($name);

        $!frame.push;
        block(self);
        $!frame.pop;

        self.ops: OP_RETURN;
    }

    method arg(NameTag::Expression $) { }

    method call(Str $name, *@args) {
        for @args {
            when NameTag::Expression { }
            default { self.ops: OP_PUSH, $_; }
        }
        self.ops: OP_PUSH, $!codex.lookup($name), OP_GOSUB;
        self.ops: @args.map({ OP_POP });
    }

    method pop() { self.ops: OP_POP; }
    method halt() { self.ops: OP_HALT; }
    method return() { self.ops: OP_RETURN; }

    method global(Str $name, Int $value = 0) {
        my $reg = $!heap.allocate($name);
        self.ops: OP_PUSH, $value, OP_PUSH, $reg, OP_SET;
    }

    method local(Str $name, Int $value = 0) {
        $!frame.allocate($name);
        self.decls: OP_PUSH, $value;
    }

    multi method local-array(Str $name, Int $size, Int $value = 0) {
        $!frame.allocate($name);
        self.decls: OP_PUSH, $value, OP_PUSH, $size, OP_ALLOC;
    }

    multi method local-array(Str $name, NameTag::Expression $size, NameTag::Expression $value) {
        $!frame.allocate($name);
        self.decls: OP_SWAP, OP_ALLOC;
    }

    method !lookup(Str $name) {
        with try { $!frame.lookup($name) } -> $offset {
            return ($offset, OP_READ, OP_WRITE);
        }
        orwith try { $!heap.lookup($name) } -> $reg {
            return ($reg, OP_GET, OP_SET);
        }
        else {
            die "no local or global variable named $name is defined";
        }
    }

    multi method set(Str $name, Int $value) {
        self.set($name, 0, $value);
    }

    multi method set(Str $name, Int $index, Int $value) {
        my ($base, $, $write) = self!lookup($name);
        self.ops: OP_PUSH, $value, OP_PUSH, $base + $index, $write;
    }

    multi method set(Str $name, NameTag::Expression $) {
        my ($base, $, $write) = self!lookup($name);
        self.ops: OP_PUSH, $base, $write;
    }

    multi method set(Str $name, NameTag::Expression $, NameTag::Expression $) {
        my ($base, $, $write) = self!lookup($name);
        self.ops: OP_SWAP, OP_PUSH, $base, OP_ADD, $write;
    }

    multi method get(Str $name, Int $index = 0) {
        my ($base, $read, $) = self!lookup($name);
        self.decls: OP_PUSH, $base + $index, $read;
        NameTag::Expression;
    }

    multi method get(Str $name, NameTag::Expression $) {
        my ($base, $read, $) = self!lookup($name);
        self.ops: OP_PUSH, $base, OP_ADD, $read;
        NameTag::Expression;
    }

    multi method get(VMRegister $reg) {
        self.ops: OP_PUSH, $reg, OP_GET;
        NameTag::Expression;
    }

    method read-arg(Int $num where $num < 0) {
        self.ops: OP_PUSH, -$num, OP_READARG;
        NameTag::Expression;
    }

    method rand() { self.decls: OP_RAND; NameTag::Expression }
    method width() { self.decls: OP_WIDTH; NameTag::Expression }
    method height() { self.decls: OP_HEIGHT; NameTag::Expression }
    method num(Int $value) { self.decls: OP_PUSH, $value; NameTag::Expression }

    method set-urgency(Int $value) {
        self.ops: OP_PUSH, $value, OP_URGENCY;
    }

    method set-brightness(Int $value) {
        self.ops: OP_PUSH, $value, OP_BRIGHTNESS;
    }

    multi method if(NameTag::Expression $, &block) {
        my $end-if = $!codex.jump-point;

        self.ops: OP_NOT, OP_PUSH, $end-if, OP_CMP;
        block(self);
        self.ops: OP_SUB, $end-if;
    }

    method loop(&block) {
        my $begin-loop = $!codex.jump-point;
        my $end-loop = $!codex.jump-point;

        self.ops: OP_SUB, $begin-loop;
        block(self, :$end-loop);
        self.ops: OP_PUSH, $begin-loop, OP_GOTO;
        self.ops: OP_SUB, $end-loop;
    }

    multi method for(NameTag::Expression $, NameTag::Expression $, Str $counter, &block) {
        my ($index, $read-op, $write-op) = self!lookup($counter);

        my $begin-loop = $!codex.jump-point;

        self.ops: OP_SWAP, OP_PUSH, $index, $write-op;
        self.ops: OP_SUB, $begin-loop;
        self.ops: OP_DUP;
        block(self, :$index, :$read-op, :$write-op);
        self.ops: OP_PUSH, $index, $read-op;
        self.ops: OP_PUSH, 1, OP_ADD;
        self.ops: OP_DUP, OP_PUSH, $index, $write-op;
        self.ops: OP_SWAP, OP_LE;
        self.ops: OP_PUSH, $begin-loop, OP_CMP;
        self.ops: OP_POP;
    }

    multi method for(Range $range, Str $counter, &block) {
        my ($index, $read-op, $write-op) = self!lookup($counter);

        my ($n, $x) = $range.int-bounds;

        my $begin-loop = $!codex.jump-point;

        self.ops: OP_PUSH, $n, OP_PUSH, $index, $write-op;
        self.ops: OP_SUB, $begin-loop;
        block(self, :$index, :$read-op, :$write-op);
        self.ops: OP_PUSH, $index, $read-op;
        self.ops: OP_PUSH, 1, OP_ADD;
        self.ops: OP_DUP;
        self.ops: OP_PUSH, $index, $write-op;
        self.ops: OP_PUSH, $x, OP_LE;
        self.ops: OP_PUSH, $begin-loop, OP_CMP;
    }

    multi method set-foreground(Int $color) {
        self.ops: OP_PUSH, $color, OP_FOREGROUND;
    }

    multi method set-foreground(NameTag::Expression $) {
        self.ops: OP_FOREGROUND;
    }

    method fill() {
        self.ops: OP_FILL;
    }

    multi method fill-columns(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_FILL_COLUMNS;
    }

    multi method fill-columns(NameTag::Expression $) {
        self.ops: OP_FILL_COLUMNS;
    }

    multi method fill-rows(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_FILL_ROWS;
    }

    multi method fill-rows(NameTag::Expression $) {
        self.ops: OP_FILL_ROWS;
    }

    multi method mask-columns(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_MASK_COLUMNS;
    }

    multi method mask-columns(NameTag::Expression $) {
        self.ops: OP_MASK_COLUMNS;
    }

    multi method mask-rows(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_MASK_ROWS;
    }

    multi method mask-rows(NameTag::Expression $) {
        self.ops: OP_MASK_ROWS;
    }

    multi method pixel(Int $x, Int $y) {
        self.ops: OP_PUSH, $x, OP_PUSH, $y, OP_PIXEL;
    }

    multi method pixel(NameTag::Expression $, NameTag::Expression $) {
        self.ops: OP_PIXEL;
    }

    multi method set-cursor(Int $x, Int $y) {
        self.ops: OP_PUSH, $x, OP_PUSH, $y, OP_SET_CURSOR;
    }

    multi method set-cursor(NameTag::Expression $, NameTag::Expression $) {
        self.ops: OP_SET_CURSOR;
    }

    method put-text(Str $text) {
        for $text.comb -> $ch {
            self.ops: OP_PUSH, $ch.ord, OP_PUTCHAR;
        }
    }

    method tick() {
        self.ops: OP_TICK;
    }

    multi method goto(Str $name) {
        self.ops: OP_PUSH, $!codex.lookup($name), OP_GOTO;
    }

    multi method goto(Int $jump-point) {
        self.ops: OP_PUSH, $jump-point, OP_GOTO;
    }

    multi method tick-mode(VMTickMode $mode, @data where { .all ~~ Int }) {
        self.ops: OP_MARK;
        self.ops: OP_PUSH, $_ for @data.reverse;
        self.ops: OP_PUSH, $mode, OP_TICK_MODE;
    }

    # multi method tick-mode(VMTickMode $mode, &block) {
    #     self.ops: OP_MARK;
    #     block(self);
    #     self.ops: OP_PUSH, $mode, OP_TICK_MODE;
    # }

    method dump() {
        my $pp = 0;
        my $str-width = [max] VMOp.enums.keys».chars;
        join "\n", gather while $pp < @!program.elems {
            my $op = @!program[ $pp ];

            if $op == OP_SUB | OP_PUSH {
                my $val = @!program[ $pp + 1 ];
                take sprintf "<PP:%08X> EXEC (%08X) %-{$str-width}s %08X", $pp, $op, $op, $val;
                $pp += 2;
            }
            else {
                take sprintf "<PP:%08X> EXEC (%08X) %-{$str-width}s", $pp, $op, $op;
                $pp++;
            }
        }
    }

    method buf8(--> buf8) {
        my @bytes;
        for flat $!start, @!program -> $dword {
            @bytes.append: (
                $dword +> 0x18,
                $dword +> 0x10,
                $dword +> 0x08,
                $dword,
            ) »%» 0x100;
        }

        buf8.new(@bytes);
    }

    method buf32(--> buf32) {
        buf32.new: flat $!start, @!program;
    }

    method program-ops() { |@!program, OP_HALT, OP_HALT }
}

sub program(&block) is export {
    my $*ntpb = my $builder = NameTag::ProgramBuilder.new;
    block($builder);
    $builder;
}

multi infix:<*> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_MUL; NameTag::Expression }
multi infix:<+> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_ADD; NameTag::Expression }
multi infix:<-> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_MIN; NameTag::Expression }
multi infix:</> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_DIV; NameTag::Expression }
multi infix:<%> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_MOD; NameTag::Expression }

multi infix:<==> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_EQ; NameTag::Expression }
multi infix:<!=> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_NE; NameTag::Expression }
multi infix:«<» (NameTag::Expression $a, NameTag::Expression $b)  is export { $*ntpb.decls: OP_LT; NameTag::Expression }
multi infix:«<=» (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_LE; NameTag::Expression }
multi infix:«>» (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_GT; NameTag::Expression }
multi infix:«>=» (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_GE; NameTag::Expression }

multi infix:<+&> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_AND; NameTag::Expression }
multi infix:<+|> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_OR; NameTag::Expression }
multi infix:<+^> (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_XOR; NameTag::Expression }
multi prefix:<!> (NameTag::Expression $a) is export { $*ntpb.decls: OP_NOT; NameTag::Expression }
multi infix:«+<» (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_BSL; NameTag::Expression }
multi infix:«+>» (NameTag::Expression $a, NameTag::Expression $b) is export { $*ntpb.decls: OP_BSR; NameTag::Expression }
multi prefix:<+^> (NameTag::Expression $a) is export { $*ntpb.decls: OP_COMP; NameTag::Expression }

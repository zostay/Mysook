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

class Codex {
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

class Heap {
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

class Frame {
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

class FrameStack {
    has Frame @.frames = Frame.new;

    method push() { @!frames.push: Frame.new; return; }
    method pop() { @!frames.pop; return; }

    method push-arg() { @!frames[*-1].push-arg() }
    method allocate(Str $name) { @!frames[*-1].allocate($name) }
    method lookup(Str $name) { @!frames[*-1].lookup($name) }
    method end-decls() { @!frames[*-1].end-decls() }
}

class Expression { }

class ProgramBuilder {
    has @!program;
    has $!start = 1;

    has Codex $.codex .= new;
    has Heap $.heap .= new;
    has FrameStack $.frame .= new;

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

    method arg(Expression $) { }

    method call(Str $name, *@args) {
        for @args {
            when Expression { }
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

    multi method local-array(Str $name, Expression $size, Expression $value) {
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

    multi method set(Str $name, Expression $) {
        my ($base, $, $write) = self!lookup($name);
        self.ops: OP_PUSH, $base, $write;
    }

    multi method set(Str $name, Expression $, Expression $) {
        my ($base, $, $write) = self!lookup($name);
        self.ops: OP_SWAP, OP_PUSH, $base, OP_ADD, $write;
    }

    multi method get(Str $name, Int $index = 0) {
        my ($base, $read, $) = self!lookup($name);
        self.decls: OP_PUSH, $base + $index, $read;
        Expression;
    }

    multi method get(Str $name, Expression $) {
        my ($base, $read, $) = self!lookup($name);
        self.ops: OP_PUSH, $base, OP_ADD, $read;
        Expression;
    }

    multi method get(VMRegister $reg) {
        self.ops: OP_PUSH, $reg, OP_GET;
        Expression;
    }

    method read-arg(Int $num where $num < 0) {
        self.ops: OP_PUSH, -$num, OP_READARG;
        Expression;
    }

    method rand() { self.decls: OP_RAND; Expression }
    method width() { self.decls: OP_WIDTH; Expression }
    method height() { self.decls: OP_HEIGHT; Expression }
    method num(Int $value) { self.decls: OP_PUSH, $value; Expression }

    method set-urgency(Int $value) {
        self.ops: OP_PUSH, $value, OP_URGENCY;
    }

    method set-brightness(Int $value) {
        self.ops: OP_PUSH, $value, OP_BRIGHTNESS;
    }

    multi method if(Expression $, &block) {
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

    multi method for(Expression $, Expression $, Str $counter, &block) {
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

    multi method set-foreground(Expression $) {
        self.ops: OP_FOREGROUND;
    }

    method fill() {
        self.ops: OP_FILL;
    }

    multi method fill-columns(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_FILL_COLUMNS;
    }

    multi method fill-columns(Expression $) {
        self.ops: OP_FILL_COLUMNS;
    }

    multi method fill-rows(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_FILL_ROWS;
    }

    multi method fill-rows(Expression $) {
        self.ops: OP_FILL_ROWS;
    }

    multi method mask-columns(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_MASK_COLUMNS;
    }

    multi method mask-columns(Expression $) {
        self.ops: OP_MASK_COLUMNS;
    }

    multi method mask-rows(Int $mask) {
        self.ops: OP_PUSH, $mask, OP_MASK_ROWS;
    }

    multi method mask-rows(Expression $) {
        self.ops: OP_MASK_ROWS;
    }

    multi method pixel(Int $x, Int $y) {
        self.ops: OP_PUSH, $x, OP_PUSH, $y, OP_PIXEL;
    }

    multi method pixel(Expression $, Expression $) {
        self.ops: OP_PIXEL;
    }

    multi method set-cursor(Int $x, Int $y) {
        self.ops: OP_PUSH, $x, OP_PUSH, $y, OP_SET_CURSOR;
    }

    multi method set-cursor(Expression $, Expression $) {
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

    method bytes() {
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

    method program-ops() { |@!program, OP_HALT, OP_HALT }
}

sub program(&block) is export {
    my $*ntpb = my $builder = NameTag::ProgramBuilder.new;
    block($builder);
    $builder;
}

multi infix:<*> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_MUL; Expression }
multi infix:<+> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_ADD; Expression }
multi infix:<-> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_MIN; Expression }
multi infix:</> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_DIV; Expression }
multi infix:<%> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_MOD; Expression }

multi infix:<==> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_EQ; Expression }
multi infix:<!=> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_NE; Expression }
multi infix:«<» (Expression $a, Expression $b)  is export { $*ntpb.decls: OP_LT; Expression }
multi infix:«<=» (Expression $a, Expression $b) is export { $*ntpb.decls: OP_LE; Expression }
multi infix:«>» (Expression $a, Expression $b) is export { $*ntpb.decls: OP_GT; Expression }
multi infix:«>=» (Expression $a, Expression $b) is export { $*ntpb.decls: OP_GE; Expression }

multi infix:<+&> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_AND; Expression }
multi infix:<+|> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_OR; Expression }
multi infix:<+^> (Expression $a, Expression $b) is export { $*ntpb.decls: OP_XOR; Expression }
multi prefix:<!> (Expression $a) is export { $*ntpb.decls: OP_NOT; Expression }
multi infix:«+<» (Expression $a, Expression $b) is export { $*ntpb.decls: OP_BSL; Expression }
multi infix:«+>» (Expression $a, Expression $b) is export { $*ntpb.decls: OP_BSR; Expression }
multi prefix:<+^> (Expression $a) is export { $*ntpb.decls: OP_COMP; Expression }

use v6;

unit module NameTag::ProgramValidator;

class GLOBAL::X::NameTag::ProgramValidator is Exception {
    has Int $.pp;
    has Str $.cause;
    only method new(Int $pp, Str $cause) { self.bless: :$pp, :$cause }
    method message() { ~self }
    method Str() { "PP$!pp.fmt('%04X'): $!cause" }
}

use NameTag;

sub validate-program(Blob $bin8-program) is export {
    my $d = buf32.new: $bin8-program.map(
        -> $a, $b, $c, $d {
            [+|] ($a +< 0x18, $b +< 0x10, $c +< 0x08, $d)
        }
    );

    my $start = $d[0];

    my UInt %codex{Int};

    my $pp = 1;
    while ($pp < $d.elems) {
        my $op = VMOp($d[$pp]);

        die X::NameTag::ProgramValidator.new($pp, "invalid opcode #$d[$pp].fmt('%08X')")
            unless $op;

        if $op == OP_SUB {
            die X::NameTag::ProgramValidator.new($pp, "reused function index $d[$op+1].fmt('%08X')")
                if %codex{ $d[$pp+1] } :exists;
            %codex{ $d[$op+1] } = $pp;
        }

        $pp++ if $op == OP_SUB | OP_PUSH;
        $pp++;
    }

    $d[*-1] != OP_SUB | OP_PUSH
        or die X::NameTag::ProgramValidator.new($pp, "program improperly terminates on OP_SUB or OP_PUSH opcode");
}

use v6;

unit module NameTag::ProgramValidator;

class GLOBAL::X::NameTag::ProgramValidator is Exception {
    has Str $.message;
    only method new(Str $message) { self.bless: :$message }
    method Str() { $!message }
}

use NameTag;

sub validate-program(Blob $d) is export {
    my $start = $d[0];

    my UInt %codex{Int};

    my $pp = 1;
    while ($pp < $d.elems) {
        my $op = VMOp($d[$pp]);

        die X::NameTag::ProgramValidator.new($pp, "invalid opcode [{$op//'-'}]")
            unless $op;

        if $op == OP_SUB {
            die X::NameTag::ProgramValidator.new($pp, "reused function index $d[$op+1]")
                if %codex{ $d[$op+1] } :exists;
            %codex{ $d[$op+1] } = $pp;
        }

        $pp++ if $op == OP_SUB | OP_PUSH;
        $pp++;
    }

    $d[*-1] != OP_SUB | OP_PUSH
        or die X::NameTag::ProgramValidator.new($pp, "program improperly terminates on OP_SUB or OP_PUSH opcode");
}

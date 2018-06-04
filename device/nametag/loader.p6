#!/usr/bin/env perl6
use v6;

use HTTP::UserAgent;
use HTTP::Request::Common;

sub get-program($program) {
    use NameTag;
    EVALFILE $program;
}

sub load-onto-nametag($program, $nametag) {
    my $ua = HTTP::UserAgent.new;

    #dd $content;

    say $program.dump();
    my $content = $program.bytes;

    my $req = POST("http://$nametag/", :$content);
    my $res = $ua.request($req);

    say $res.Str unless $res.is-success;
}

sub dump-cpp($program) {
    my @str = $program.program-ops.join(", ");
    while @str[*-1].chars > 74 {
        my $split = pop @str;

        my $i = $split.rindex(' ', 78);
        my $x = $split.substr(0..^$i);
        my $y = $split.substr($i^..*);
        @str.append: $x, $y;
    }

    say q:to/END_PREAMBLE/;
    #include <inttypes.h>
    #include "ops.h"
    END_PREAMBLE

    for $program.codex.call-index.kv -> $name, $index {
        say "#define $name.uc.subst(/\W/, "_", :g) $index";
    }

    say '';
    say 'uint32_t program[] = {';
    say @str.join("\n").indent(4);
    say '};';
}

sub MAIN(Str :program($program-file)!, Str :host($nametag), Bool :$cpp) {
    my $program = get-program($program-file);

    if ($nametag) {
        load-onto-nametag($program, $nametag);
    }
    elsif ($cpp) {
        dump-cpp($program);
    }
    else {
        USAGE();
    }
}

sub USAGE { say $*USAGE; }

#!/usr/bin/env perl6
use v6;

use NameTag;
use HTTP::UserAgent;
use HTTP::Request::Common;

constant PROGRAM_MAIN = 1;
constant LOOP_MAIN = 2;

my $program = program {
    .fun: "update-pixel", {
        .if: .read-arg(-3) == .num(0), {
            .set-foreground: 0x000000;
        }

        .if: .read-arg(-3) == .num(1), {
            .set-foreground: 0x0000FF;
        }

        .if: .read-arg(-3) == .num(2), {
            .set-foreground: 0xFFFF00;
        }

        .if: .read-arg(-3) == .num(3), {
            .set-foreground: 0xFF0000;
        }

        .pixel: .read-arg(-1), .read-arg(-2);
    }

    .fun: "piles", {
        .local: "x", 0;
        .local: "y", 0;
        .local: "xy", 0;
        .local-array: "grid", .width * .height, .num(0);

        .set-brightness: 10;
        .set-urgency: 1;
        .set-foreground: 0x000000;
        .fill;

        .loop: -> $_, *%_ {
            .set: "x", .rand % .width;
            .set: "y", .rand % .height;
            .set: "xy", .get("x") + .get("y") * .width;

            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

            .arg: .get("grid", .get("xy"));
            .arg: .get("y");
            .arg: .get("x");
            .call: "update-pixel";
            .pop; .pop; .pop;

            .set: "y", 0;
            .loop: -> $_, :end-loop($end-loop-y) {
                .if: .get("y") >= .height, {
                    .goto: $end-loop-y;
                }

                .set: "x", 0;

                .loop: -> $_, :end-loop($end-loop-x) {
                    .if: .get("x") >= .width, {
                        .goto: $end-loop-x;
                    }

                    .set: "xy", .get("x") + .get("y") * .width;

                    .if: .get("grid", .get("xy")) >= .num(4), {
                        .set: "grid", .get("xy"), .num(0);

                        .if: .get("x") > .num(0), {
                            .set: "xy", .get("x") - .num(1) + .get("y") * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .arg: .get("grid", .get("xy"));
                            .arg: .get("y");
                            .arg: .get("x") - .num(1);
                            .call: "update-pixel";
                            .pop; .pop; .pop;
                        }

                        .if: .get("y") > .num(0), {
                            .set: "xy", .get("x") + (.get("y") - .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .arg: .get("grid", .get("xy"));
                            .arg: .get("y") - .num(1);
                            .arg: .get("x");
                            .call: "update-pixel";
                            .pop; .pop; .pop;
                        }

                        .if: .get("x") < .width - .num(1), {
                            .set: "xy", .get("x") + .num(1) + .get("y") * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .arg: .get("grid", .get("xy"));
                            .arg: .get("y");
                            .arg: .get("x") + .num(1);
                            .call: "update-pixel";
                            .pop; .pop; .pop;
                        }

                        .if: .get("y") < .height - .num(1), {
                            .set: "xy", .get("x") + (.get("y") + .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .arg: .get("grid", .get("xy"));
                            .arg: .get("y") + .num(1);
                            .arg: .get("x");
                            .call: "update-pixel";
                            .pop; .pop; .pop;
                        }
                    }

                    .set: "x", .get("x") + .num(1);
                }

                .set: "y", .get("y") + .num(1);
            }

            .tick;
        }
    }


    .start: "piles";
}

sub MAIN(:host($nametag), :$cpp) {
    my $ua = HTTP::UserAgent.new;

    #dd $content;

    if ($nametag) {
        say $program.dump();
        my $content = $program.bytes;

        my $req = POST("http://$nametag/", :$content);
        my $res = $ua.request($req);

        say $res.Str unless $res.is-success;
    }
    elsif ($cpp) {
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
    else {
        USAGE();
    }
}

sub USAGE { say $*USAGE; }

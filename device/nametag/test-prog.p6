#!/usr/bin/env perl6
use v6;

use NameTag;
use HTTP::UserAgent;
use HTTP::Request::Common;

constant PROGRAM_MAIN = 1;
constant LOOP_MAIN = 2;

my $program = program {
    .fun: "MAIN", {
        .local: "i";
        .set-urgency: 5;
        .set-brightness: 30;
        .for: ^10, :local, "i", -> $_, *%_ {
            .set-foreground: 0xFF9900;
            .fill;
            .tick;

            .set-foreground: 0x9900FF;
            .fill;
            .tick;
        }
    }

    .fun: "piles", {
        .local: "x", 0;
        .local: "y", 0;
        .local: "xy", 0;
        .local-array: "grid", .width * .height, .num(0);

        .loop: -> $_, *%_ {
            .set: "x", .rand % .width;
            .set: "y", .rand % .height;
            .set: "xy", .get("x") + .get("y") * .width;

            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

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

                    .if: .get("grid", .get("xy")) == .num(0), {
                        .set-foreground: 0x000000;
                    }

                    .if: .get("grid", .get("xy")) == .num(1), {
                        .set-foreground: 0x0000FF;
                    }

                    .if: .get("grid", .get("xy")) == .num(2), {
                        .set-foreground: 0xFFFF00;
                    }

                    .if: .get("grid", .get("xy")) == .num(3), {
                        .set-foreground: 0xFF0000;
                    }

                    .pixel: .get("x"), .get("y");

                    .if: .get("grid", .get("xy")) >= .num(4), {
                        .set: "grid", .get("xy"), .num(0);

                        .if: .get("x") > .num(0), {
                            .set: "xy", .get("x") - .num(1) + .get("y") * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);
                        }

                        .if: .get("y") > .num(0), {
                            .set: "xy", .get("x") + (.get("y") - .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);
                        }

                        .if: .get("x") < .width - .num(1), {
                            .set: "xy", .get("x") + .num(1) + .get("y") * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);
                        }

                        .if: .get("y") < .height - .num(1), {
                            .set: "xy", .get("x") + (.get("y") + .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);
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

# my @program =
#     PROGRAM_MAIN,
#     OP_SUB, PROGRAM_MAIN,
#     OP_PUSH, 10, OP_PUSH, REG_USER0, OP_SET,
#     OP_PUSH, 5, OP_URGENCY,
#     OP_PUSH, 30, OP_BRIGHTNESS,
#     OP_SUB, LOOP_MAIN,
#     OP_PUSH, 0xFF9900, OP_FOREGROUND, OP_FILL, OP_TICK,
#     OP_PUSH, 0x9900FF, OP_FOREGROUND, OP_FILL, OP_TICK,
#     OP_PUSH, REG_USER0, OP_GET,
#     OP_PUSH, 1, OP_MIN,
#     OP_PUSH, REG_USER0, OP_SET,
#     OP_PUSH, REG_USER0, OP_GET,
#     OP_PUSH, 0, OP_GT,
#     OP_PUSH, LOOP_MAIN, OP_CMP,
#     OP_HALT,
# ;

sub MAIN(:host($nametag)!) {
    my $ua = HTTP::UserAgent.new;

    #dd $content;

    say $program.dump();
    my $content = $program.bytes;

    my $req = POST("http://$nametag/", :$content);
    my $res = $ua.request($req);

    say $res.Str unless $res.is-success;
}

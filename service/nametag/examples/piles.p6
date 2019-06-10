use v6;

program {
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
        .tick;

        .loop: -> $_, *% {
            .set: "x", .rand % .width;
            .set: "y", .rand % .height;
            .set: "xy", .get("x") + .get("y") * .width;

            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

            .call: "update-pixel",
                .get("grid", .get("xy")),
                .get("y"),
                .get("x");

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

                            .call: "update-pixel",
                                .get("grid", .get("xy")),
                                .get("y"),
                                .get("x") - .num(1);
                        }

                        .if: .get("y") > .num(0), {
                            .set: "xy", .get("x") + (.get("y") - .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .call: "update-pixel",
                                .get("grid", .get("xy")),
                                .get("y") - .num(1),
                                .get("x");
                        }

                        .if: .get("x") < .width - .num(1), {
                            .set: "xy", .get("x") + .num(1) + .get("y") * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .call: "update-pixel",
                                .get("grid", .get("xy")),
                                .get("y"),
                                .get("x") + .num(1);
                        }

                        .if: .get("y") < .height - .num(1), {
                            .set: "xy", .get("x") + (.get("y") + .num(1)) * .width;
                            .set: "grid", .get("xy"), .get("grid", .get("xy")) + .num(1);

                            .call: "update-pixel",
                                .get("grid", .get("xy")),
                                .get("y") + .num(1),
                                .get("x");
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

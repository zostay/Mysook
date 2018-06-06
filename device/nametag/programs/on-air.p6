use v6;

program {
    .fun: "throb-red", {
        .set-urgency: 1;
        .set-foreground: 0xFF0000;
        .fill;

        .loop: -> $_, *% {
            .tick-mode: MODE_BRIGHTNESS, (
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
            );
        };
    }

    .fun: "make-masks", {
        .local: "half-width", 0;
        .local: "half-height", 0;
        .local: "mask", 0;

        .global: "right-mask", 0;
        .global: "left-mask", 0;
        .global: "top-mask", 0;
        .global: "bottom-mask", 0;

        .set: "half-width", .width / .num(2);
        .set: "half-height", .height / .num(2);

        .set: "mask", (.num(1) +< .get("half-width")) - .num(1);
        .set: "right-mask", .get("mask");

        .set: "mask", .get("mask") +< .get("half-width");
        .set: "left-mask", .get("mask");

        .set: "mask", (.num(1) +< .get("half-height")) - .num(1);
        .set: "top-mask", .get("mask");

        .set: "mask", .get("mask") +< .get("half-height");
        .set: "bottom-mask", .get("mask");
    }

    .fun: "police-vert", {
        .set-urgency: 1;
        .set-brightness: 30;

        .set-foreground: 0xFF0000;
        .fill-columns: .get("left-mask");

        .set-foreground: 0x0000FF;
        .fill-columns: .get("right-mask");
    }

    .fun: "vertical-triple-flash", {
        .local: "count";

        .for: ^3, "count", -> $_, *% {
            .mask-columns: .get("right-mask");
            .tick;

            .mask-columns: 0;
            .tick;
        }

        .for: ^3, "count", -> $_, *% {
            .mask-columns: .get("left-mask");
            .tick;

            .mask-columns: 0;
            .tick;
        }
    }

    .fun: "police-horz", {
        .set-urgency: 1;
        .set-brightness: 30;

        .set-foreground: 0xFF0000;
        .fill-rows: .get("top-mask");

        .set-foreground: 0x0000FF;
        .fill-rows: .get("bottom-mask");
    }

    .fun: "horizontal-triple-flash", {
        .local: "count";

        .for: ^3, "count", -> $_, *% {
            .mask-rows: .get("top-mask");
            .tick;

            .mask-rows: 0;
            .tick;
        }

        .for: ^3, "count", -> $_, *% {
            .mask-rows: .get("bottom-mask");
            .tick;

            .mask-rows: 0;
            .tick;
        }
    }

    .fun: "police", {
        .local: "counter";

        .call: "make-masks";
        .loop: -> $_, *% {
            .for: ^10, "counter", -> $_, *% {
                .call: "police-vert";
                .call: "vertical-triple-flash";
            }

            .for: ^10, "counter", -> $_, *% {
                .call: "police-horz";
                .call: "horizontal-triple-flash";
            }

            .for: ^10, "counter", -> $_, *% {
                .call: "police-vert";
                .call: "horizontal-triple-flash";
            }

            .for: ^10, "counter", -> $_, *% {
                .call: "police-horz";
                .call: "vertical-triple-flash";
            }
        }
    }

    .fun: "on-air-text", {
        .local: "marquee-pos", 0;

        .set-urgency: 2;
        .set-brightness: 30;

        .loop: -> $_, *% {
            .set: "marquee-pos", .width;
            .loop: -> $_, :$end-loop {
                .set-foreground: 0x000000;
                .fill;

                .set-foreground: 0xFF000;

                .set-cursor: .get("marquee-pos"), .num(5);
                .put-text: "ONAIR";

                .set: "marquee-pos", .get("marquee-pos") - .num(1);

                .tick;

                # these are all uint32_ts inside the VM, so fake in
                # 2s-compliment
                .if: .get(REG_CURSOR_X) > .num(0x80000000), {
                    .goto: $end-loop;
                }
            }

            .tick;
            .tick;
            .tick;
            .tick;
        }

        .loop:  -> $_, *% {
            .tick;
        }
    }

    .start: "on-air-text";
}

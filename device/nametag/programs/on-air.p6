use v6;

program {
    .fun: "throb-red", {
        .local: "counter";

        .set-urgency: 1;
        .set-foreground: 0xFF0000;
        .fill;

        .for: ^3, "counter", -> $_, *% {
            .tick-mode: MODE_BRIGHTNESS, (
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
            );
        };
    }

    .fun: "on-air-text", {
        .local: "counter";
        .local: "marquee-pos", 0;

        .set-urgency: 2;
        .set-brightness: 30;

        .for: ^3, "counter", -> $_, *% {
            .set: "marquee-pos", .width;
            .loop: -> $_, :$end-loop {
                .set-foreground: 0xFF0000;
                .fill;

                .set-foreground: 0x000000;

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
    }

    .fun: "on-air", {
        .loop: -> $_, *% {
            .call: "throb-red";
            .call: "on-air-text";
        }
    }

    .start: "on-air";
}

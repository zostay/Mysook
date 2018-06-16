use v6;

program {

    .fun: "flash", {
        .set-urgency: 1;

        .set-foreground: 0xAAFF77;
        .fill;
        .tick-mode: MODE_BRIGHTNESS, (
            0x30, 0x35, 0x40, 0x45, 0x50, 0x00, 0x00,
            0x30, 0x35, 0x40, 0x45, 0x50, 0x00, 0x00,
            0x30, 0x35, 0x40, 0x45, 0x50, 0x00, 0x00,
        );

        .set-urgency: 2;
        .set-brightness: 75;

    }

    .fun: "nametag", {
        .local: "foreground", 0xFFFFFF;
        .local: "counter";
        .local: "marquee-pos", 0;

        .set-urgency: 2;
        .set-brightness: 75;

        .loop: -> $_, *% {
            .set: "foreground", .rand +| .num(0xC0C0C0);
            .set: "marquee-pos", .width;
            .loop: -> $_, :$end-loop {

                .set-foreground: 0;
                .fill;

                .set-foreground: .get("foreground");

                .set-cursor: .get("marquee-pos"), .num(5);
                .put-text: "Sterling";

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

            .call: "flash";

            .set: "foreground", 0xCCFF99;
            .set-brightness: 75;

            .set: "marquee-pos", .width;
            .loop: -> $_, :$end-loop {

                .set-foreground: 0;
                .fill;

                .set-foreground: .get("foreground");

                .set-cursor: .get("marquee-pos"), .num(5);
                .put-text: "GREEN";

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

            .call: "flash";
        }
    }

    .start: "nametag";
}


use v6;

program {

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

            .set: "foreground", 0xCCFF99;

            .set: "marquee-pos", .width;
            .loop: -> $_, :$end-loop {

                .set-foreground: 0;
                .fill;

                .set-foreground: .get("foreground");

                .set-cursor: .get("marquee-pos"), .num(5);
                .put-text: "ZipRecruiter";

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
        }
    }

    .start: "nametag";
}


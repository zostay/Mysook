use v6;

program {
    .fun: "sterlings-awful-soldering", {
        .local: "foreground", 0xFFFFFF;
        .local: "marquee-pos", 0;

        .set-urgency: 2;
        .set-brightness: 75;

        .set: "marquee-pos", .width;

        .loop: -> $_, :$end-loop {
            .set-foreground: 0;
            .fill;

            .set-foreground: .get("foreground");

            .set-cursor: .get("marquee-pos"), .num(6);
            .put-text:  "also Sterling's ";

            .set-cursor: .get(REG_CURSOR_X), .num(5) + .rand % .num(3);
            .set-foreground: 0xFF0000;
            .put-text: "A";

            .set-cursor: .get(REG_CURSOR_X), .num(5) + .rand % .num(3);
            .set-foreground: 0xFF0000;
            .put-text: "W";

            .set-cursor: .get(REG_CURSOR_X), .num(5) + .rand % .num(3);
            .set-foreground: 0xFF0000;
            .put-text: "F";

            .set-cursor: .get(REG_CURSOR_X), .num(5) + .rand % .num(3);
            .set-foreground: 0xFF0000;
            .put-text: "U ";

            .set-cursor: .get(REG_CURSOR_X), .num(5) + .rand % .num(3);
            .set-foreground: 0xFF0000;
            .put-text: "L";

            .set-cursor: .get(REG_CURSOR_X), .num(6);
            .set-foreground: .get("foreground");
            .put-text: " soldering";

            .set: "marquee-pos", .get("marquee-pos") - .num(1);

            .tick;

            .if: .get(REG_CURSOR_X) > .num(0x80000000), {
                .set: "marquee-pos", .width;
            }
        }
    }

    .start: "sterlings-awful-soldering";
}

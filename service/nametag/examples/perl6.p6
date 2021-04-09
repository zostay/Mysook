use v6;

program {
    .fun: "async-web-programming", {
        .local: "foreground", 0xFFFFFF;
        .local: "marquee-pos", 0;

        .set-urgency: 2;
        .set-brightness: 75;

        .set: "marquee-pos", .width;

        .loop: -> $_, :$end-loop {
            .set-foreground: 0;
            .fill;

            .set-cursor: .get("marquee-pos"), .num(6);

            .set-foreground: 0xFF0000;
            .put-text: "P";

            .set-foreground: 0xFFFF00;
            .put-text: "E";

            .set-foreground: 0x00FF00;
            .put-text: "R";

            .set-foreground: 0x00FFFF;
            .put-text: "L ";

            .set-foreground: 0x0000FF;
            .put-text: "6";

            .set: "marquee-pos", .get("marquee-pos") - .num(1);

            .tick;

            .if: .get(REG_CURSOR_X) > .num(0x80000000), {
                .set: "marquee-pos", .width;
            }
        }
    }

    .start: "async-web-programming";
}

use v6;

program {
    .fun: "rotate-fill", {
        .set-urgency: 10;
        .set-brightness: 30;

        .loop: -> $_, *% {
            .set-foreground: 0x000000;
            .fill;
            .tick;

            .set-foreground: 0xFF0000;
            .fill;
            .tick;

            .set-foreground: 0x00FF00;
            .fill;
            .tick;

            .set-foreground: 0x0000FF;
            .fill;
            .tick;

            .set-foreground: 0xFFFF00;
            .fill;
            .tick;

            .set-foreground: 0x00FFFF;
            .fill;
            .tick;

            .set-foreground: 0xFF00FF;
            .fill;
            .tick;

            .set-foreground: 0xFFFFFF;
            .fill;
            .tick;
        }
    }

    .start: "rotate-fill";
}

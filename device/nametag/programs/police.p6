use v6;

program {
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
        .set-brightness: 75;

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
        .set-brightness: 75;

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

    .start: "police";
}

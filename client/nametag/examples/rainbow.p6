use v6;

program {
    .fun: "setup", {
        .global: "rgb";
        .global: "tick", 0;
    }

    .fun: "hsv2rgb", {
        .local: "hsv";
        .local: "h";
        .local: "s";
        .local: "v";
        .local: "r";
        .local: "g";
        .local: "b";
        .local: "i";
        .local: "q";
        .local: "t";
        .local: "p";

        .set: "hsv", .read-arg(-1);
        .set: "h", .get("hsv") +> .num(0x10);
        .set: "s", (.get("hsv") +> .num(0x08)) +& .num(0xFF);
        .set: "v", .get("hsv") +& .num(0xFF);

        .if: .get("s") == .num(0), {
            .set: "r", .get("v");
            .set: "g", .get("v");
            .set: "b", .get("v");
        }

        .if: .get("s") != .num(0), {
            .set: "i", .get("h") / .num(15_360); # 60*512
            .set: "p", (.num(256) * .get("v") - .get("v") * .get("s")) / .num(256);

            .if: .get("i") +& .num(1), {
                .set: "q", (.num(256) * .num(15_360) * .get("v") - .get("h") * .get("s") * .get("v") + .num(15_360) * .get("s") * .get("v") * .get("i")) / (.num(256) * .num(15_360));

                .if: .get("i") == .num(1), {
                    .set: "r", .get("q");
                    .set: "g", .get("v");
                    .set: "b", .get("p");
                }

                .if: .get("i") == .num(3), {
                    .set: "r", .get("p");
                    .set: "g", .get("q");
                    .set: "b", .get("v");
                }

                .if: .get("i") == .num(5), {
                    .set: "r", .get("v");
                    .set: "g", .get("p");
                    .set: "b", .get("q");
                }
            }

            .if: ! (.get("i") +& .num(1)), {
                .set: "t", (.num(256) * .num(15_360) * .get("v") + .get("h") * .get("s") * .get("v") - .num(15_360) * .get("s") * .get("v") * (.get("i") + .num(1))) / (.num(256) * .num(15_360));

                .if: .get("i") == .num(0), {
                    .set: "r", .get("v");
                    .set: "g", .get("t");
                    .set: "b", .get("p");
                }

                .if: .get("i") == .num(2), {
                    .set: "r", .get("p");
                    .set: "g", .get("v");
                    .set: "b", .get("t");
                }

                .if: .get("i") == .num(4), {
                    .set: "r", .get("t");
                    .set: "g", .get("p");
                    .set: "b", .get("v");
                }
            }
        }

        .set: "rgb", (.get("r") +< .num(0x10)) +| (.get("g") +< .num(0x08)) +| (.get("b"));
    }

    .fun: "rainbow", {
        .local: "h";
        .local: "counter";

        .set-urgency: 1;
        .set-brightness: 50;

        .loop: -> *@, *% {
            .set-foreground: 0;
            .fill;

            .for: .num(0), .width - .num(1), "counter", -> *@, *% {
                .set: "h", .get("counter") * (.num(65536) / .width);
                .call: "hsv2rgb", .get("h") +< .num(0x10) +| .num(0xFFFF);

                .set-foreground: .get("rgb");
                .fill-columns: .num(0x01) +< ((.get("counter") + .get("tick")) % .width);
            }

            .tick;

            .set: "tick", .get("tick") + .num(1);
        }
    }

    .start: "rainbow";
}

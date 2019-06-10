use v6;

program {
    .global: "rgb";
    .global: "tick", 0;
    .global: "marquee-pos", 0;
    .global: "marquee-pos-init", 0;

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

        .set-foreground: 0;
        .fill;

        .for: .num(0), .width - .num(1), "counter", -> *@, *% {
            .set: "h", .get("counter") * (.num(65536) / .width);
            .call: "hsv2rgb", .get("h") +< .num(0x10) +| .num(0x9999);

            .set-foreground: .get("rgb");
            .fill-columns: .num(0x01) +< (.width - (.get("counter") + .get("tick")) % .width - .num(1));
        }

    }

    .fun: "name", {
        .set-foreground: .rand +| .num(0xC0C0C0);

        .set-cursor: .get("marquee-pos"), .height - .num(1);
        .put-text: "Sterling   ";

        .set-foreground: 0xCCFF99;
        .put-text: "ZipRecruiter";

        .set: "marquee-pos", .get("marquee-pos") - .num(1);

        # these are all uint32_ts inside the VM, so fake in
        # 2s-compliment
        .if: .get(REG_CURSOR_X) > .num(0x80000000), {
            .set: "marquee-pos", .get("marquee-pos-init");
        }
    }

    .fun: "nametag", {
        .set-urgency: 1;
        .set-brightness: 50;

        .set: "marquee-pos-init", .width + .num(4);
        .set: "marquee-pos", .get("marquee-pos-init");

        .loop: -> *@, *% {
            .call: "rainbow";
            .call: "name";

            .tick;
            .set: "tick", .get("tick") + .num(1);
        }
    }

    .start: "nametag";
}

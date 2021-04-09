use v6;

program {
    .fun: "fire", {
        .local-array: "col-height", .width, .num(0);
        .local: "num-drivers", 0;
        .local-array: "drivers", .width / .num(4), .num(0);
        .local: "this-col-height", 0;
        .local: "found-driver", 0;
        .local: "left-height", 0;
        .local: "right-height", 0;
        .local: "driver-index", 0;
        .local: "col-index", 0;
        .local: "row", 0;

        .set: "num-drivers", .width / .num(4);

        .set-urgency: 5;

        .for: .num(0), .get("num-drivers"), "driver-index", -> *@, *% {
            .set: "drivers", .get("driver-index"), .rand % .width;
        }

        .for: .num(0), .width, "col-index", -> *@, *% {
            .set: "found-driver", .num(0);

            .for: .num(0), .get("num-drivers"), "driver-index", -> *@, *% {
                .if: .get("drivers", .get("driver-index")) == .get("col-index"), -> *@ {
                    .set: "found-driver", .num(1);
                }
            }

            .if: .get("found-driver") == .num(1), -> *@ {

                .set: "this-col-height", .get("col-index"), .get("col-height", .get("col-index")) + .num(2);

                .if: .get("this-col-height") > .height, -> *@ {
                    .set: "this-col-height", .height;
                    .set: "drivers", .get("col-index"), .rand % .width;
                }

                .set: "col-height", .get("driver-index"), .get("this-col-height");
            }

            .if: .get("found-driver") == .num(0), -> *@ {
                .set: "left-height", .get("col-height", (.get("col-index") + .width - .num(1)) % .width);
                .set: "right-height", .get("col-height", (.get("col-index") + .num(1)) % .width);

                .set: "col-height", .get("col-index"), (.get("left-height") + .get("right-height")) / .num(2);
            }
        }

        .set-foreground: 0x00000000;
        .fill;

        .set-foreground: 0x00FF0000;

        .for: .num(0), .width, "col-index", -> *@, *% {
            .for: .num(0), .get("col-height", .get("col-index")), "row", -> *@, *% {
                .pixel: .get("col-index"), .get("row");
            }
        }

        .tick;
    }

    .start: "fire";
}

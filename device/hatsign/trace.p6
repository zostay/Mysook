#!/usr/bin/env perl6
use v6;

use NameTag;

$*ARGFILES.lines.map({
    .subst(/<?after EXEC " "> <[ 0..9 a..f A..F ]>+/,  {
        try { VMOp(.Str.parse-base(16)) } // $_
    }).say;
});

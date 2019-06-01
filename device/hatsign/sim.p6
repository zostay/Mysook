#!/usr/bin/env perl6
use v6;

use Terminal::Print <T>;
use NameTag;

sub MAIN($program) {
    T.initialize-screen;



    T.shutdown-screen;
}

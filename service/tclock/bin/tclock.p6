#!/usr/bin/env perl6
use v6;

use TClock::Main;

sub MAIN(
    Str :$host = %*ENV<DEPLOYMENT_HOST> // 'localhost',
    Int :$port = %*ENV<DEPLOYMENT_PORT> // 5000,
    Str :$stage = %*ENV<DEPLOYMENT_STAGE> // 'production',
    Str :$config-dir = %*ENV<TCLOCK_CONFIG>,
) {
    my $main = TClock::Main.instance(
        |(do with $host  { :$host }),
        |(do with $port  { :$port }),
        |(do with $stage { :$stage }),
        config-dir => $config-dir.IO,
    );

    my $server = $main.web-server;
    $server.run;
}

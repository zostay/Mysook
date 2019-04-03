#!/usr/bin/env perl6
use v6;

use TClock::Main;

sub MAIN(
    Str :$host = %*ENV<DEPLOYMENT_HOST> // 'localhost',
    Int :$port = %*ENV<DEPLOYMENT_PORT> // 5000,
    Str :$stage = %*ENV<DEPLOYMENT_STAGE> // 'production',
    Str :$ddb-hostname,
    Int :$ddb-port,
) {
    my $main = TClock::Main.instance(
        |(do with $host  { :$host }),
        |(do with $port  { :$port }),
        |(do with $stage { :$stage }),
        :$ddb-hostname,
        :$ddb-port,
    );
    my $application = $main.app;

    note "Using local DynamoDB http://$_:$main.ddb-port()/ ..."
        with $main.ddb-hostname;

    my $server = $main.web-server;
    $server.run;
}

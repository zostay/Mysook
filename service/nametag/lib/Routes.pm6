use v6;

use Base64;
use Cro::HTTP::Middleware;
use Cro::HTTP::Router;
use Crypt::RSA;

use DB;
use NameTag::ProgramValidator;

class NameTagID does Cro::HTTP::Auth { }

class NameTagAuth does Cro::HTTP::Middleware::Request {
    has $.db;
    has $.rsa;
    has $.credentials;

    submethod TWEAK {
        die "missing DEVICE_KEY" without %*ENV<DEVICE_KEY>;
        die "device key must be formatted as EXPONENT:MODULUS"
            unless %*ENV<DEVICE_KEY>.Str ~~ /^\d+ ':' \d+$/;

        my $public-key = Crypt::RSA::Key.new(
            exponent => %*ENV<DEVICE_KEY>.Str.split(":", 2)[0].Int,
            modulus  => %*ENV<DEVICE_KEY>.Str.split(":", 2)[1].Int,
        );

        $!rsa = Crypt::RSA.new(:$public-key);
    }

    method process(Supply $requests --> Supply) {
        supply whenever $requests -> $req {
            my $auth = Nil;
            with $req.header('Authorization') -> $auth {
                my ($type, $credentials) = $auth.split(' ', 2);

                if ($type.lc eq 'signature') {
                    my ($message, $signature) = $credentials.split(':', 2);
                    my ($plain) = $!rsa.encrypt($signature);
                    my ($nonce, $sig-message) = split(':', 2);
                    if $!db.check-nonce && $message eq $sig-message {
                        $auth = NameTagID.new;
                    }
                }
            }

            $req.auth = $auth;
            emit $req;
        }
    }
}

class RateLimiter does Cro::HTTP::Middleware::Request {
    has $.db;

    method process(Supply $requests --> Supply) {
        supply whenever $requests -> $req {
            my $length = $!db.queue-length;

            # The longer the queue, the longer the wait time, up to 60 seconds
            my $wait-time = 60 min e ** $length / 7;
            sleep $wait-time if $wait-time > 1;

            emit $req;
        }
    }
}

class X::ValidationFail is Exception {
    has $.field;
    has $.message;
    method Str() { "$!field: $!message" }
}

sub routes() is export {
    my $db = DB.new;
    $db.create-schema;

    my $rate-limited-routes = route {
        before RateLimiter.new(:$db);

        # Upload a program
        post -> 'program' {
            request-body -> %program {
                %program<name> ~~ /\w+/
                    or die X::ValidationFail("name", "You must give your program a name.");
                my $name = %program<name>.trim;

                %program<author> ~~ /^[github|cpan|http|https]:\S+/
                    or die X::ValidationFail("author", "You must give a URL claiming authorship, e.g., github:zostay, cpan:HANENKAMP, or https://sterling.hanenkamp.com/");
                my $author = %program<author>.trim;

                %program<program> ~~ m{^<[a..zA..Z+/]> ** 22..87382 $}
                    or die X::ValidationFail("program", "You must provide the program as a BASE-64 encoded string.");
                my $b64-program = %program<program>;
                my $bin-program = decode-base64($b64-program, :bin);

                {
                    validate-program($bin-program);

                    CATCH {
                        when (X::NameTag::ProgramValidator) {
                            die X::ValidationFail.new("program", "Your program is not valid: $_");
                        }
                    }
                }

                my $id = $db.save-program($name, $author, $bin-program);
                created "program/$id", 'application/json', %(
                    id      => $id,
                    name    => $name,
                    author  => $author,
                    program => encode-base64($bin-program, :str),
                );

                CATCH {
                    when X::ValidationFail {
                        bad-request 'application/json', %(
                            field   => .field,
                            message => .message,
                        );
                    }
                }
            }
        }

        # Enqueue a program
        post -> 'queue', UInt $id where * > 0 {
            my %program = $db.load-program($id);
            with %program<id> {
                $db.enqueue-program($id);
            }
            else {
                not-found, 'application/json', %;
            }
        }
    }

    my $japh-routes = route {

        # List all programs
        get -> 'program' {
            content 'application/json', %(
                programs => $db.list-programs,
            );
        }

        # Get a program
        get -> 'program', UInt $id where * > 0 {
            my %program = $db.load-program($id);
            if %program {
                content 'application/json', %(
                    id      => $id,
                    name    => %program<name>,
                    author  => %program<author>,
                    program => encode-base64(%program<descriptor>, :str),
                );
            }
            else {
                not-found, 'application/json', %;
            }
        }
    }


    my $nametag-routes = route {
        before NameTagAuth.new(:$db);

        # Dequeue a program
        post -> NameTagID $tag, 'next-program' {
            content 'application/octet', $db.dequeue-program;
        }
    }

    route {
        include $rate-limited-routes;
        include $japh-routes;
        include $nametag-routes;
    }
}

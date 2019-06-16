use v6;

use Base64;
use Cro::HTTP::Middleware;
use Cro::HTTP::Router;

use DB;
use NameTag;
use NameTag::ProgramValidator;

subset AutoID of UInt where * > 0;
subset ProgramName of Str where /^ <[- _ a..z A..Z 0..9 .]> ** 3..* $/;
subset QueueName of Str where /^ <[- _ a..z A..Z 0..9 .]> ** 3..* $/;
subset AdminToken of Str where /^ <[ a..z A..Z 0..9 ]> ** 40 $/;

class LoggedAdmin does Cro::HTTP::Auth { }

class AdminAuth does Cro::HTTP::Middleware::Request {
    has $.admin-token;

    submethod TWEAK {
        die "missing ADMIN_TOKEN" without %*ENV<ADMIN_TOKEN>;
        die "ADMIN_TOKEN must be formatted as 40 letters and numbers"
            unless %*ENV<ADMIN_TOKEN>.Str ~~ AdminToken;


        $!admin-token = %*ENV<ADMIN_TOKEN>.Str;
    }

    method process(Supply $requests --> Supply) {
        # I trust anyone who has a private key matching the server's
        # public key... because that should only be me. :)
        supply whenever $requests -> $req {
            my $auth = Nil;
            my $header = $req.header('Authorization');
            with $header -> $auth-header {
                my ($type, $credentials) = $auth-header.split(' ', 2);

                if $type.lc eq 'bearer' {
                    $auth = LoggedAdmin.new if $credentials eq $!admin-token;
                }
            }

            $req.auth = $auth;
            emit $req;
        }
    }
}

sub check-auth(&code) {
    if request.auth ~~ LoggedAdmin {
        code();
    }
    else {
        response.status = 401;
        content 'application/json', %(
            error   => True,
            message => 'unauthorized',
        );
    }
}

class X::ValidationFail is Exception {
    has $.field;
    has $.message;
    multi method new($field, $message) { self.bless(:$field, :$message) }
    method Str() { "$!field: $!message" }
}

sub routes() is export {
    my $db = DB.new;
    $db.create-schema;

    my $rate-limited-routes = route {
        before AdminAuth.new;

        # Upload a program
        post -> 'program' {
            check-auth {
                request-body -> %program {
                    %program<name>.defined && %program<name> ~~ /\w+/
                        or die X::ValidationFail.new("name", "You must give your program a name.");
                    my $name = %program<name>.trim;

                    %program<author>.defined && %program<author> ~~ /^[github|cpan|http|https]:\S+/
                        or die X::ValidationFail.new("author", "You must give a URL claiming authorship, e.g., github:zostay, cpan:HANENKAMP, or https://sterling.hanenkamp.com/");
                    my $author = %program<author>.trim;

                    %program<program>.defined && %program<program> ~~ m{^<[a..zA..Z0..9+/]> ** 22..87382 '='* $}
                        or die X::ValidationFail.new("program", "You must provide the program as a BASE-64 encoded string.");
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
                                error   => True,
                                field   => .field,
                                message => .message,
                            );
                        }
                    }
                }
            }
        }

        # Enqueue a program
        post -> 'queue', QueueName $name {
            check-auth {
                request-body -> %program-info (AutoID :$id) {
                    my %program = $db.load-program($id);
                    with %program<id> {
                        $db.enqueue-program($name, $id);

                        content 'application/json', %;
                    }
                    else {
                        bad-request 'application/json', %(
                            error   => True,
                            field   => 'id',
                            message => 'You must name the program ID to enqueue.',
                        );
                    }
                }
            }
        }

        # List programs in a queue
        get -> 'queue', QueueName $name {
            my @programs = $db.list-queue($name);
            content 'application/json', %(:@programs);
        }

        # Overwrite the queue completely
        put -> 'queue', QueueName $name {
            check-auth {
                request-body -> %program-info {
                    my $bad = False;
                    for @(%program-info<programs>) -> %p {
                        unless %p<id>.defined && %p<id> ~~ AutoID {
                            $bad = True;
                            last;
                        }
                    }

                    if $bad {
                        bad-request 'application/json', %(
                            error => True,
                            field => 'programs',
                            message => 'You must name uploaded program IDs to set the queue.',
                        );
                    }

                    my @programs = $db.set-queue($name, %program-info<programs>);
                    content 'application/json', %(:@programs);
                }
            }
        }
    }

    my $japh-routes = route {

        # List all programs
        get -> 'program' {
            my @programs = $db.list-programs;
            content 'application/json', %(:@programs);
        }

        # Get a program
        get -> 'program', AutoID $id {
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
        before AdminAuth.new;

        # Dequeue a program
        post -> 'next-program', QueueName $queue-name {
            check-auth {
                content 'application/octet', $db.dequeue-program($queue-name);
            }
        }

        # Get a specific program
        get -> 'program', AutoID $id, 'descriptor' {
            check-auth {
                my %program = $db.load-program($id);
                content 'application/octet', %program<descriptor>;
            }
        }
    }

    route {
        include $rate-limited-routes;
        include $japh-routes;
        include $nametag-routes;
    }
}

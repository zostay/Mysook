use Cro::HTTP::Log::File;
use Cro::HTTP::Server;
use Routes;

my Cro::Service $http = Cro::HTTP::Server.new(
    http => <1.1>,
    host => %*ENV<ONAIRLIGHT_HOST> ||
        die("Missing ONAIRLIGHT_HOST in environment"),
    port => %*ENV<ONAIRLIGHT_PORT> ||
        die("Missing ONAIRLIGHT_PORT in environment"),
    application => routes(),
    after => [
        Cro::HTTP::Log::File.new(logs => $*OUT, errors => $*ERR)
    ]
);
$http.start;
say "Listening at http://%*ENV<ONAIRLIGHT_HOST>:%*ENV<ONAIRLIGHT_PORT>";
react {
    whenever signal(SIGINT) {
        say "Shutting down...";
        $http.stop;
        done;
    }
}

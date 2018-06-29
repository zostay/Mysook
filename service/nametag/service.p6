use Cro::HTTP::Log::File;
use Cro::HTTP::Server;
use Routes;

my Cro::Service $http = Cro::HTTP::Server.new(
    http => <1.1>,
    host => %*ENV<NAMETAG_HOST> ||
        die("Missing NAMETAG_HOST in environment"),
    port => %*ENV<NAMETAG_PORT> ||
        die("Missing NAMETAG_PORT in environment"),
    application => routes(),
    after => [
        Cro::HTTP::Log::File.new(logs => $*OUT, errors => $*ERR),
    ]
);
$http.start;
say "Listening at http://%*ENV<NAMETAG_HOST>:%*ENV<NAMETAG_PORT>";
react {
    whenever signal(SIGINT) {
        say "Shutting down...";
        $http.stop;
        done;
    }
}

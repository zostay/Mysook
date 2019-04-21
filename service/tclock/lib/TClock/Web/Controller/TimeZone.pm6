use v6;

use Cofra::Web::Controller;
use X::Cofra::Web::Error;

use TClock::Biz::TimeZone;

unit class TClock::Web::Controller::TimeZone is Cofra::Web::Controller;

has TClock::Biz::TimeZone $.time-zone is required;

method fetch($request) is action {
    my $config = $.time-zone.fetch;
    my $json   = $.web.view('JSON', $request);
    $json.render($config);
}

method store($request) is action {
    use JSON::Fast;

    my $config  = $.time-zone.fetch;
    my $json    = $.web.view('JSON', $request);
    my $in-json = $request.content.&from-json;

    $config.offset = $in-json<offset>;

    $.time-zone.store($config);

    $json.render($config);
}

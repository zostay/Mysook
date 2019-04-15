use v6;

use Cofra::Web::Controller;
use X::Cofra::Web::Error;

use TClock::Biz::Time;

unit class TClock::Web::Controller::Time is Cofra::Web::Controller;

has TClock::Biz::Time $.time is required;

method fetch($request) is action {
    my $json = $.web.view('JSON', $request);

    my $now = $.time.now;
    given $request.path-parameters<c> {
        when 'year'|'month'|'day'|'hour'|'minute'|'second' {
            $json.render(%( $_ => $now."$_"() ));
        }
        when !*.defined {
            $json.render(%( time => $now ));
        }
        default {
            die X::Cofra::Web::Error::NotFound.new;
        }
    }
}

use v6;

use Cofra::Web::Controller;
use X::Cofra::Web::Error;

use TClock::Biz::Alarm;

unit class TClock::Web::Controller::Alarm is Cofra::Web::Controller;

has TClock::Biz::Alarm $.alarm is required;

subset AlarmGroup of Str where 'morning-time' | 'sleeping-time';
subset ColorGroup of Str where 'night-color' | 'day-color';
subset AlarmSegment of Str where 'hour' | 'minute';
subset ColorSegment of Str where 'red' | 'green' | 'blue' | 'brightness';

multi method partial-fetch(AlarmGroup $alarm, $config, $json) {
    $json.render(%( $alarm => $config."$alarm"() ));
}

multi method partial-fetch(ColorGroup $color, $config, $json) {
    $json.render(%( $color => $config."$color"() ));
}

multi method partial-fetch($, $, $) {
    die X::Cofra::Web::Error::NotFound.new;
}

multi method partial-fetch(AlarmGroup $alarm, AlarmSegment $method, $config, $json) {
    $json.render(
        %(
            $alarm => %(
                $method => $config."$alarm"()."$method"(),
            ),
        ),
    );
}

multi method partial-fatch(ColorGroup $color, ColorSegment $method, $config, $json) {
    $json.render(
        %(
            $color => %(
                $method => $config."$color"()."$method"(),
            ),
        ),
    );
}

multi method partial-fetch($, $, $, $) {
    die X::Cofra::Web::Error::NotFound.new;
}

method fetch($request) is action {
    my $config = $.alarm.fetch;
    my $json   = $.web.view('JSON', $request);

    if all($request.path-parameters<c1 c2>).defined {
        self.partial-fetch(
            $request.path-parameters<c1>,
            $request.path-parameters<c2>,
            $config,
            $json,
        );
    }
    elsif $request.path-parameters<c1>.defined {
        self.partial-fetch(
            $request.path-parameters<c1>,
            $config,
            $json,
        );
    }
    else {
        $json.render($config);
    }
}

multi method partial-enmesh(AlarmGroup $alarm, AlarmSegment $method, $config, Int $input) {
    $config."$alarm"()."$method"() = $input;
    $config;
}

multi method partial-enmesh(ColorGroup $color, ColorSegment $method, $config, Int $input) {
    $config."$color"()."$method"() = $input;
    $config;
}

multi method partial-enmesh($, $, $, $) {
    die X::Cofra::Web::Error::NotFound.new;
}

multi method partial-enmesh(AlarmGroup $alarm, $config, %input) {
    $config."$alarm"().enmesh(%input{$alarm});
    $config;
}

multi method partial-enmesh(ColorGroup $color, $config, %input) {
    $config."$color"().enmesh(%input{$color});
    $config;
}

multi method partial-enmesh($, $, $) {
    die X::Cofra::Web::Error::NotFound.new;
}

method store($request) is action {
    use JSON::Fast;

    my $config  = $.alarm.fetch;
    my $json    = $.web.view('JSON', $request);

    my $new-config = do {
        if all($request.path-parameters<c1 c2>).defined {
            my $in = $request.content;
            self.partial-enmesh(
                $request.path-parameters<c1>,
                $request.path-parameters<c2>,
                $config,
                $in,
            );
        }
        elsif $request.path-parameters<c1>.defined {
            my $in-json = $request.content.&from-json;
            self.partial-enmesh(
                $request.path-parameters<c1>,
                $config,
                $in-json,
            );
        }
        else {
            my $in-json = $request.content.&from-json;
            $config.enmesh($in-json);
        }
    }

    $.alarm.store($new-config);

    self.fetch($request);
}

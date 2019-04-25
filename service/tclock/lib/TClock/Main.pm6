use v6;

no precompilation;

use Cofra::Singleton;
use Cofra::Web::Main;

unit class TClock::Main is Cofra::Web::Main does Cofra::Singleton['tclock-main'];

use Cofra::IOC;
use TClock;

has IO::Path $.config-dir is required;

my method json-config-file-name(:$name) {
    self.config-dir.add($name);
}

has IO::Path $.tz-file is factory(&json-config-file-name);
has IO::Path $.alarm-file is factory(&json-config-file-name);

has TClock $.app is constructed is construction-args(
    \(
        logger => dep,
        bizzes => dep,
    )
) is post-initialized(anon method initialize-app(Cofra::App:D:) {
    .app = self for %.bizzes.values;
});

use TClock::Biz::Alarm;
has TClock::Biz::Alarm $.alarm-biz is constructed is construction-args(
    \(
        alarm-file => dep,
    )
);

use TClock::Biz::TimeZone;
has TClock::Biz::TimeZone $.time-zone-biz is constructed is construction-args(
    \(
        tz-file => dep,
    )
);

use TClock::Biz::Time;
has TClock::Biz::Time $.time-biz is constructed is construction-args(
    \(
        time-zone => dep('time-zone-biz'),
    )
);

has Hash[Cofra::Biz] $.bizzes is constructed is construction-args(
    \(
        'alarm',     dep('alarm-biz'),
        'time',      dep('time-biz'),
        'time-zone', dep('time-zone-biz'),
    )
);

use TClock::Web::Controller::Alarm;
has TClock::Web::Controller::Alarm $.alarm-controller is constructed is construction-args(
    \(
        alarm => dep('alarm-biz'),
    )
);

use TClock::Web::Controller::Time;
has TClock::Web::Controller::Time $.time-controller is constructed is construction-args(
    \(
        time => dep('time-biz'),
    )
);

use TClock::Web::Controller::TimeZone;
has TClock::Web::Controller::TimeZone $.time-zone-controller is constructed is construction-args(
    \(
        time-zone => dep('time-zone-biz'),
    )
);

has Hash[Cofra::Web::Controller] $.controllers is constructed is construction-args(
    \(
        'alarm',     dep('alarm-controller'),
        'time',      dep('time-controller'),
        'time-zone', dep('time-zone-controller'),
    )
);

use Cofra::Web::View::JSON;
has Cofra::Web::View::JSON $.json-view is constructed;

has Hash[Cofra::Web::View] $.views is constructed is construction-args(
    \(
        'JSON', dep('json-view'),
    )
);

use TClock::Web::Router;
has TClock::Web::Router $.router is constructed;

use v6;

no precompilation;

use Cofra::Singleton;
use Cofra::Web::Main;

unit class TClock::Main is Cofra::Web::Main does Cofra::Singleton['tclock-main'];

use Cofra::IOC;
use TClock;

has Str $.ddb-hostname is required;
has Int $.ddb-port = 8000;
has Str $.alarm-table = 'TClockAlarm';

use Amazon::DynamoDB;
has Amazon::DynamoDB $.ddb is factory(anon method build-ddb {
    if $.ddb-hostname {
        Amazon::DynamoDB.new(
            scheme => 'http',
            hostname => $.ddb-hostname,
            |(do with $.ddb-port { port => $.ddb-port }),
        );
    }
    else {
        Amazon::DynamoDB.new;
    }
});

has TClock $.app is constructed is construction-args(
    \(
        ddb    => dep,
        logger => dep,
        bizzes => dep,
    )
) is post-initialized(anon method initialize-app(Cofra::App:D:) {
    .app = self for %.bizzes.values;
});

use TClock::Biz::Alarm;
has TClock::Biz::Alarm $.alarm-biz is constructed is construction-args(
    \(
        ddb   => dep,
        table => dep('alarm-table'),
    )
);

has Hash[Cofra::Biz] $.bizzes is constructed is construction-args(
    \(
        alarm => dep('alarm-biz'),
    )
);

use TClock::Web::Controller::Alarm;
has Hash[Cofra::Web::Controller] $.controllers is constructed is construction-args(
    \(
        alarm => dep('alarm-controller'),
    )
);

use Cofra::Web::View::JSON;
has Cofra::Web::View::JSON $.json-view is constructed;

has Hash[Cofra::Web::View] $.views is constructed is construction-args(
    \(
        json => dep('json-view'),
    )
);

use TClock::Web::Router;
has TClock::Web::Router $.router is constructed;

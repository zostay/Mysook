use v6;

use Cofra::Biz;
use JSON::Class;

unit class TClock::Biz::TimeZone is Cofra::Biz;

class GLOBAL::TClock::Model::TimeZone does JSON::Class {
    subset TZOffset of Int where 86400 > * > -86400;

    has TZOffset $.offset is rw = 0;
}

has IO::Path $.tz-file is required;

method fetch(--> TClock::Model::TimeZone:D) {
    if $.tz-file.f {
        TClock::Model::TimeZone.from-json($.tz-file.slurp);
    }
    else {
        TClock::Model::TimeZone.new;
    }
}

method store(TClock::Model::TimeZone:D $time-zone) {
    $.tz-file.spurt: $time-zone.to-json;
}

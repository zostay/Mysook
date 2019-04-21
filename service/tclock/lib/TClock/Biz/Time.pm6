use v6;

use Cofra::Biz;
use JSON::Class;

use TClock::Biz::TimeZone;

unit class TClock::Biz::Time is Cofra::Biz;

has TClock::Biz::TimeZone $.time-zone is required;

class GLOBAL::TClock::Model::Time does JSON::Class {
    subset Year of Int where * >= 2019;
    subset Month of Int where 12 >= * >= 1;
    subset Day of Int where 31 >= * >= 1;

    subset Hour of Int where 24 > * >= 0;
    subset Minute of Int where 60 > * >= 0;
    subset Second of Int where 60 > * >= 0;

    has Year $.year;
    has Month $.month;
    has Day $.day;

    has Hour $.hour;
    has Minute $.minute;
    has Second $.second;

    has Int $.timezone;

    #| Coercion is helpful for testing.
    method DateTime(:$time-zone --> DateTime:D) {
        DateTime.new(
            :$!year, :$!month, :$!day,
            :$!hour, :$!minute, :$!second,
            :$!timezone,
        );
    }
}

method now() {
    given DateTime.now {
        my $timezone = $.time-zone.fetch.offset;
        TClock::Model::Time.new(
            year   => .year,
            month  => .month,
            day    => .day,

            hour   => .hour,
            minute => .minute,
            second => .second.Int,

            :$timezone,
        );
    }
}

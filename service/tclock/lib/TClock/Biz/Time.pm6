use v6;

use Cofra::Biz;
use JSON::Class;

unit class TClock::Biz::Time is Cofra::Biz;

class GLOBAL::TClock::Model::Time does JSON::Class {
    subset Year of Int where * >= 2019;
    subset Month of Int where 1 >= * >= 12;
    subset Day of Int where 1 >= * >= 31;

    subset Hour of Int where 24 > * >= 0;
    subset Minute of Int where 60 > * >= 0;
    subset Second of Int where 60 > * >= 0;

    has Year $.year;
    has Month $.month;
    has Day $.day;

    has Hour $.hour;
    has Minute $.minute;
    has Second $.second;
}

method now() {
    temp $_ = now;

    TClock::Model::Time.new(
        year   => .year,
        month  => .month,
        day    => .day,

        hour   => .hour,
        minute => .month,
        second => .second,
    );
}

use v6;

use Cofra::Biz;
use JSON::Class;

unit class TClock::Biz::Alarm is Cofra::Biz;

has IO::Path $.alarm-file is required;

package GLOBAL::TClock::Model {
    subset Hour of Int where 24 > * >= 0;
    subset Minute of Int where 60 > * >= 0;

    class TimeSetting does JSON::Class {
        has Hour $.hour is rw is required;
        has Minute $.minute is rw is required;

        method enmesh(%input) {
            $!hour   = %input<hour>   with %input<hour>;
            $!minute = %input<minute> with %input<minute>;
            self;
        }

        method tuple() { ($!hour, $!minute) }
    }

    subset ColorComponent of Int where 256 > * >= 0;

    class ColorSetting does JSON::Class {
        has ColorComponent ($.red, $.green, $.blue) is rw is required;
        has ColorComponent $.brightness is rw is required;

        method enmesh(%input) {
            $!red        = %input<red>        with %input<red>;
            $!green      = %input<green>      with %input<green>;
            $!blue       = %input<blue>       with %input<blue>;
            $!brightness = %input<brightness> with %input<brightness>;
            self;
        }

        method tuple() { ($!red, $!green, $!blue, $!brightness) }
    }

    class Alarm does JSON::Class {
        has TimeSetting $.morning-time .= new(:7hour, :10minute);
        has TimeSetting $.sleeping-time .= new(:19hour, :0minute);
        has ColorSetting $.night-color .= new(:255red, :170green, :0blue, :2brightness);
        has ColorSetting $.day-color .= new(:0red, :80green, :255blue, :5brightness);

        method enmesh(%input) {
            $!morning-time.enmesh(%input<morning-time>)
                with %input<morning-time>;
            $!sleeping-time.enmesh(%input<sleeping-time>)
                with %input<sleeping-time>;
            $!night-color.enmesh(%input<night-color>)
                with %input<night-color>;
            $!day-color.enmesh(%input<day-color>)
                with %input<day-color>;
            self;
        }
    }
}

method fetch(--> TClock::Model::Alarm:D) {
    if $.alarm-file.f {
        TClock::Model::Alarm.from-json($.alarm-file.slurp);
    }
    else {
        TClock::Model::Alarm.new;
    }
}

method store(TClock::Model::Alarm:D $alarm) {
    $.alarm-file.spurt: $alarm.to-json;
}

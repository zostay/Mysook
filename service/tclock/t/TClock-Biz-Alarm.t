use v6;
use Test;

use Cofra::Biz;
use TClock::Biz::Alarm;

my $alarm-file = $*TMPDIR.add(rand ~ '.json');
my TClock::Biz::Alarm $alarm .= new(:$alarm-file);
END $alarm-file.unlink;

ok $alarm.defined;

# TODO Why does this next test fail like this?
#     # Failed test 'The object is-a 'Cofra::Biz''
#     # at t/TClock-Biz-Alarm.t line X
#     # Actual type: TClock::Biz::Alarm
todo 'The class is Cofra::Biz, but this test fails.';
isa-ok $alarm, Cofra::Biz;
cmp-ok $alarm, '~~', Cofra::Biz, 'TClock::Biz::Alarm ~~ Cofra::Biz';

lives-ok {
    my $a = $alarm.fetch;
    is $a.morning-time.tuple, (7, 10),'default morning-time is 7:10';
    is $a.sleeping-time.tuple, (19, 0), 'default sleeping-time is 19:00';
    is $a.night-color.tuple, (255, 170, 0, 2), 'default night-color is (255, 170, 0) at 2';
    is $a.day-color.tuple, (0, 80, 255, 5), 'default day-color is (0, 80, 255) at 5';

    $a.morning-time.hour   = 6;
    $a.morning-time.minute = 45;

    $a.sleeping-time.hour   = 20;
    $a.sleeping-time.minute = 30;

    $a.night-color.red        = 127;
    $a.night-color.green      = 85;
    $a.night-color.blue       = 255;
    $a.night-color.brightness = 4;

    $a.day-color.red        = 255;
    $a.day-color.green      = 160;
    $a.day-color.blue       = 127;
    $a.day-color.brightness = 10;

    $alarm.store($a);
}, 'setting Alarm works #1';

lives-ok {
    my $a = $alarm.fetch;
    is $a.morning-time.tuple, (6, 45),'set morning-time is 6:45';
    is $a.sleeping-time.tuple, (20, 30), 'set sleeping-time is 20:30';
    is $a.night-color.tuple, (127, 85, 255, 4), 'set night-color is (127, 85, 255) at 4';
    is $a.day-color.tuple, (255, 160, 127, 10), 'set day-color is (255, 160, 127) at 10';

    $a.morning-time.hour   = 8;
    $a.morning-time.minute = 23;

    $a.sleeping-time.hour   = 18;
    $a.sleeping-time.minute = 55;

    $a.night-color.red        = 63;
    $a.night-color.green      = 42;
    $a.night-color.blue       = 127;
    $a.night-color.brightness = 8;

    $a.day-color.red        = 127;
    $a.day-color.green      = 80;
    $a.day-color.blue       = 63;
    $a.day-color.brightness = 20;

    $alarm.store($a);
}, 'setting Alarm works #2';

lives-ok {
    my $a = $alarm.fetch;
    is $a.morning-time.tuple, (8, 23),'set morning-time is 8:23';
    is $a.sleeping-time.tuple, (18, 55), 'set sleeping-time is 18:55';
    is $a.night-color.tuple, (63, 42, 127, 8), 'set night-color is (63, 42, 127) at 8';
    is $a.day-color.tuple, (127, 80, 63, 20), 'set day-color is (127, 80, 20) at 20';
}, 'reading Alarm works';

done-testing;

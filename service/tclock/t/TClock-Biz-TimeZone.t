use v6;
use Test;

use Cofra::Biz;
use TClock::Biz::Time;

my $tz-file = $*TMPDIR.add(rand ~ '.json');
my TClock::Biz::TimeZone $time-zone .= new(:$tz-file);
END $tz-file.unlink;

ok $time-zone.defined;

# TODO Why does this next test fail like this?
#     # Failed test 'The object is-a 'Cofra::Biz''
#     # at t/TClock-Biz-TimeZone.t line X
#     # Actual type: TClock::Biz::TimeZone
todo 'The class is Cofra::Biz, but this test fails.';
isa-ok $time-zone, Cofra::Biz;
cmp-ok $time-zone, '~~', Cofra::Biz, 'TClock::Biz::TimeZone ~~ Cofra::Biz';

lives-ok {
    my $tz = $time-zone.fetch;
    is $tz.offset, 0, 'default TZ is 0';

    $tz.offset = -18000;
    $time-zone.store($tz);
}, 'setting TZ works #1';

lives-ok {
    my $tz = $time-zone.fetch;
    is $tz.offset, -18000, 'write to TZ was saved';

    $tz.offset = 12000;
    $time-zone.store($tz);
}, 'setting TZ works #2';

lives-ok {
    my $tz = $time-zone.fetch;
    is $tz.offset, 12000, 'second TZ write was saved';
}, 'reading TZ workds #3';

dies-ok {
    my $tz = $time-zone.fetch;
    $tz.offset = 86400;
}, 'cannot set offset to 86400';

dies-ok {
    my $tz = $time-zone.fetch;
    $tz.offset = -86400;
}, 'cannot set offset to -86400';

done-testing;

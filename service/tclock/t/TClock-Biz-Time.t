use v6;
use Test;

use Cofra::Biz;
use TClock::Biz::Time;

my TClock::Biz::TimeZone $time-zone .= new(
    tz-file => 't/data/tzfile.json'.IO,
);

my TClock::Biz::Time $time .= new(
    time-zone => $time-zone,
);

ok $time.defined;

# TODO Why does this next test fail like this?
#     # Failed test 'The object is-a 'Cofra::Biz''
#     # at t/TClock-Biz-Time.t line X
#     # Actual type: TClock::Biz::Time
todo 'The class is Cofra::Biz, but this test fails.';
isa-ok $time, Cofra::Biz;
cmp-ok $time, '~~', Cofra::Biz, 'TClock::Biz::Time ~~ Cofra::Biz';

my DateTime $before .= now;
sleep 1; # since the time service drops nanos...
my $now = $time.now.DateTime(time-zone => $time.time-zone);
my DateTime $after  .= now;

cmp-ok $now, '>=', $before;
cmp-ok $now, '<=', $after;

done-testing;

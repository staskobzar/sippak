#
# Acceptence test:
# Run sippak subscribe method SIP flow
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/subscribe.terminated.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak publish basic
$output = `$sippak SUBSCRIBE -X 0 sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^SUBSCRIBE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic SUBSCRIBE packet sent.");

$regex = '^Expires: 0$';
ok ($output =~ m/$regex/m, "Subscription expires is set to 0.");

$regex = 'Subscription is terminated';
ok ($output =~ m/$regex/m, "Basic SUBSCRIBE subscription is terminated.");

done_testing();

#
# Acceptence test:
# Run sippak basic invite SIP flow
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/invite.cancel.xml";
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
$output = `$sippak INVITE --cancel sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^INVITE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic INVITE packet sent.");

$regex = '^SIP\/2.0 180 Ringing$';
ok ($output =~ m/$regex/m, "Ringing 180 early state.");

$regex = '^SIP\/2.0 200 Canceled$';
ok ($output =~ m/$regex/m, "Cancel invite session is confirmed.");

$regex = '^CANCEL sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "INVITE request canceled.");

$regex = '^SIP\/2.0 200 Canceled$';
ok ($output =~ m/$regex/m, "Cancel confirmed with 200.");

$regex = '^SIP\/2.0 487 Request Terminated$';
ok ($output =~ m/$regex/m, "Cancel response 487 sent.");

$regex = '^ACK sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Response 487 acknowlaged.");

done_testing();

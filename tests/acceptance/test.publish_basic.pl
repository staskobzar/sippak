#
# Acceptence test:
# Run sippak ping command without colors and trailing dot
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/publish.basic.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak ping basic
$output = `$sippak PUBLISH sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^PUBLISH sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic PUBLISH packet sent.");

# test response
$regex = '^SIP\/2\.0 200 OK Basic PUBLISH Test$';
ok ($output =~ m/$regex/m, "Basic PUBLISH Response 200 OK received.");

done_testing();

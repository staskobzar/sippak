#
# Acceptence test:
# Run sippak ping command via TCP
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/options.basic.xml";
my $sippargs = "-timeout 10s -t t1 -p 5061 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run test with TCP
$output = `$sippak --proto=tcp sip:alice\@127.0.0.1:5061`;

# test request
$regex = '^Via: SIP\/2\.0\/TCP ';
ok ($output =~ m/$regex/m, "Basic OPTIONS packet sent via TCP.");

# test response
$regex = '^SIP\/2\.0 200 OK Basic OPTIONS Test$';
ok ($output =~ m/$regex/m, "Basic OPTIONS Response 200 OK via TCP.");

done_testing();

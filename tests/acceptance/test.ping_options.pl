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
my $scenario = $ARGV[2] . "/options.basic.xml";
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
$output = `$sippak --ns=127.0.0.1 sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^OPTIONS sip:alice\@127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic OPTIONS packet sent.");

# test response
$regex = '^SIP\/2\.0 200 OK Basic OPTIONS Test$';
ok ($output =~ m/$regex/m, "Basic OPTIONS Response 200 OK received.");

# ------------------------------------------------------------------
# run sippak ping output with trailing dots
system("$sipp $sippargs -sf $scenario");
$output = `$sippak --trail-dot sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^OPTIONS sip:alice\@127\.0\.0\.1:5060 SIP\/2.0\.$';
ok ($output =~ m/$regex/m, "Basic OPTIONS with trailing dot output.");

# test response
$regex = '^SIP\/2\.0 200 OK Basic OPTIONS Test\.$';
ok ($output =~ m/$regex/m, "Basic OPTIONS Response 200 OK with trailing dot output.");

# ------------------------------------------------------------------
# run sippak ping output with color decoration
system("$sipp $sippargs -sf $scenario");
$output = `$sippak --color sip:alice\@127.0.0.1:5060`;

$regex = '^(\x1b\[[0-9;]+m){2}OPTIONS \x1b\[[0-9;]+msip:alice\@127\.0\.0\.1:5060';
ok ($output =~ m/$regex/m, "Basic OPTIONS Request with colored output.");

$regex = '^(\x1b\[[0-9;]+m){2}SIP/2.0 (\x1b\[[0-9;]+m)+200';
ok ($output =~ m/$regex/m, "Basic OPTIONS Response with colored output.");

done_testing();

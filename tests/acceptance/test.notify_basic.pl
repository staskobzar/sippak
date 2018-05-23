#
# Acceptence test:
# Run sippak NOTIFY command with presence info and check-sync
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/notify.basic.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak notify basic presence with pidf
$output = `$sippak notify sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^NOTIFY sip:alice\@127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic NOTIFY packet sent.");

$regex = '^SIP\/2\.0 200 OK Basic NOTIFY Test$';
ok ($output =~ m/$regex/m, "Basic NOTIFY response displayed.");

$regex = '^Event: keep-alive$';
ok ($output =~ m/$regex/m, "Default event is 'keep-alive'");

# presence pidf/xpidf
#$regex = '^<basic>open</basic>$';
#ok ($output =~ m/$regex/m, "Default status is open");

#$regex = '^Content-Type: application/pidf\+xml$';
#ok ($output =~ m/$regex/m, "Default content type is application/pidf+xml");
# ------------------------------------------------------------------
# run sippak ping output with trailing dots

# system("$sipp $sippargs -sf $scenario");
# $output = `$sippak --trail-dot sip:alice\@127.0.0.1:5060`;

# test request
# $regex = '^OPTIONS sip:alice\@127\.0\.0\.1:5060 SIP\/2.0\.$';
# ok ($output =~ m/$regex/m, "Basic OPTIONS with trailing dot output.");

done_testing();

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
my $scenario = $ARGV[2] . "/register.no_auth.xml";
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
$output = `$sippak register sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^REGISTER sip:127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic REGISTER packet sent.");

$regex = '^SIP\/2\.0 200 OK Register no auth$';
ok ($output =~ m/$regex/m, "Basic REGISTER register confirmed.");

# rfc3665 request current contact list
system("$sipp $sippargs -sf $scenario");
$output = `$sippak register --clist sip:alice\@127.0.0.1:5060`;

my @lines = split(/\n/, $output);
my $pack = '';
foreach (@lines) {
  if(/^REGISTER/../\.RX/) {
    $pack .= "$_\n";
  }
}
$regex = '^REGISTER sip:127\.0\.0\.1:5060 SIP\/2.0$';
ok ($pack =~ m/$regex/m, "REGISTER packet sent for contact list.");

ok ($pack !~ m/Contact: /m, "No Contact header in list request");

# rfc3665 cancel all registrations
system("$sipp $sippargs -sf $scenario");
$output = `$sippak register --cancel-all sip:alice\@127.0.0.1:5060`;

$regex = '^REGISTER sip:127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "REGISTER cancel all packet sent.");

$regex = '^Contact: \*$';
ok ($output =~ m/$regex/m, "Cancel all registartions contact header is asterisk.");

$regex = '^Expires: 0$';
ok ($output =~ m/$regex/m, "Cancel all registartions expires header is zero.");

done_testing();

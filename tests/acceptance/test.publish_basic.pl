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

# run sippak publish basic
$output = `$sippak PUBLISH sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^PUBLISH sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic PUBLISH packet sent.");

# test response
$regex = '^SIP\/2\.0 200 OK Basic PUBLISH Test$';
ok ($output =~ m/$regex/m, "Basic PUBLISH Response 200 OK received.");

# contenct type is Content-Type: application/pidf+xml
$regex = '^Content-Type: application/pidf\+xml$';
ok ($output =~ m/$regex/m, "Basic PUBLISH content type is pidf+xml.");

# default expire is 3600
$regex = '^Expires: 3600$';
ok ($output =~ m/$regex/m, "Expires default value is 3600");

# invalid Expires header value
$output = `$sippak PUBLISH --expires=5foo sip:alice\@127.0.0.1:5060`;
$regex = 'Invalid expires value: 5foo. Must be numeric.';
ok ($output =~ m/$regex/m, "Invalid Expires value set.");

# run sippak publish with expire set value
system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH -E 84877 sip:alice\@127.0.0.1:5060`;
# test request
$regex = '^Expires: 84877$';
ok ($output =~ m/$regex/m, "Set expire value 84877.");

# run sippak publish with expire set to 0
$output = `$sippak PUBLISH --expires=0 sip:alice\@127.0.0.1:5060`;
# test request
$regex = 'Expires header value must be more then 0.';
ok ($output =~ m/$regex/m, "Set expire value to 0 not allowed.");

# test basic opens/close
system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH sip:alice\@127.0.0.1:5060`;

$regex = '<basic>open</basic>';
ok ($output =~ m/$regex/m, "Default status is 'open'.");

system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH --pres-status=closed sip:alice\@127.0.0.1:5060`;

$regex = '<basic>closed</basic>';
ok ($output =~ m/$regex/m, "Set presence status to 'closed'.");

system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH --pres-status=open sip:alice\@127.0.0.1:5060`;

$regex = '<basic>open</basic>';
ok ($output =~ m/$regex/m, "Set presence status to 'open'.");

system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH --pres-status=foo sip:alice\@127.0.0.1:5060`;

$regex = '<basic>open</basic>';
ok ($output =~ m/$regex/m, "For unknown presence status, use 'open'");

$regex = "Unknown presence status 'foo'. Will use 'open'.";
ok ($output =~ m/$regex/m, "Warning for unknown presence status.");

# test presence note message
system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH --pres-note="Will be back soon!" sip:alice\@127.0.0.1:5060`;

$regex = '<note>Will be back soon!</note>';
ok ($output =~ m/$regex/m, "Set pids note message.");

system("$sipp $sippargs -sf $scenario");
my $too_long_msg = 'This memo further defines the Presence Information Data Format as a common presence data format for CPP-compliant presence protocols, allowing presence information to be transferred across CPP-cstas@stas-aspire ~/Dev/sippak';
$output = `$sippak PUBLISH --pres-note="$too_long_msg" sip:alice\@127.0.0.1:5060`;

$regex = 'Note message is too long. Max 64 char allowed. Message is stripped.';
ok ($output =~ m/$regex/m, "Warning, max length of note is limited to 64 chars.");

$regex = '<note>This memo further defines the Presence Information Data Format a</note>';
ok ($output =~ m/$regex/m, "Long note message is stripped to 64 chars.");

# test XPIDF presence message
system("$sipp $sippargs -sf $scenario");
$output = `$sippak PUBLISH --pres-xpidf sip:alice\@127.0.0.1:5060`;

$regex = '^Content-Type: application/xpidf\+xml$';
ok ($output =~ m/$regex/m, "Basic PUBLISH content type is XPIDF.");

done_testing();

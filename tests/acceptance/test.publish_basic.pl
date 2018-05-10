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

done_testing();

#
# Acceptence test:
# Run sippak ping command with trailing dot at SIP package output
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;

my $retval = 0; # return value
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

# run sippak ping
$output = `$sippak --trail-dot sip:alice\@127.0.0.1:5060`;

# test request
$regex = 'OPTIONS sip:alice\@127\.0\.0\.1:5060 SIP\/2.0\.';
if ($output !~ /$regex/m) {
  print "\nTEST FAILED: Output doesn't match pattern \"$regex\".\n\n";
  $retval = 2;
}

# test response
$regex = 'SIP\/2\.0 200 OK Basic OPTIONS Test\.';
if ($output !~ /$regex/m) {
  print "\nTEST FAILED: Output doesn't match pattern \"$regex\".\n\n";
  $retval = 2;
}

print $output if ($retval gt 0);

exit ($retval);

#
# Acceptence test:
# Run sippak ping command and timeout in 10 transactions
#

use strict;
use warnings;
use Socket;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/refer.basic.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sippak refer click-to-dial basic
$output = `$sippak refer sip:alice\@127.0.0.1:5060`;

$regex = 'Failed. Requires parameter "--to" to initiate REFER request.';
ok ($output =~ m/$regex/m, "Refer method fails when no param --to set.");

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# refer click to dial send
$output = `$sippak refer --to=sip:bob\@bar.fr sip:alice\@127.0.0.1:5060`;

$regex = '^REFER sip:alice\@127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic REFER packet sent.");

$regex = '^Refer-To: sip:bob@bar.fr$';
ok ($output =~ m/$regex/m, "Basic REFER has valid Refer-To header.");

$regex = '^Refer-Sub: false$';
ok ($output =~ m/$regex/m, "Basic REFER header Refer-Sub must be false.");

done_testing();

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
my $scenario = $ARGV[2] . "/refer.auth.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak refer click-to-dial with authentication
$output = `$sippak refer --to=sip:bob\@sip.com sip:alice\@127.0.0.1:5060`;

# auth failed
$regex = '^SIP/2.0 407 Refer Proxy Authentication$';
ok ($output =~ m/$regex/m, "Proxy Authentication challange issued.");

$regex = '^SIP/2.0 407 Proxy Authentication Retry$';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed.");

$regex = 'Authentication failed';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed message.");

# auth success
system("$sipp $sippargs -sf $scenario");
$output = `$sippak refer -u alice -p pa55w0rd --to=sip:bob\@sip.com sip:alice\@127.0.0.1:5060`;

$regex = '^SIP/2.0 202 OK Accepted$';
ok ($output =~ m/$regex/m, "Proxy Authentication Success.");

done_testing();

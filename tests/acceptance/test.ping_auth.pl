#
# Acceptence test:
# Run sippak ping command with 407 Proxy Authentication handle
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/options.auth.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak ping basic with source port option
# short option
$output = `$sippak -P 8899 sip:alice\@127.0.0.1:5060`;

# auth failed
$regex = '^SIP/2.0 407 Proxy Authentication Required$';
ok ($output =~ m/$regex/m, "Proxy Authentication challange issued.");

$regex = 'Proxy-Authorization: Digest username="alice"';
ok ($output =~ m/$regex/m, "Poxy Auth digest with result resent.");

$regex = '^SIP/2.0 407 Proxy Authentication Retry$';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed.");

# auth success
system("$sipp -trace_logs -log_file /tmp/sipp.log $sippargs -sf $scenario");
$output = `$sippak -u alice -p pa55w0rd sip:alice\@127.0.0.1:5060`;

$regex = '^SIP/2.0 200 OK Auth$';
ok ($output =~ m/$regex/m, "Proxy Authentication Success.");

# fail with given invalid password
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u unknown -p invalid sip:alice\@127.0.0.1:5060`;

$regex = '^SIP/2.0 407 Proxy Authentication Required$';
ok ($output =~ m/$regex/m, "Proxy Authentication with given uaser/pass challange issued.");

$regex = '^SIP/2.0 407 Proxy Authentication Retry$';
ok ($output =~ m/$regex/m, "Proxy Authentication with given uaser/pass Failed.");

done_testing();

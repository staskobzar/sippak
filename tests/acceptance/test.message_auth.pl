#
# Acceptence test:
# Run sippak message method
#

use strict;
use warnings;
use Socket;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/message.auth.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak message requires parameter --body
$output = `$sippak message --body="Hello" sip:alice\@127.0.0.1:5060`;

# auth failed
$regex = '^SIP/2.0 407 Message Proxy Authentication$';
ok ($output =~ m/$regex/m, "Proxy Authentication challange issued.");

$regex = '^SIP/2.0 407 Proxy Authentication Retry$';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed.");

$regex = 'Authentication failed';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed message.");

# auth success
system("$sipp $sippargs -sf $scenario");
$output = `$sippak message -u alice -p pa55w0rd --body="Hello" sip:alice\@127.0.0.1:5060`;

$regex = '^SIP\/2.0 200 OK Auth$';
ok ($output =~ m/$regex/m, "Proxy Authentication Success.");

done_testing();

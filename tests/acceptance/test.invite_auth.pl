#
# Acceptence test:
# Run sippak basic invite SIP flow
#

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename;
use Test::More;

my $sippak = $ARGV[0];
my $sipp   = $ARGV[1];
my $scenario = $ARGV[2] . "/invite.auth.xml";
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
$output = `$sippak INVITE sip:alice\@127.0.0.1:5060`;

# test auth failed
$regex = '^INVITE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic INVITE packet sent.");

$regex = '^SIP\/2.0 407 Proxy Authentication Required$';
ok ($output =~ m/$regex/m, "Proxy auth response 407");

$regex = '^SIP\/2.0 407 Proxy Authentication Failed$';
ok ($output =~ m/$regex/m, "Proxy auth failed response 407");

# test auth success
system("$sipp $sippargs -sf $scenario");
$output = `$sippak INVITE -u alice -p pa55w0rd sip:alice\@127.0.0.1:5060`;

$regex = '^INVITE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic INVITE packet sent.");

$regex = '^SIP\/2.0 200 OK Invite$';
ok ($output =~ m/$regex/m, "Invite session is confirmed.");

$regex = '^ACK sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Invite 200 in acknowlaged.");

$regex = '^BYE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Session is terminated with BYE.");

$regex = '^SIP\/2.0 200 OK bye$';
ok ($output =~ m/$regex/m, "BYE method is confirmed.");

done_testing();

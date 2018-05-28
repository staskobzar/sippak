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
my $scenario = $ARGV[2] . "/register.auth.xml";
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
$output = `$sippak register -p passw0rd sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^REGISTER sip:127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic REGISTER packet sent.");

$regex = '^SIP/2.0 401 Authorization Required$';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed.");

$regex = '^SIP/2.0 401 Authorization Retry$';
ok ($output =~ m/$regex/m, "Authorization re-challange issued.");

$regex = 'Authentication failed';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed message.");

# auth success
system("$sipp $sippargs -sf $scenario");
$output = `$sippak register -p pa55w0rd sip:alice\@127.0.0.1:5060`;

$regex = '^SIP/2.0 401 Authorization Required$';
ok ($output =~ m/$regex/m, "Authorization challange issued for good password.");

$regex = '^SIP/2.0 200 Registered Ok$';
ok ($output =~ m/$regex/m, "Registration successfull.");

done_testing();

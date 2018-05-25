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
my $scenario = $ARGV[2] . "/notify.auth.xml";
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
$output = `$sippak notify sip:alice\@127.0.0.1:5060`;

# auth failed
$regex = '^SIP/2.0 407 Proxy Authentication Required$';
ok ($output =~ m/$regex/m, "Proxy Authentication challange issued.");

$regex = '^SIP/2.0 407 Proxy Authentication Retry$';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed.");

$regex = 'Authentication failed';
ok ($output =~ m/$regex/m, "Proxy Authentication Failed message.");

# auth success
system("$sipp $sippargs -sf $scenario");
$output = `$sippak notify -u alice -p pa55w0rd sip:alice\@127.0.0.1:5060`;

$regex = '^SIP/2.0 200 OK Auth$';
ok ($output =~ m/$regex/m, "Proxy Authentication Success.");

done_testing();

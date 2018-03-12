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

# run sippak ping basic with source port option
# short option
$output = `$sippak -p 8899 sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^Contact: \<sip:[^:]+:8899>$';
ok ($output =~ m/$regex/m, "Contact header has correct port with short option.");

$regex = '^Via: SIP\/2\.0\/UDP [^:]+:8899;';
ok ($output =~ m/$regex/m, "Via header has correct port with short option.");

# long option
system("$sipp $sippargs -sf $scenario");
$output = `$sippak --local-port=8899 sip:alice\@127.0.0.1:5060`;

# test request
$regex = '^Contact: <sip:[^:]+:8899>$';
ok ($output =~ m/$regex/m, "Contact header has correct port with long option.");

$regex = '^Via: SIP\/2\.0\/UDP [^:]+:8899;';
ok ($output =~ m/$regex/m, "Via header has correct port with short option.");

# username long param
system("$sipp $sippargs -sf $scenario");
$output = `$sippak --username=bob sip:alice\@127.0.0.1:5060`;

$regex = '^Contact: <sip:bob@[^>]+>$';
ok ($output =~ m/$regex/m, "Username in Contact header short opt.");
$regex = '^From: <sip:bob@[^>]+>;tag=';
ok ($output =~ m/$regex/m, "Username in From header short opt.");

# username short param
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u john -p 8889 sip:alice\@127.0.0.1:5060`;

$regex = '^Contact: <sip:john@[^:]+:8889>$';
ok ($output =~ m/$regex/m, "Username in Contact header long opt.");
$regex = '^From: <sip:john@[^:]+>;tag=';
ok ($output =~ m/$regex/m, "Username in From header long opt.");

done_testing();

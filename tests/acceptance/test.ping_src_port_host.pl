#
# Acceptence test:
# Run sippak ping command with custom port and username
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
$output = `$sippak -P 8899 sip:alice\@127.0.0.1:5060`;

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
$output = `$sippak -u john -P 8889 sip:alice\@127.0.0.1:5060`;

$regex = '^Contact: <sip:john@[^:]+:8889>$';
ok ($output =~ m/$regex/m, "Username in Contact header long opt.");
$regex = '^From: <sip:john@[^:]+>;tag=';
ok ($output =~ m/$regex/m, "Username in From header long opt.");

# hostname setup short param
# NOTE! On MacOS enable interface before test:
# sudo ifconfig lo0 alias 127.0.0.8 up
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u john -l 127.0.0.8 -P 9988 sip:alice\@127.0.0.1:5060`;

$regex = '^Contact: <sip:john@127.0.0.8:9988>$';
ok ($output =~ m/$regex/m, "Host in Contact header short opt.");

# hostname setup long param
# NOTE! On MacOS enable interface before test:
# sudo ifconfig lo0 alias 127.0.0.18 up
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u john --local-host 127.0.0.18 -P 9977 sip:alice\@127.0.0.1:5060`;

$regex = '^Contact: <sip:john@127.0.0.18:9977>$';
ok ($output =~ m/$regex/m, "Host in Contact header long opt.");

# from display name long option
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u 5554477544 --from-name="Gianni Schicchi" -P 9977 sip:alice\@127.0.0.1:5060`;

$regex = '^From: "Gianni Schicchi" <sip:5554477544@127.0.0.1>';
ok ($output =~ m/$regex/m, "From header display name long option.");

# from display name short option
system("$sipp $sippargs -sf $scenario");
$output = `$sippak -u 5555 -F Lauretta sip:alice\@127.0.0.1:5060`;

$regex = '^From: "Lauretta" <sip:5555@127.0.0.1>';
ok ($output =~ m/$regex/m, "From header display name short option.");

done_testing();

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
my $scenario = $ARGV[2] . "/options.408.xml";
my $sippargs = "-p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

sub sipp_quit {
  my $sipp_port = 8888;
  my $sipp_addr = inet_aton("127.0.0.1");
  my $msg = "q";

  socket(SIPPSock, PF_INET, SOCK_DGRAM, getprotobyname("udp"))
    or die("socket: $!");

  send(SIPPSock, $msg, 0,
    sockaddr_in($sipp_port, $sipp_addr)) == length($msg) or
    die("Cannot send to sipp socket.");
}

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# run sippak ping basic
$output = `$sippak sip:alice\@127.0.0.1:5060`;
sipp_quit();

$regex = 'Retransmitions count: 10';
ok ($output =~ m/$regex/m, "Retransmitions count output.");

ok ($output =~ m/408 Request Timeout/m, "Retransmitions timeout message output.");

done_testing();

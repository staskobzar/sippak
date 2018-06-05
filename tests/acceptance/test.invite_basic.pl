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
my $scenario = $ARGV[2] . "/invite.basic.xml";
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

# test request
$regex = '^INVITE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Basic INVITE packet sent.");

$regex = '^SIP\/2.0 180 Ringing$';
ok ($output =~ m/$regex/m, "Ringing 180 early response.");

$regex = '^SIP\/2.0 200 OK Invite$';
ok ($output =~ m/$regex/m, "Invite session is confirmed.");

$regex = '^ACK sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Invite 200 in acknowlaged.");

$regex = '^BYE sip:alice\@127\.0\.0\.1:5060 SIP\/2\.0$';
ok ($output =~ m/$regex/m, "Session is terminated with BYE.");

$regex = '^SIP\/2.0 200 OK bye$';
ok ($output =~ m/$regex/m, "BYE method is confirmed.");

# default codecs are ulaw and alaw
$regex = '^a=rtpmap:0 PCMU\/8000$';
ok ($output =~ m/$regex/m, "Default codec mu-law code used.");

$regex = '^a=rtpmap:8 PCMA\/8000$';
ok ($output =~ m/$regex/m, "Default codec a-law code used.");

$regex = '^m=audio \d+ RTP\/AVP 0 8$';
ok ($output =~ m/$regex/m, "Default codec order is 0, 8 (pcmu,pcma).");

# test when codec not supported
system("$sipp $sippargs -sf $scenario");
$output = `$sippak INVITE --codec=foo sip:alice\@127.0.0.1:5060`;

$regex = '^Codec "foo" is not supported.$';
ok ($output =~ m/$regex/m, "Fails when codec does not exist.");

# speex codec set
system("$sipp $sippargs -sf $scenario");
$output = `$sippak INVITE --codec=speex sip:alice\@127.0.0.1:5060`;

$regex = 'a=rtpmap:99 speex\/32000';
ok ($output =~ m/$regex/m, "Speex codec ultra-wideband is set.");

$regex = 'a=rtpmap:98 speex\/16000';
ok ($output =~ m/$regex/m, "Speex codec wideband is set.");

$regex = 'a=rtpmap:97 speex\/8000';
ok ($output =~ m/$regex/m, "Speex codec narrowband is set.");

$regex = '^a=rtpmap:0 PCMU\/8000$';
ok ($output !~ m/$regex/m, "When only speex is set, no mu-law is set.");

$regex = '^m=audio \d+ RTP\/AVP 99 98 97$';
ok ($output =~ m/$regex/m, "Default codec order is 0, 8 (pcmu,pcma).");

# TODO: test default RTP port is 4000
# TODO: test set RTP port
# TODO: test when invalid port set (foo, 0, < 1, > 2^16)
# TODO: test codec ilbc set with default mode 30
# TODO: test GSM codec set
# TODO: test g722 set
# TODO: test set all available (key word all)
# TODO: test multiple codecs order

done_testing();

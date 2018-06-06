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
my $scenario = $ARGV[2] . "/message.basic.xml";
my $sippargs = "-timeout 10s -p 5060 -m 1 -bg";
my $output = "";
my $regex  = "";

# run sippak message requires parameter --body
$output = `$sippak message sip:alice\@127.0.0.1:5060`;

$regex = 'Failed. Requires parameter "--body" to initiate MESSAGE.';
ok ($output =~ m/$regex/m, "Message method fails when no param --body set.");

# run sipp scenario in background mode
system("$sipp $sippargs -sf $scenario");
if ($? == -1) {
  print "Failed execute sipp\n";
  exit(1);
}

# refer click to dial send
$output = `$sippak message --body="Hello, there! Ca va?" sip:alice\@127.0.0.1:5060`;

$regex = '^MESSAGE sip:alice\@127\.0\.0\.1:5060 SIP\/2.0$';
ok ($output =~ m/$regex/m, "Basic MESSAGE packet sent.");

$regex = '^Content-Type: text\/plain$';
ok ($output =~ m/$regex/m, "Basic MESSAGE content type is text/plain.");

$regex = '^Content-Length: 20$';
ok ($output =~ m/$regex/m, "Basic MESSAGE has valid content-length value.");

$regex = '^Hello, there! Ca va\?$';
ok ($output =~ m/$regex/m, "Basic MESSAGE has good body text.");

$regex = '^SIP\/2\.0 200 OK Basic Message Test$';
ok ($output =~ m/$regex/m, "Basic MESSAGE has valid response.");

# long message is cut to 128 char
system("$sipp $sippargs -sf $scenario");

my $body = "Instant Messaging refers to the transfer of messages between ";
$body .= "users in near realtime. These messages are usually, but not ";
$body .= "required to be, short. IMs are often used in a conversational mode, ";
$body .= "that is, the transfer of messages back and forth is fast enough for ";
$body .= "participants to maintain an interactive conversation.";
$output = `$sippak message --body="$body" sip:alice\@127.0.0.1:5060`;

$regex = 'Message body is too long. Max allowed characters is 128';
ok ($output =~ m/$regex/m, "Basic MESSAGE with too long body warning.");

$regex = '^Instant Messaging refers to the transfer of messages between users in near realtime. These messages are usually, but not require$';
ok ($output =~ m/$regex/m, "Basic MESSAGE with too long body is trimmed.");

# Header User-Agent add
system("$sipp $sippargs -sf $scenario");
$output = `$sippak message --body="Hello World" --user-agent="sippak" sip:alice\@127.0.0.1:5060`;

$regex = '^User-Agent: sippak$';
ok ($output =~ m/$regex/m, "Add User-Agent header.");

done_testing();

#
# Acceptence test:
# Make sure usage prints all application parameters and options
#

use strict;
use warnings;
use Test::More;

my $getopts_file = $ARGV[0];
my $sippak = $ARGV[1];
my $struct_found = 0;
my @opts = ();
my $output = "";

open(GETOPTS, $getopts_file) or die("Can not open getopts.c");

while(<GETOPTS>){
  if (m/struct\s+pj_getopt_option\s+sippak_long_opts\[\]/) {
    $struct_found = 1;
    next;
  }

  if (m/\{\s*NULL\s*,(\s*0\s*,){2}\s*0\s*\}/) {
    $struct_found = 0;
  }

  if ($struct_found) {
    if (my @params = m/
        \{\s*
          "([^"]+)"\s*,         # long name
          \s*([\w\d]+)\s*,      # has arg
          \s*([\w\d]+)\s*,      # flag
          \s*'?([\w\d]+)'?\s*   # val or short name
        \},
      /x) {
      push(@opts, $params[0]);
    }
  }
}
$output = `$sippak --help`;
foreach my $opt (@opts) {
  ok ($output =~ m/\-\-$opt\b/, "Match options --$opt");
}
done_testing();

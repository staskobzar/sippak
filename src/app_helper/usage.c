/**
 * sippak -- SIP command line utility.
 * Copyright (C) 2018, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of sippak.
 *
 * sippak is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sippak is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sippak.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file usage.c
 * @brief sippak usage print.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "sippak.h"

void usage ()
{
  version ();
  puts("Usage: " PROJECT_NAME " [COMMAND] [OPTIONS] [DESTINATION]");
  puts("");
  puts("  COMMAND:");
  puts("  Default commans is \"PING\".");
  puts("    PING    Send OPTIONS packet to destination.");
  puts("");
  puts("  OPTIONS:");
  puts("    -h, --help      Print this usage message and exit.");
  puts("    -V, --version   Print version and exit.");
  puts("    -v, --verbose   Verbosity increase. Short option can be repeated multiple times.");
  puts("                    Long option can have value. Example: --verbose=6");
  puts("    -q, --quiet     Silent or quiet mode. Mute sippak.");
printf("    --ns=LIST       Define DNS nameservers to use. Comma separated list up to %d servers.\n", MAX_NS_COUNT);
  puts("                    Can be defined with ports. If ports are not defined will use default port 53.");
  puts("                    For example: --ns=8.8.8.8 or --ns=4.4.4.4:553,3.3.3.3");
  puts("");
}

void version ()
{
  puts(PROJECT_NAME " " PROJECT_VERSION " (" PROJECT_ARCH ") " LICENSE);
  puts("built with pjproject " PJSIP_VERSION ", at " PROJECT_BUILTTIME);
  puts("(c) 2018 " PROJECT_AUTHOR);
  puts("project site and bug report: " PROJECT_URL);
  puts("");
}

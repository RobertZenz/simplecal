/*
 * Copyright 2019, Robert 'Bobby' Zenz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "calendar.c"
#include "configuration.c"
#include "timeutil.c"

int main(int argc, char** argv) {
	struct configuration config = configure(argc, argv);
	
	if (config.error) {
		fprintf(stderr, "Try 'simplecal --help' for more information.\n");
		
		return EXIT_FAILURE;
	}
	
	if (config.help) {
		printf(
			"USAGE: simplecal [OPTION] [MONTH [YEAR]]\n"
			"\n"
			"  -A, --after=VALUE          The months to display after the current date.\n"
			"                             Defaults to 0.\n"
			"  -B, --before=VALUE         The months to display before the current date.\n"
			"                             Defaults to 0.\n"
			"  -h, --help                 Print this help text.\n"
			"  -r, --months-in-row=VALUE  Print VALUE number of months in a single row.\n"
			"      --row=VALUE            Defaults to 3.\n"
			"  -t, --no-highlight-today   If today should not be highlighted in the output.\n"
			"                             By default today is being highlighted by making\n"
			"                             it bold in the output.\n"
			"                             Has no effect if --today-format is provided.\n"
			"  -w, --weeks[=CALCULATION]  Display week numbers in front of each week.\n"
			"                             CALCULATION is optional and can be of:\n"
			"                               * FIRST / FIRST_DAY / ANY: The first day of\n"
			"                                 the year is the start of the first week.\n"
			"                               * FOUR / FOUR_DAYS / ISO / THURSDAY:\n"
			"                                 The first week with four days/includes\n"
			"                                 Thursday is the first week of the year.\n"
			"                               * FULL / FULL_WEEK / MONDAY: The first week\n"
			"                                 with seven days/includes Monday is the first\n"
			"                                 week of the year.\n"
			"                             Defaults to ISO.\n"
			"  -y, --year[=YEAR]          Display the whole year, either the current or\n"
			"                             the given one. This is a shortcut for:\n"
			"                               --after=11 1 YEAR\n"
			"                             Defaults to the current year.\n"
			"\n"
			"Number formats               The following options allow you to define format\n"
			"                             strings as they are being used by printf.\n"
			"                             Please consult the printf manual for available\n"
			"                             options. Note that the format only can accept\n"
			"                             a single number as parameter.\n"
			"\n"
			"  --day-format=FORMAT        Use this format for formatting days.\n"
			"                             Defaults to \"%%2i\".\n"
			"  --today-format=FORMAT      Use this format for formatting today.\n"
			"                             Defaults to \"\\033[1m%%2i\\033[0m\" unless\n"
			"                             -t/--no-highlight-today is provided, in which\n"
			"                             case it defaults to \"%%2i\".\n"
			"  --week-format=FORMAT       Use this format for formatting week-numbers.\n"
			"                             Defaults to \"%%2i\".\n"
			"\n"
			"Date formats                 The following options allow you to define format\n"
			"                             strings as they are being used by strftime.\n"
			"                             Please consult the strftime manual for available\n"
			"                             options. Note that the format accepts a single\n"
			"                             date struct as parameter.\n"
			"\n"
			"  --day-header-format= FOMAT  Use this format for formatting day names in.\n"
			"                             the header of the month.\n"
			"                             Defaults to \"%%a\".\n"
			"  --month-header-format=FORMAT Use this format for formatting the month\n"
			"                             name/header of the month.\n"
			"                             Defaults to \"%%B %%Y\".\n");
		return EXIT_SUCCESS;
	}
	
	if (config.version) {
		printf("simplecal 1.0\n");
		return EXIT_SUCCESS;
	}
	
	printCalendar(config);
	
	return EXIT_SUCCESS;
}


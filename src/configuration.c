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

#ifndef CONFIGURATION_C
#define CONFIGURATION_C

#include <getopt.h>
#include <limits.h>
#include <stdbool.h>


const int WEEK_CALCULATION_FIRST_DAY = 0;
const int WEEK_CALCULATION_FOUR_DAYS = 1;
const int WEEK_CALCULATION_ISO = 1;
const int WEEK_CALCULATION_FULL_WEEK = 2;

struct configuration {
	char* dayFormat;
	char* dayHeaderFormat;
	int displayMonth;
	int displayYear;
	bool error;
	bool help;
	bool highlightToday;
	int monthsAfter;
	int monthsBefore;
	char* monthHeaderFormat;
	int monthsInRow;
	struct tm today;
	char* todayFormat;
	bool version;
	char* weekFormat;
	bool weekNumbers;
	int weekCalculation;
};


static struct option options[] = {
	{ "after", required_argument, 0, 'A' },
	{ "before", required_argument, 0, 'B' },
	{ "day-format", required_argument, 0, 0 },
	{ "day-header-format", required_argument, 0, 0 },
	{ "help", no_argument, 0, 'h' },
	{ "highlight-today", no_argument, 0, 't' },
	{ "month-header-format", required_argument, 0, 0 },
	{ "row", required_argument, 0, 'r' },
	{ "today-format", required_argument, 0, 0 },
	{ "version", no_argument, 0, 'v' },
	{ "week-format", required_argument, 0, 0 },
	{ "weeks", optional_argument, 0, 'w' },
	{ "year", optional_argument, 0, 'y' },
	{ NULL, 0, NULL, 0 }
};


int parseWeekCalculation(char* argument) {
	if (strcmp(argument, "FIRST") == 0
			|| strcmp(argument, "FIRST_DAY") == 0
			|| strcmp(argument, "ANY") == 0) {
		return WEEK_CALCULATION_FIRST_DAY;
	} else if (strcmp(argument, "FOUR") == 0
			   || strcmp(argument, "FOUR_DAYS") == 0
			   || strcmp(argument, "ISO") == 0
			   || strcmp(argument, "THURSDAY") == 0) {
		return WEEK_CALCULATION_ISO;
	} else if (strcmp(argument, "FULL") == 0
			   || strcmp(argument, "FULL_WEEK") == 0
			   || strcmp(argument, "MONDAY") == 0) {
		return WEEK_CALCULATION_FULL_WEEK;
	} else {
		return -1;
	}
}

struct configuration configure(int argcount, char** args) {
	struct configuration config;
	config.dayFormat = "%2i";
	config.dayHeaderFormat = "%a";
	config.error = false;
	config.help = false;
	config.highlightToday = true;
	config.monthsAfter = 0;
	config.monthsBefore = 0;
	config.monthHeaderFormat = "%B %Y";
	config.monthsInRow = 3;
	config.todayFormat = "\033[1m%2i\033[0m";
	config.version = false;
	config.weekCalculation = WEEK_CALCULATION_ISO;
	config.weekFormat = "%2i";
	config.weekNumbers = false;
	
	time_t currentTime = time(NULL);
	localtime_r(&currentTime, &config.today);
	
	config.displayMonth = config.today.tm_mon + 1;
	config.displayYear = config.today.tm_year + 1900;
	
	int value = 0;
	int optionindex = 0;
	
	while((value = getopt_long(
					   argcount,
					   args,
					   "A:B:htr:vw::y::",
					   options,
					   &optionindex)) != -1) {
		if (value == 0) {
			struct option option = options[optionindex];
			
			if (strcmp(option.name, "help") == 0) {
				config.help = true;
			} else if (strcmp(option.name, "after") == 0) {
				config.monthsAfter = atoi(optarg);
			} else if (strcmp(option.name, "before") == 0) {
				config.monthsBefore = atoi(optarg);
			} else if (strcmp(option.name, "day-format") == 0) {
				config.dayFormat = optarg;
			} else if (strcmp(option.name, "day-header-format") == 0) {
				config.dayHeaderFormat = optarg;
			} else if (strcmp(option.name, "no-highlight-today") == 0) {
				config.highlightToday = false;
			} else if (strcmp(option.name, "month-header-format") == 0) {
				config.monthHeaderFormat = optarg;
			} else if (strcmp(option.name, "months-in-row") == 0
					   || strcmp(option.name, "row") == 0) {
				config.monthsInRow = atoi(optarg);
			} else if (strcmp(option.name, "today-format") == 0) {
				config.todayFormat = optarg;
			} else if (strcmp(option.name, "version") == 0) {
				config.version = true;
			} else if (strcmp(option.name, "week-format") == 0) {
				config.weekFormat = optarg;
			} else if (strcmp(option.name, "weeks") == 0) {
				config.weekNumbers = true;
				
				if (optarg) {
					config.weekCalculation = parseWeekCalculation(optarg);
					
					if (config.weekCalculation == -1) {
						config.error = true;
					}
				}
			} else if (strcmp(option.name, "year") == 0) {
				if (optarg) {
					config.displayYear = atoi(optarg);
				}
				config.displayMonth = 1;
				config.monthsAfter = 11;
			}
		} else if (value == 'A') {
			config.monthsAfter = atoi(optarg);
		} else if (value == 'B') {
			config.monthsBefore = atoi(optarg);
		} else if (value == 'h') {
			config.help = true;
		} else if (value == 'r') {
			config.monthsInRow = atoi(optarg);
		} else if (value == 't') {
			config.highlightToday = false;
		} else if (value == 'v') {
			config.version = true;
		} else if (value == 'w') {
			config.weekNumbers = true;
			
			if (optarg) {
				config.weekCalculation = parseWeekCalculation(optarg);
				
				if (config.weekCalculation == -1) {
					config.error = true;
				}
			}
		} else if (value == 'y') {
			if (optarg) {
				config.displayYear = atoi(optarg);
			}
			config.displayMonth = 1;
			config.monthsAfter = 11;
		} else if (value == '?') {
			config.error = true;
		}
	}
	
	if (args[optind] != NULL) {
		config.displayMonth = atoi(args[optind]);
		
		if (config.displayMonth < 1 || config.displayMonth > 12) {
			config.error = true;
		}
		
		optind++;
		
		if (args[optind] != NULL) {
			config.displayYear = atoi(args[optind]);
		}
	}
	
	return config;
}

#endif


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

#ifndef CALENDAR_C
#define CALENDAR_C

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "timeutil.c"
#include "configuration.c"

#define MIN(x, y) (x < y ? x : y)

/** The maximum numbers of weeks per month. */
const int MAX_WEEKS_PER_MONTH = 6;
/**
 * The width of the month sheet. 7 days two spaces wide and the whitespace
 * in between.
 */
const int MONTH_SHEET_WIDTH = (7 * 2) + (6 * 1);
/** The size of the buffer used to print the month header and days. */
const int PRINT_BUFFER_SIZE = 512;

int calculateWeekNumber(struct tm* date, struct configuration config) {
	int currentDay = date->tm_mday;
	int currentMonth = date->tm_mon;
	int currentDayOfYear = date->tm_yday;
	
	setDay(date, 1);
	setMonth(date, 1);
	
	int firstWeekday = date->tm_wday;
	if (firstWeekday == WEEKDAY_SUNDAY_SOW) {
		firstWeekday = WEEKDAY_SUNDAY_EOW;
	}
	
	setMonth(date, currentMonth + 1);
	setDay(date, currentDay);
	
	int currentWeekday = date->tm_wday;
	if (currentWeekday == WEEKDAY_SUNDAY_SOW) {
		currentWeekday = WEEKDAY_SUNDAY_EOW;
	}
	
	int weekdayDifference = firstWeekday - currentWeekday;
	int correctedCurrentDayOfYear = currentDayOfYear + weekdayDifference;
	int week = (correctedCurrentDayOfYear / 7) + 1;
	
	if (config.weekCalculation == WEEK_CALCULATION_FIRST_DAY) {
		// Nothing to change here, calculation is already correct.
	} else if (config.weekCalculation == WEEK_CALCULATION_FOUR_DAYS
			   || config.weekCalculation == WEEK_CALCULATION_ISO) {
		if (firstWeekday > WEEKDAY_THURSDAY) {
			week = week - 1;
		}
	} else if (config.weekCalculation == WEEK_CALCULATION_FULL_WEEK) {
		if (firstWeekday > WEEKDAY_MONDAY) {
			week = week - 1;
		}
	}
	
	return week;
}

void executeForEachMonth(struct tm* date, struct configuration config, int monthsCount, void (*action)(struct tm*, struct configuration, int, int), int counter) {
	for (int monthCounter = 0; monthCounter < monthsCount; monthCounter++) {
		setDay(date, 1);
		addMonths(date, monthCounter);
		
		(*action)(date, config, monthCounter, counter);
		
		if (monthCounter < (monthsCount - 1)) {
			printf("  ");
		}
		
		setDay(date, 1);
		addMonths(date, -monthCounter);
	}
	printf("\n");
}

void printDayName(struct tm* value, struct configuration config) {
	char printBuffer[PRINT_BUFFER_SIZE];
	
	strftime(printBuffer, PRINT_BUFFER_SIZE, config.dayHeaderFormat, value);
	printBuffer[2] = 0;
	
	printf("%s", printBuffer);
}

void printMonthName(struct tm* value, struct configuration config, int currentMonthCounter, int counter) {
	char printBuffer[PRINT_BUFFER_SIZE];
	
	strftime(printBuffer, PRINT_BUFFER_SIZE, config.monthHeaderFormat, value);
	int printLength = (int)strlen(printBuffer);
	int printWidth = (printLength + MONTH_SHEET_WIDTH) / 2;
	
	if (config.weekNumbers) {
		printf("   ");
	}
	
	printf("%*s", printWidth, printBuffer);
	printf("%*s", MONTH_SHEET_WIDTH - printWidth, "");
}

void printWeek(struct tm* date, struct configuration config, int currentMonthCounter, int week) {
	int currentYear = date->tm_year;
	int currentMonth = date->tm_mon;
	
	resetToFirstMonday(date);
	addDays(date, week * 7);
	
	if (config.weekNumbers) {
		if (date->tm_mon == currentMonth || week == 0) {
			int weekNumber = 0;
			
			// If this is the first week, make sure that we hand the calculation
			// function a date of the current month. After that, we revert to
			// monday again.
			if (week == 0 && date->tm_year != currentYear) {
				addDays(date, 6);
				weekNumber = calculateWeekNumber(date, config);
				addDays(date, -6);
				
				if (weekNumber == 0) {
					// If the weekNumber calculation determined that the first
					// day of this month does not qualify as valid week start in
					// the current calculation scheme, display the weekNumber
					// of Monday instead.
					weekNumber = calculateWeekNumber(date, config);
				}
			} else {
				weekNumber = calculateWeekNumber(date, config);
			}
			
			printf(config.weekFormat, weekNumber);
		} else {
			printf("  ");
		}
		
		printf(" ");
	}
	
	for (int day = 0; day < 7; day++) {
		if (date->tm_mon == currentMonth) {
			if (config.highlightToday
					&& date->tm_yday == config.today.tm_yday
					&& date->tm_year == config.today.tm_year) {
				printf(config.todayFormat, date->tm_mday);
			} else {
				printf(config.dayFormat, date->tm_mday);
			}
		} else if (date->tm_mon < currentMonth) {
			printf("  ");
		} else if (date->tm_mon > currentMonth) {
			printf("  ");
		}
		
		if (date->tm_wday != WEEKDAY_SUNDAY_SOW) {
			printf(" ");
		}
		
		addDays(date, 1);
	}
	
	// We must check for inequality instead of greater-than because we might
	// have went into the next year, which means that the current month is
	// less than the current one.
	if (date->tm_mon != currentMonth) {
		setDay(date, 1);
		addMonths(date, -1);
	}
}

void printWeekHeader(struct tm* date, struct configuration config, int currentMonthCounter, int counter) {
	resetToFirstMonday(date);
	
	if (config.weekNumbers) {
		printf("   ");
	}
	
	for (int day = 0; day < 7; day++) {
		printDayName(date, config);
		addDays(date, 1);
		
		if (day < 6) {
			printf(" ");
		}
	}
}

void printCalendar(struct configuration config) {
	time_t currentTime = time(NULL);
	struct tm* date = localtime(&currentTime);
	
	// Set the date which is being given by the configuration.
	setDay(date, 1);
	setMonth(date, config.displayMonth);
	setYear(date, config.displayYear);
	
	// Calculate the total number of months which are going to be displayed.
	int monthsToDisplay = config.monthsBefore + config.monthsAfter + 1;
	
	// Rewind to the first month which is going to be displayed.
	addMonths(date, -config.monthsBefore);
	
	// Iterate over every row which is going to be dispalyed.
	for (int month = 0; month < monthsToDisplay; month = month + config.monthsInRow) {
		// Check how many months there are left to print. The last row might not
		// contain a full set of months.
		int monthsToPrint = MIN(config.monthsInRow, monthsToDisplay - month);
		
		executeForEachMonth(date, config, monthsToPrint, &printMonthName, -1);
		executeForEachMonth(date, config, monthsToPrint, &printWeekHeader, -1);
		
		for (int week = 0; week < MAX_WEEKS_PER_MONTH; week++) {
			executeForEachMonth(date, config, monthsToPrint, &printWeek, week);
		}
		
		// Forward after the months we've just printed.
		addMonths(date, config.monthsInRow);
		
		// If we are not at the last row of months, add a separator line.
		if (month < (monthsToDisplay - config.monthsInRow)) {
			printf("\n");
		}
	}
}

#endif


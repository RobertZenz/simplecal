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

#ifndef TIMEUTIL_C
#define TIMEUTIL_C

#include <time.h>


const int WEEKDAY_SUNDAY_SOW = 0;
const int WEEKDAY_MONDAY = 1;
const int WEEKDAY_TUESDAY = 2;
const int WEEKDAY_WEDNESDAY = 3;
const int WEEKDAY_THURSDAY = 4;
const int WEEKDAY_FRIDAY = 5;
const int WEEKDAY_SATURDAY = 6;
const int WEEKDAY_SUNDAY_EOW = 7;

void addDays(struct tm* value, int days) {
	value->tm_mday = value->tm_mday + days;
	mktime(value);
}

void addMonths(struct tm* value, int months) {
	value->tm_mon = value->tm_mon + months;
	mktime(value);
}

void setDay(struct tm* value, int day) {
	value->tm_mday = day;
	mktime(value);
}

void setMonth(struct tm* value, int month) {
	value->tm_mon = month - 1;
	mktime(value);
}

void setYear(struct tm* value, int year) {
	value->tm_year = year - 1900;
	mktime(value);
}

void resetToFirstMonday(struct tm* value) {
	setDay(value, 1);
	
	if (value->tm_wday == WEEKDAY_SUNDAY_SOW) {
		setDay(value, value->tm_mday - 6);
	} else {
		setDay(value, value->tm_mday - (value->tm_wday - 1));
	}
}

#endif

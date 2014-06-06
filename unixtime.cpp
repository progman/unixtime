//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.6
// Alexey Potehin <gnuplanet@gmail.com>, http://www.gnuplanet.ru/doc/cv
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <string.h>
#include <algorithm>
#include "submodule/libcore.cpp/libcore.hpp"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// global vars
namespace global
{
	bool flag_debug  = false;
	bool flag_stdin  = false;

	bool flag_decode = false;
	bool flag_encode = false;

	int64_t tz       = 0;

	std::string format;
	std::string value;

	char line_buf[4096];
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// get val from string area
bool get_val(const char *p, const char *p_end, size_t size, int &i, bool flag_am_pm = false)
{
	std::string str;

	if ((p + size) > p_end)
	{
		return false;
	}

	str.clear();

	for (size_t i=0; i < size; i++)
	{
		str += *p;
		p++;
	}

	if (flag_am_pm == false)
	{
		if (libcore::is_udec(str) == false)
		{
			return false;
		}

		i = atoi(str.c_str());

		return true;
	}

	std::transform(str.begin(), str.end(), str.begin(), toupper);

	if (str == "AM")
	{
		i = 0;
		return true;
	}

	if (str == "PM")
	{
		i = 1;
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// encode string to unixtime
int unixtime_encode(const char *format, const char *value, time_t *time)
{
	if ((format == NULL) || (value == NULL))
	{
		*time = 0;
		return -1;
	}


	const char *format_end = format;
	while (*format_end != 0) format_end++;

	const char *value_end  = value;
	while (*value_end != 0) value_end++;


	bool flag_year  = false;
	bool flag_month = false;
	bool flag_day   = false;
	bool flag_hour  = false;
	bool flag_min   = false;
	bool flag_sec   = false;
	bool flag_am_pm = false;


	int year  = 0;
	int month = 0;
	int day   = 0;
	int hour  = 0;
	int min   = 0;
	int sec   = 0;
	int am_pm = 0;


	for (;;)
	{
		if (*format == 0) break;

		if (*format == '%')
		{
			format++;
			if (*format == 0) break;

			switch (*format)
			{
				case 'Y':
				{
					if (get_val(value, value_end, 4, year) == false)
					{
						*time = 0;
						return -1;
					}
					flag_year = true;
					value += 4;
					break;
				}
				case 'm':
				{
					if (get_val(value, value_end, 2, month) == false)
					{
						*time = 0;
						return -1;
					}
					flag_month = true;
					value += 2;
					break;
				}
				case 'd':
				{
					if (get_val(value, value_end, 2, day) == false)
					{
						*time = 0;
						return -1;
					}
					flag_day = true;
					value += 2;
					break;
				}
				case 'p':
				{
					if (get_val(value, value_end, 2, am_pm, true) == false)
					{
						*time = 0;
						return -1;
					}

					if (flag_hour != false)
					{
						if (hour > 12)
						{
							*time = 0;
							return -1;
						}
						if (am_pm != 0)
						{
							hour += 12;
						}
					}

					flag_am_pm = true;
					value += 2;
					break;
				}
				case 'I':
				{
					if (get_val(value, value_end, 2, hour) == false)
					{
						*time = 0;
						return -1;
					}

					if (hour > 12)
					{
						*time = 0;
						return -1;
					}

					if ((flag_am_pm != false) && (am_pm != 0))
					{
						hour += 12;
					}

					flag_hour = true;
					value += 2;
					break;
				}
				case 'H':
				{
					if (get_val(value, value_end, 2, hour) == false)
					{
						*time = 0;
						return -1;
					}

					if (flag_am_pm != false)
					{
						if (hour > 12)
						{
							*time = 0;
							return -1;
						}
						if (am_pm != 0)
						{
							hour += 12;
						}
					}

					flag_hour = true;
					value += 2;
					break;
				}
				case 'M':
				{
					if (get_val(value, value_end, 2, min) == false)
					{
						*time = 0;
						return -1;
					}
					flag_min = true;
					value += 2;
					break;
				}
				case 'S':
				{
					if (get_val(value, value_end, 2, sec) == false)
					{
						*time = 0;
						return -1;
					}
					flag_sec = true;
					value += 2;
					break;
				}
				case '?':
				{
					value++;
					break;
				}
				default:
				{
					if (*value != *format)
					{
						*time = 0;
						return -1;
					}
					value++;
					break;
				}
			}
		}
		else
		{
			if (*value != *format)
			{
				*time = 0;
				return -1;
			}
			value++;
		}
		format++;
	}


	if (global::flag_debug != false)
	{
		printf("year  = \"%u\"\n", year);
		printf("month = \"%u\"\n", month);
		printf("day   = \"%u\"\n", day);
		printf("hour  = \"%u\"\n", hour);
		printf("min   = \"%u\"\n", min);
		printf("sec   = \"%u\"\n", sec);
	}


	if
	(
		(flag_year  == false) ||
		(flag_month == false) ||
		(flag_day   == false) ||
		(flag_hour  == false) ||
		(flag_min   == false) ||
		(flag_sec   == false)
	)
	{
		*time = 0;
		return -1;
	}


	struct tm my_tm;
	my_tm.tm_sec  = sec;
	my_tm.tm_min  = min;
	my_tm.tm_hour = hour;
	my_tm.tm_mday = day;
	my_tm.tm_mon  = month - 1;
	my_tm.tm_year = year - 1900;
	*time = timegm(&my_tm) + (global::tz * 60 * 60);
	if (*time == -1)
	{
		*time = 0;
		return -1;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// decode unixtime to string
int unixtime_decode(const char *format, const char *value, std::string &str)
{
	time_t time = 0;
	struct tm result;


	if (value == NULL)
	{
		return -1;
	}

	if (libcore::is_udec(value) == false)
	{
		return -1;
	}


	time = atoi(value);
	time += (global::tz * 60 * 60);
	gmtime_r(&time, &result);


	std::string year;
	std::string month;
	std::string day;
	std::string hour;
	std::string min;
	std::string sec;

	if (libcore::uint2str(year,  result.tm_year + 1900) == false) return -1;
	if (libcore::uint2str(month, result.tm_mon + 1)     == false) return -1;
	if (libcore::uint2str(day,   result.tm_mday)        == false) return -1;
	if (libcore::uint2str(hour,  result.tm_hour)        == false) return -1;
	if (libcore::uint2str(min,   result.tm_min)         == false) return -1;
	if (libcore::uint2str(sec,   result.tm_sec)         == false) return -1;


	if (global::flag_debug != false)
	{
		printf("year  = \"%s\"\n", year.c_str());
		printf("month = \"%s\"\n", month.c_str());
		printf("day   = \"%s\"\n", day.c_str());
		printf("hour  = \"%s\"\n", hour.c_str());
		printf("min   = \"%s\"\n", min.c_str());
		printf("sec   = \"%s\"\n", sec.c_str());
	}


	for (;;)
	{
		if (*format == 0) break;

		if (*format == '%')
		{
			format++;
			if (*format == 0) break;

			switch (*format)
			{
				case 'Y':
				{
					str += year;
					break;
				}
				case 'm':
				{
					if (month.size() == 1) str += '0';
					str += month;
					break;
				}
				case 'd':
				{
					if (day.size() == 1) str += '0';
					str += day;
					break;
				}
				case 'p':
				{
					if (result.tm_hour > 12)
					{
						str += "PM";
						break;
					}
					str += "AM";
					break;
				}
				case 'I':
				{
					if (result.tm_hour > 12)
					{
						if (libcore::uint2str(hour, result.tm_hour - 12) == false) return -1;
					}
					if (hour.size() == 1) str += '0';
					str += hour;
					break;
				}
				case 'H':
				{
					if (hour.size() == 1) str += '0';
					str += hour;
					break;
				}
				case 'M':
				{
					if (min.size() == 1) str += '0';
					str += min;
					break;
				}
				case 'S':
				{
					if (sec.size() == 1) str += '0';
					str += sec;
					break;
				}
				default:
				{
					break;
				}
			}
		}
		else
		{
			str += *format;
		}
		format++;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// get line buf from stdin and convert and check it
int get_line_buf(char *line_buf, size_t size)
{
// read from stdin
	char *p_line = fgets(line_buf, size, stdin);
	if (p_line == NULL)
	{
		return -1;
	}

// search '\n' and change to 0
	int flag_found = 0;
	char *p_line2 = p_line;
	for (;;)
	{
		if (*p_line2 == 0) break;
		if (*p_line2 == '\n')
		{
			*p_line2 = 0;
			flag_found = 1;
			break;
		}
		p_line2++;
	}
	if (flag_found == 0)
	{
		return -1; // line incomplete
	}

//	if (*p_line == 0)
//	{
//		return -1; // line empty
//	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// view help
void help()
{
	printf("%s    %s\n", PROG_FULL_NAME, PROG_URL);
	printf("unixtime encoder/decoder from command line, or standard input\n");
	printf("\n");

	printf("example: %s --to '%%Y-%%m-%%d %%H:%%M:%%S' '2012-11-04 15:41:08'\n", PROG_NAME);
	printf("\n");
	printf("Usage: unixtime [OPTION]\n");
	printf("Options:\n");
	printf("    --to   FORMAT VALUE    encode to unixtime\n");
	printf("    --from FORMAT VALUE    decode from unixtime\n");
	printf("    --tz=NUMBER    time offset (time zone), default is 0\n");

	printf("FORMAT controls body VALUE. Interpreted sequences are:\n");
	printf("    %%%%     a literal %%\n");
	printf("    %%?     any literal\n");
	printf("    %%Y     year\n");
	printf("    %%m     month (01..12)\n");
	printf("    %%d     day of month (e.g., 01)\n");
	printf("    %%p     hour modifier (AM or PM) for %%I\n");
	printf("    %%I     hour (00..12)\n");
	printf("    %%H     hour (00..23)\n");
	printf("    %%M     minute (00..59)\n");
	printf("    %%S     second (00..60)\n");

	printf("When VALUE is - or --, read standard input.\n");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// general function
int main(int argc, char *argv[])
{
	libcore::env2bool(global::flag_debug, false, "FLAG_DEBUG");


// parse command line args
	for (int i=1; i < argc; i++)
	{
		if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-help") == 0) || (strcmp(argv[i], "-h") == 0))
		{
			help();
			return 1;
		}


		{
			std::string key = argv[i];
			std::string tmpl = "--tz=";
			if ((key.size() >= tmpl.size()) && (key.substr(0, tmpl.size()) == tmpl))
			{
				std::string value = key.substr(tmpl.size(), key.size() - 1);
				if (libcore::dec2sint(global::tz, value) == false)
				{
					fprintf(stderr, "ERROR: invalid tz value \"%s\"\n", value.c_str());
					fflush(stderr);
					return 1;
				}
				continue;
			}
		}


		if (strcmp(argv[i], "--to") == 0)
		{
			global::flag_encode = true;

			if ((i + 1) == argc)
			{
				help();
				return 1;
			}
			i += 1;
			global::format = argv[i];

			if ((i + 1) == argc)
			{
				help();
				return 1;
			}
			i += 1;
			global::value = argv[i];

			if ((i + 1) != argc)
			{
				help();
				return 1;
			}

			if
			(
				(strcmp(argv[i], "-")  == 0) ||
				(strcmp(argv[i], "--") == 0)
			)
			{
				global::flag_stdin = true;
			}
			continue;
		}


		if (strcmp(argv[i], "--from") == 0)
		{
			global::flag_decode = true;

			if ((i + 1) == argc)
			{
				help();
				return 1;
			}
			i += 1;
			global::format = argv[i];

			if ((i + 1) == argc)
			{
				help();
				return 1;
			}
			i += 1;
			global::value = argv[i];

			if ((i + 1) != argc)
			{
				help();
				return 1;
			}

			if
			(
				(strcmp(argv[i], "-")  == 0) ||
				(strcmp(argv[i], "--") == 0)
			)
			{
				global::flag_stdin = true;
			}
			continue;
		}
	}


// from
	if (global::flag_decode != false)
	{
		const char *value = global::value.c_str();
		int result = 0;
		for (;;)
		{
			if (global::flag_stdin == true)
			{
				if (get_line_buf(global::line_buf, sizeof(global::line_buf)) == -1) break;
				value = global::line_buf;
			}

			std::string str;
			int rc = unixtime_decode(global::format.c_str(), value, str);
			if (rc == -1)
			{
				result = 1;
				unixtime_decode(global::format.c_str(), "0", str);
			}
			printf("%s\n", str.c_str());

			if (global::flag_stdin == false) break;
		}
		return result;
	}


// to
	if (global::flag_encode != false)
	{
		const char *value = global::value.c_str();
		int result = 0;
		for (;;)
		{
			if (global::flag_stdin == true)
			{
				if (get_line_buf(global::line_buf, sizeof(global::line_buf)) == -1) break;
				value = global::line_buf;
			}

			time_t time;
			int rc = unixtime_encode(global::format.c_str(), value, &time);
			if (rc == -1)
			{
				result = 1;
			}
			printf("%lu\n", time);

			if (global::flag_stdin == false) break;
		}
		return result;
	}


	help();


	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

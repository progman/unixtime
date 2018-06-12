//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.7
// Alexey Potehin <gnuplanet@gmail.com>, http://www.gnuplanet.ru/doc/cv
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <string.h>
#include <errno.h>
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

	enum get_mode { GET_REGULAR, GET_AM_PM, GET_MS };
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0 -> 0, 00 -> 0, 000 -> 0, 100 -> 1, 101 -> 101
void drop_right_zero(std::string& str)
{
	if (str.size() == 0) return;

	for (;;)
	{
		if (str.size() == 1) break;

		if (str[str.size() - 1] != '0') break;

		str.resize(str.size() - 1);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// get val from string area
size_t get_val(const char *p, const char *p_end, size_t size, int &i, global::get_mode mode)
{
	std::string str;
	str.clear();


	if
	(
		(mode == global::get_mode::GET_REGULAR) ||
		(mode == global::get_mode::GET_AM_PM)
	)
	{
		if ((p + size) > p_end)
		{
			return 0;
		}

		for (size_t i=0; i < size; i++)
		{
			str += *p;
			p++;
		}
	}


	if (mode == global::get_mode::GET_MS)
	{
		if ((p + size) > p_end)
		{
			size = p_end - p;
		}

		if (size == 0)
		{
			return 0;
		}

		for (size_t i=0; i < size; i++)
		{
			str += *p;
			p++;
		}
	}


	if
	(
		(mode == global::get_mode::GET_REGULAR) ||
		(mode == global::get_mode::GET_MS)
	)
	{
		if (libcore::is_udec(str) == false)
		{
			return 0;
		}

		i = atoi(str.c_str());

		return str.size();
	}


	if (mode == global::get_mode::GET_AM_PM)
	{
		std::transform(str.begin(), str.end(), str.begin(), toupper);

		if (str == "AM")
		{
			i = 0;
			return str.size();
		}

		if (str == "PM")
		{
			i = 1;
			return str.size();
		}
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// encode string to unixtime
int unixtime_encode(const char *format, const char *value, time_t *time, int64_t *time_ms)
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
	bool flag_ms    = false;


	int year  = 0;
	int month = 0;
	int day   = 0;
	int hour  = 0;
	int min   = 0;
	int sec   = 0;
	int am_pm = 0;
	int ms    = 0;

	size_t value_size;


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
					value_size = get_val(value, value_end, 4, year, global::get_mode::GET_REGULAR);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_year = true;
					value += value_size;
					break;
				}
				case 'm':
				{
					value_size = get_val(value, value_end, 2, month, global::get_mode::GET_REGULAR);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_month = true;
					value += value_size;
					break;
				}
				case 'd':
				{
					value_size = get_val(value, value_end, 2, day, global::get_mode::GET_REGULAR);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_day = true;
					value += value_size;
					break;
				}
				case 'p':
				{
					value_size = get_val(value, value_end, 2, am_pm, global::get_mode::GET_AM_PM);
					if (value_size == 0)
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
					value += value_size;
					break;
				}
				case 'I':
				{
					value_size = get_val(value, value_end, 2, hour, global::get_mode::GET_REGULAR);
					if (value_size == 0)
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
					value += value_size;
					break;
				}
				case 'H':
				{
					value_size = get_val(value, value_end, 2, hour, global::get_mode::GET_REGULAR);
					if (value_size == 0)
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
					value += value_size;
					break;
				}
				case 'M':
				{
					value_size = get_val(value, value_end, 2, min, global::get_mode::GET_REGULAR);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_min = true;
					value += value_size;
					break;
				}
				case 'S':
				{
					value_size = get_val(value, value_end, 2, sec, global::get_mode::GET_REGULAR);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_sec = true;
					value += value_size;
					break;
				}
				case 's':
				{
					value_size = get_val(value, value_end, 6, ms, global::get_mode::GET_MS);
					if (value_size == 0)
					{
						*time = 0;
						return -1;
					}
					flag_ms = true;
					value += value_size;
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
	*time = timegm(&my_tm) - (global::tz * 60 * 60);
	if (*time == -1)
	{
		*time = 0;
		return -1;
	}


	if (flag_ms == true)
	{
		*time_ms = ((*time) * 1000000) + ms;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// decode unixtime to string
int unixtime_decode(const char *format, std::string &value, std::string &str)
{
	time_t time = 0;
	struct tm result;

	std::string year;
	std::string month;
	std::string day;
	std::string hour;
	std::string min;
	std::string sec;
	std::string ms = "0";


	if (value.size() == 0)
	{
		return -1;
	}


	if (libcore::is_udec(value) == false)
	{
		return -1;
	}


// 1528799349000000 -> 1528799349
	if (value.size() == 16)
	{
		ms = value.substr(10, 6);
		drop_right_zero(ms);

		value = value.substr(0, 10);
	}


	time = atoi(value.c_str());
	time += (global::tz * 60 * 60);
	gmtime_r(&time, &result);


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
		printf("ms    = \"%s\"\n", ms.c_str());
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
				case 's':
				{
					str += ms;
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

	printf("example: %s --to   '%%Y-%%m-%%d %%H:%%M:%%S'    '2018-06-12 13:29:09'\n", PROG_NAME);
	printf("         %s --to   '%%Y-%%m-%%d %%H:%%M:%%S.%%s' '2018-06-12 13:29:09.866429'\n", PROG_NAME);
	printf("         %s --from '%%Y-%%m-%%d %%H:%%M:%%S'    1528799349\n", PROG_NAME);
	printf("         %s --from '%%Y-%%m-%%d %%H:%%M:%%S.%%s' 1528799349866429\n", PROG_NAME);

	printf("\n");
	printf("Usage: unixtime [OPTION]\n");
	printf("Options:\n");
	printf("    --to   FORMAT VALUE    encode to unixtime/unixmicrotime\n");
	printf("    --from FORMAT VALUE    decode from unixtime/unixmicrotime\n");
	printf("    --tz=NUMBER    time zone(time offset), default is local\n");

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
	printf("    %%s     microsecond (0..999999)\n");

	printf("When VALUE is - or --, read standard input.\n");
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// general function
int main(int argc, char *argv[])
{
	libcore::env2bool(global::flag_debug, false, "FLAG_DEBUG");


// set default tz
	time_t cur_time = time(NULL);
	if (cur_time == -1)
	{
		fprintf(stderr, "ERROR[time()]: %s\n", strerror(errno));
		fflush(stderr);
		return 1;
	}

	struct tm *local_time = localtime(&cur_time);
	if (local_time == NULL)
	{
		fprintf(stderr, "ERROR[localtime()]: %s\n", strerror(errno));
		fflush(stderr);
		return 1;
	}
	global::tz = local_time->tm_gmtoff / (60 * 60);


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
		std::string value = global::value;
		std::string str;
		int rc;
		for (;;)
		{
			if (global::flag_stdin == true)
			{
				rc = get_line_buf(global::line_buf, sizeof(global::line_buf));
				if (rc == -1) break;
				value = global::line_buf;
			}

			str.clear();
			rc = unixtime_decode(global::format.c_str(), value, str);
			if (rc == -1)
			{
				value  = "0";
				rc = unixtime_decode(global::format.c_str(), value, str);
				if (rc == -1)
				{
					return 1;
				}
			}
			printf("%s\n", str.c_str());

			if (global::flag_stdin == false) break;
		}
		return 0;
	}


// to
	if (global::flag_encode != false)
	{
		const char *value = global::value.c_str();
		int rc;
		time_t time;
		int64_t time_ms;
		for (;;)
		{
			if (global::flag_stdin == true)
			{
				rc = get_line_buf(global::line_buf, sizeof(global::line_buf));
				if (rc == -1) break;
				value = global::line_buf;
			}

			time_ms = -1;
			int rc = unixtime_encode(global::format.c_str(), value, &time, &time_ms);
			if (rc == -1)
			{
				return 1;
			}

			if (time_ms == -1)
			{
				printf("%lu\n", time);
			}
			else
			{
				printf("%ld\n", time_ms);
			}

			if (global::flag_stdin == false) break;
		}
		return 0;
	}


	help();


	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

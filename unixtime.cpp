//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.2
// Alexey Potehin http://www.gnuplanet.ru/doc/cv
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>
#include <algorithm>
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// global vars
namespace global
{
	bool flag_debug = false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// test is uint ?
bool is_uint(const std::string& str)
{
	for (size_t i=0; i < str.size(); i++)
	{
		if
		(
			(str[i] < '0') ||
			(str[i] > '9')
		)
		{
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// convert int to string
std::string sint2str(int value)
{
	char buf[128];
	sprintf(buf, "%d", value);

	return std::string(buf);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// convert string to bool
bool str2bool(const std::string& str)
{
	std::string tmp = str;
	std::transform(str.begin(), str.end(), tmp.begin(), tolower);

	if
	(
		(tmp == "true") ||
		(tmp == "on") ||
		(tmp == "1")
	)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// get env var and convert to bool
bool env2bool(const char* name, bool value_default = false)
{
	char* p = getenv(name);
	if (p == NULL)
	{
		return value_default;
	}

	return str2bool(p);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// get val from string area
bool get_val(const char* p, const char* p_end, size_t size, int& i)
{
	std::string str;

	if ((p + size) > p_end)
	{
		return false;
	}

	str.clear();

	for(size_t i=0; i < size; i++)
	{
		str += *p;
		p++;
	}
	if (is_uint(str) == false)
	{
		return false;
	}

	i = atoi(str.c_str());

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// encode string to unixtime
int unixtime_encode(const char* format, const char* data, time_t* time)
{
	if ((format == NULL) || (data == NULL)) return -1;


	const char* format_end = format;
	while (*format_end != 0) format_end++;

	const char* data_end   = data;
	while (*data_end != 0) data_end++;


	bool flag_year  = false;
	bool flag_month = false;
	bool flag_day   = false;
	bool flag_hour  = false;
	bool flag_min   = false;
	bool flag_sec   = false;


	int year  = 0;
	int month = 0;
	int day   = 0;
	int hour  = 0;
	int min   = 0;
	int sec   = 0;


	for(;;)
	{
		if (*format == 0) break;

		if (*format == '%')
		{
			format++;
			if (*format == 0) break;

			switch(*format)
			{
				case 'Y':
				{
					if (get_val(data, data_end, 4, year) == false)
					{
						return -1;
					}
					flag_year = true;
					data += 4;
					break;
				}
				case 'm':
				{
					if (get_val(data, data_end, 2, month) == false)
					{
						return -1;
					}
					flag_month = true;
					data += 2;
					break;
				}
				case 'd':
				{
					if (get_val(data, data_end, 2, day) == false)
					{
						return -1;
					}
					flag_day = true;
					data += 2;
					break;
				}
				case 'H':
				{
					if (get_val(data, data_end, 2, hour) == false)
					{
						return -1;
					}
					flag_hour = true;
					data += 2;
					break;
				}
				case 'M':
				{
					if (get_val(data, data_end, 2, min) == false)
					{
						return -1;
					}
					flag_min = true;
					data += 2;
					break;
				}
				case 'S':
				{
					if (get_val(data, data_end, 2, sec) == false)
					{
						return -1;
					}
					flag_sec = true;
					data += 2;
					break;
				}
				default:
				{
					data++;
					break;
				}
			}
		}
		else
		{
			data++;
		}
		format++;
	}


	if (global::flag_debug != false)
	{
		printf ("year  = \"%u\"\n", year);
		printf ("month = \"%u\"\n", month);
		printf ("day   = \"%u\"\n", day);
		printf ("hour  = \"%u\"\n", hour);
		printf ("min   = \"%u\"\n", min);
		printf ("sec   = \"%u\"\n", sec);
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
		return -1;
	}


	struct tm my_tm;
	my_tm.tm_sec  = sec;
	my_tm.tm_min  = min;
	my_tm.tm_hour = hour;
	my_tm.tm_mday = day;
	my_tm.tm_mon  = month - 1;
	my_tm.tm_year = year - 1900;
	*time = timegm(&my_tm);
	if (*time == -1)
	{
		return -1;
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// decode unixtime to string
int unixtime_decode(const char* format, const char* data, std::string& str)
{
	time_t time = 0;
	struct tm result;


	if (data != NULL)
	{
		if (is_uint(data) != false)
		{
			time = atoi(data);
		}
	}
	gmtime_r(&time, &result);


	std::string year  = sint2str(result.tm_year + 1900);
	std::string month = sint2str(result.tm_mon + 1);
	std::string day   = sint2str(result.tm_mday);
	std::string hour  = sint2str(result.tm_hour);
	std::string min   = sint2str(result.tm_min);
	std::string sec   = sint2str(result.tm_sec);


	if (global::flag_debug != false)
	{
		printf ("year  = \"%s\"\n", year.c_str());
		printf ("month = \"%s\"\n", month.c_str());
		printf ("day   = \"%s\"\n", day.c_str());
		printf ("hour  = \"%s\"\n", hour.c_str());
		printf ("min   = \"%s\"\n", min.c_str());
		printf ("sec   = \"%s\"\n", sec.c_str());
	}


	for(;;)
	{
		if (*format == 0) break;

		if (*format == '%')
		{
			format++;
			if (*format == 0) break;

			switch(*format)
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
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// view help
void help(const char* filename)
{
	printf ("unixtime version 0.0.2\n");
	printf ("example: %s --to|--from '%%Y-%%m-%%d %%H:%%M:%%S' '2012-11-04 15:41:08'\n", filename);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//
// general function
int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		help(argv[0]);
		exit(1);
	}


	if
	(
		(strcmp(argv[1], "--to")   != 0) &&
		(strcmp(argv[1], "--from") != 0)
	)
	{
		help(argv[0]);
		exit(1);
	}


	global::flag_debug = env2bool("FLAG_DEBUG");


	if (strcmp(argv[1], "--to") == 0)
	{
		time_t time;
		int rc = unixtime_encode(argv[2], argv[3], &time);
		if (rc == -1)
		{
			printf ("ERROR: invalid input\n");
			return 1;
		}
		printf ("%lu\n", time);
	}


	if (strcmp(argv[1], "--from") == 0)
	{
		std::string str;
		int rc = unixtime_decode(argv[2], argv[3], str);
		if (rc == -1)
		{
			printf ("ERROR: invalid input\n");
			return 1;
		}
		printf ("%s\n", str.c_str());
	}


	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

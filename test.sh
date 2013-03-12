#!/bin/bash
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# 0.0.3
# Alexey Potehin <gnuplanet@gmail.com>, http://www.gnuplanet.ru/doc/cv
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test1
function test1()
{
	A='04.11.2012 15:41:08';
	B=$(./bin/unixtime --to '%d.%m.%Y %H:%M:%S' "${A}");
	C=$(./bin/unixtime --from '%d.%m.%Y %H:%M:%S' "${B}");

	if [ "${A}" != "${C}" ];
	then
		echo "${A}";
		echo "${B}";
		echo "${C}";
		echo "ERROR[test1]: result different";
		exit 1;
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test2
function test2()
{
	X='2012.11.04-15.41.08';
	Y=$(./bin/unixtime --to '%Y.%m.%d-%H.%M.%S' "${X}");
	Z=$(./bin/unixtime --from '%Y.%m.%d-%H.%M.%S' "${Y}");

	if [ "${X}" != "${Z}" ];
	then
		echo "${X}";
		echo "${Y}";
		echo "${Z}";
		echo "ERROR[test2]: result different";
		exit 1;
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test3
function test3()
{
	TMPFILE1="$(mktemp)";
	TMPFILE2="$(mktemp)";
	TMPFILE3="$(mktemp)";

	echo '04.11.2012 15:41:08' >> "${TMPFILE1}";
	echo '04.11.2012 15:41:09' >> "${TMPFILE1}";
	echo '04.11.2012 15:41:10' >> "${TMPFILE1}";

	cat "${TMPFILE1}" | ./bin/unixtime --to   '%d.%m.%Y %H:%M:%S' -- &> "${TMPFILE2}";
	cat "${TMPFILE2}" | ./bin/unixtime --from '%d.%m.%Y %H:%M:%S' -- &> "${TMPFILE3}";

	HASH1=$(md5sum "${TMPFILE1}" | awk '{print $1}');
	HASH2=$(md5sum "${TMPFILE3}" | awk '{print $1}');

	if [ "${HASH1}" != "${HASH2}" ];
	then
		echo "TMPFILE1:${TMPFILE1}";
		echo "TMPFILE2:${TMPFILE2}";
		echo "TMPFILE3:${TMPFILE3}";
		echo "ERROR[test3]: result different";
		exit 1;
	fi

	rm -rf "${TMPFILE1}" &> /dev/null;
	rm -rf "${TMPFILE2}" &> /dev/null;
	rm -rf "${TMPFILE3}" &> /dev/null;
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test4
function test4()
{
	X=$(echo -e "\n04.11.2012 15:41:09" | ./bin/unixtime --to '%d.%m.%Y %H:%M:%S' -- | ./bin/unixtime --from '%d.%m.%Y %H:%M:%S' -- | tail -n 1);

	if [ "${X}" != "04.11.2012 15:41:09" ];
	then
		echo "${X}";
		echo "ERROR[test4]: result different";
		exit 1;
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test5
function test5()
{
	X=$(echo -e "04.11.2012 15:41:09"    | ./bin/unixtime --to '%d.%m.%Y %H:%M:%S' --);
	Y=$(echo -e "04.11.2012 03:41:09 PM" | ./bin/unixtime --to '%d.%m.%Y %I:%M:%S %p' --);

	if [ "${X}" != "${Y}" ];
	then
		echo "${X}";
		echo "${Y}";
		echo "ERROR[test5.1]: result different";
		exit 1;
	fi


	X=$(echo -e "04.11.2012 04:41:09"    | ./bin/unixtime --to '%d.%m.%Y %H:%M:%S' --);
	Y=$(echo -e "04.11.2012 04:41:09 am" | ./bin/unixtime --to '%d.%m.%Y %I:%M:%S %p' --);

	if [ "${X}" != "${Y}" ];
	then
		echo "${X}";
		echo "${Y}";
		echo "ERROR[test5.2]: result different";
		exit 1;
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test6
function test6()
{
	X="04.11.2012 14:41:09";

	Y=$(echo -e "${X}" | ./bin/unixtime --to '%d.%m.%Y %H:%M:%S' -- | ./bin/unixtime --from '%d.%m.%Y %I:%M:%S %p' --);

	Z=$(echo -e "${Y}" | ./bin/unixtime --to '%d.%m.%Y %I:%M:%S %p' -- | ./bin/unixtime --from '%d.%m.%Y %H:%M:%S' --);


	if [ "${X}" != "${Z}" ];
	then
		echo "${X}";
		echo "${Y}";
		echo "${Z}";
		echo "ERROR[test6]: result different";
		exit 1;
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# check depends
function check_prog()
{
	for i in ${CHECK_PROG_LIST};
	do
		if [ "$(which ${i})" == "" ];
		then
			echo "ERROR: you must install \"${i}\"...";
			echo;
			echo;
			exit 1;
		fi
	done
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
if [ ! -e ./bin/unixtime ];
then
	echo "ERROR: make it";
	exit 1;
fi


CHECK_PROG_LIST='awk cat echo md5sum mktemp rm tail';
check_prog;


test1;
test2;
test3;
test4;
test5;
test6;


echo "ok, test passed";
exit 0;
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#

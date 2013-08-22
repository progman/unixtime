#!/bin/bash
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# 0.0.3
# Alexey Potehin <gnuplanet@gmail.com>, http://www.gnuplanet.ru/doc/cv
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
APP='./bin/unixtime';
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# run app
function run_app()
{
	STDIN=$(cat);


	if [ "${FLAG_VALGRIND}" != "1" ];
	then
		if [ "${STDIN}" != "" ];
		then
			STDOUT=$(echo "${STDIN}" | ${APP} "${@}");
		else
			STDOUT=$(${APP} "${@}");
		fi
	else
		VAL="valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-reachable=yes --log-file=valgrind.log";

		STDOUT=$(echo "${STDIN}" | ${VAL} ${APP} "${@}");

		echo '--------------------------' >> valgrind.all.log;
		cat valgrind.log >> valgrind.all.log;
		rm -rf valgrind.log;
	fi


	if [ "${STDOUT}" != "" ];
	then
		echo "${STDOUT}";
	fi
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test1
function test1()
{
	A='04.11.2012 15:41:08';
	B=$(run_app --to '%d.%m.%Y %H:%M:%S' "${A}" < /dev/null);
	C=$(run_app --from '%d.%m.%Y %H:%M:%S' "${B}" < /dev/null);

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
	Y=$(run_app --to '%Y.%m.%d-%H.%M.%S' "${X}" < /dev/null);
	Z=$(run_app --from '%Y.%m.%d-%H.%M.%S' "${Y}" < /dev/null);

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

	cat "${TMPFILE1}" | run_app --to   '%d.%m.%Y %H:%M:%S' -- &> "${TMPFILE2}";
	cat "${TMPFILE2}" | run_app --from '%d.%m.%Y %H:%M:%S' -- &> "${TMPFILE3}";

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
	X=$(echo -e "\n04.11.2012 15:41:09" | run_app --to '%d.%m.%Y %H:%M:%S' -- | run_app --from '%d.%m.%Y %H:%M:%S' -- | tail -n 1);

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
	X=$(echo -e "04.11.2012 15:41:09"    | run_app --to '%d.%m.%Y %H:%M:%S' -- );
	Y=$(echo -e "04.11.2012 03:41:09 PM" | run_app --to '%d.%m.%Y %I:%M:%S %p' -- );

	if [ "${X}" != "${Y}" ];
	then
		echo "${X}";
		echo "${Y}";
		echo "ERROR[test5.1]: result different";
		exit 1;
	fi


	X=$(echo -e "04.11.2012 04:41:09"    | run_app --to '%d.%m.%Y %H:%M:%S' -- );
	Y=$(echo -e "04.11.2012 04:41:09 am" | run_app --to '%d.%m.%Y %I:%M:%S %p' -- );

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

	Y=$(echo -e "${X}" | run_app --to '%d.%m.%Y %H:%M:%S' -- | run_app --from '%d.%m.%Y %I:%M:%S %p' -- );

	Z=$(echo -e "${Y}" | run_app --to '%d.%m.%Y %I:%M:%S %p' -- | run_app --from '%d.%m.%Y %H:%M:%S' -- );


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
	local FLAG_OK=1;
	for i in ${CHECK_PROG_LIST};
	do
		if [ "$(which ${i})" == "" ];
		then
			echo "FATAL: you must install \"${i}\"...";
			echo;
			echo;
			FLAG_OK=0;
			break;
		fi
	done

	return ${FLAG_OK};
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
if [ ! -e "${APP}" ];
then
	echo "ERROR: make it";
	exit 1;
fi


check_prog "awk cat echo md5sum mktemp rm tail";
if [ "${?}" == "0" ];
then
	exit 1;
fi


test1;
test2;
test3;
test4;
test5;
test6;


echo "ok, test passed";
exit 0;
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#

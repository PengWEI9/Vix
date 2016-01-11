#!/bin/bash

PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin
export PATH
TERMINAL_TYPE=`getini -f /afc/etc/commissioned.ini General:TerminalType`
if ! echo "$TERMINAL_TYPE" |grep FPD; then
	echo "try this on FPD terminals ONLY"
	exit 0
fi
{


ipv4=`ifconfig eth0|grep 'inet addr:'|sed 's/.*inet addr:\([0-9.]\+\) .*/\1/'`
echo "checking cobra $ipv4"
full=`df -h|grep ^/dev/root|sed -e 's#/dev/root.* \([0-9]\{1,3\}\)%.*#\1#'`
if [ $full -ge 85 ]; then
	str=WARN
	if [ $full -ge 95 ]; then
		str=ERROR
	fi
	echo "$str : $ipv4 is $full % full"
fi

alguid_downloaded=`echo /afc/data/cdimport/AL*.gz`
if ! test -f $alguid_downloaded ; then
	echo "ERROR : $ipv4 has no AL file $alguid_downloaded in /afc/data/cdimport !"
else
	countstale=`find /afc/data/cdimport -name AL\*gz -mmin +240|wc -l`
	if [ $countstale -gt 0 ]; then
		echo "ERROR : $ipv4 has AL file older that 240 minutes"
	fi
	alguid_downloaded=`basename $alguid_downloaded|sed 's/AL.*\.\(.*\).db.gz/\1/'`
	alguid_inapp=`echo /afc/data/config/guid_*`
	if ! test -f $alguid_inapp; then
		echo "ERROR : $ipv4 has no guid_* file under /afc/data/config !"
	else
		alguid_inapp=`basename $alguid_inapp|sed 's/guid_//'`
		if [ "x$alguid_inapp" != "x$alguid_downloaded" ]; then
			echo "ERROR : $ipv4 downloaded guild $alguid_downloaded differs from in app guid $alguid_inapp !"
		else
			echo "AL CHECK : $ipv4 is OK"
		fi
	fi
	
fi


tariffid_downloaded=`echo /afc/data/cdimport/CD*.gz`
if ! test -f $tariffid_downloaded ; then
	echo "ERROR : $ipv4 has no CD file in /afc/data/cdimport !"
else
	tariffid_downloaded=`basename $tariffid_downloaded|sed 's/\.db.gz//'|sed 's/CD[^.]*[.]//'`
	tariffid_inapp=`echo /afc/data/config/original_*`
	if ! test -f $tariffid_inapp; then
		echo "ERROR : $ipv4 has no guid_* file under /afc/data/config !"
	else
		tariffid_inapp=`basename $tariffid_inapp|sed 's/original_//'`
		if [ "x$tariffid_inapp" != "x$tariffid_downloaded" ]; then
			echo "ERROR : $ipv4 downloaded tariff  $tariffid_downloaded differs from in app tariff  $tariffid_inapp !"
		else
			echo "CD CHECK : $ipv4 is OK"
		fi
	fi
	
fi

pmcli init>/tmp/pmcli.$$
pmcli_has_at_least_one_proc=0
error=0
while read line
do
	if echo $line | grep -q '<process>'; then
		read name
		procname=`echo $name|sed 's#<name>\(.*\)</name>#\1#'`
		read # discard
		read runlevel
		rl=`echo $runlevel|sed 's#<runLevel>\(.*\)</runLevel>#\1#'`
		if ! echo $rl |egrep -q "^[0-9]{1,2}$"; then
			echo "ERROR : process $procname on $ipv4 has BAD run level $rl!"
			error=$((error+1))
		fi
		pmcli_has_at_least_one_proc=1
	fi
done < /tmp/pmcli.$$
rm -f /tmp/pmcli.$$
if [ $pmcli_has_at_least_one_proc -eq 0 ]; then
	echo "ERROR : $ipv4  pmcli produced no process list, pm may be down"
elif [ $error -eq 0 ]; then
	echo "processes on $ipv4 OK"
fi

countolder1=`find /afc/data/switch/import/ldt -type f -mmin +240|wc -l`
countolder2=`find /afc/data/switch/import/ldt -type f -mmin +240|wc -l`
if [ $countolder1 -gt 0 -o $countolder2 -gt 0 ]; then
	echo "ERROR : $ipv4 has files not sent for over 240 minutes"
else
	echo "$ipv4 Switch UD/Alarm check OK"
fi

} |egrep 'ERROR|WARN|OK'

#!/bin/bash




PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin
export PATH
TERMINAL_TYPE=`getini -f /afc/etc/commissioned.ini General:TerminalType`
if [ "$TERMINAL_TYPE" != "EGK" ]; then
	echo "try this on EGK terminals ONLY"
	exit 0
fi

ipv4=`ifconfig p1p1|grep 'inet addr:'|sed 's/.*inet addr:\([0-9.]\+\) .*/\1/'`
echo "checking EMM $ipv4"

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
	echo "ERROR : $ip4 pmcli produced no process list, pm may be down"
elif [ $error -eq 0 ]; then
	echo "processes on $ipv4 OK"
fi


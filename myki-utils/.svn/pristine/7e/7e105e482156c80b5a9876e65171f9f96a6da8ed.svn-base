#!/bin/bash

AFC=/afc
AFC_BIN=$AFC/bin
AFC_ETC=$AFC/etc
AFC_DATA=$AFC/data
AFC_LDT=$AFC_DATA/switch/import/ldt
AFC_ALARM=$AFC_DATA/switch/import/alarm
PATH=$PATH:/bin:/usr/bin:/sbin:$AFC_BIN

cleanup()
{
	rm -f $COMMISSIONED_FILE
}
trap cleanup TERM HUP EXIT INT


COMMISSIONED_FILE=$AFC_ETC/commissioned.ini
COMMISSIONED_FILE_DECOM=$AFC_ETC/commissioned.ini-decom
if [ ! -f $COMMISSIONED_FILE ]; then
	if [ ! -f $COMMISSIONED_FILE_DECOM ]; then
		echo "ERROR : No commissioned.ini or commissioned.ini-decom file found in /afc/etc, ABORTING"
		exit 1
	else
	# create a copy with right name for thr duration of this script
	cp -p $COMMISSIONED_FILE_DECOM $COMMISSIONED_FILE
	fi
fi
TERMINAL_TYPE=`$AFC_BIN/getini -f $COMMISSIONED_FILE General:TerminalType`
TERMINAL_ID=`$AFC_BIN/getini -f $COMMISSIONED_FILE General:TerminalId`


TurnOnNetwork()
{
	echo "Attempting to turn on networking"
	echo >> /etc/network/interfaces <<EOF
auto lo
iface lo inet loopback

# Configure ethernet
auto eth0
iface eth0 inet dhcp
EOF
	service network restart
	IP_ADDRESS=`ifconfig -a|grep  'inet addr:'|grep -v 127.0.0.1|sed -e 's/.*inet addr:\([0-9.][0-9.]*\).*Bcast.*/\1/'`
	if echo $IP_ADDRESS | egrep '[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*'; then
		echo "Obtained IP Address $IP_ADDRESS"
	else
		echo "ERROR : COuld not obtain an IP address"
	fi
}

FailUsb()
{
	TurnOnNetwork
	echo "ERROR : failed to detect USB device, please wait ..."
	if [ ! -z $IP_ADDRESS ]; then
		echo "Please connect to $IP_ADDRESS using scp client to retrieve $FILE_LIST from  /afc/var/tmp/recdata"
		exit 2
	else
		echo "ERROR : Failed to activate network, please ensure connected to network and re-run, recovered files $FILE_LIST are in /afc/var/tmp/recdata"
		exit 3
	fi
}

clear
echo "Data Recovery script FOR USE IN MAINTENANCE LAB, NOT to be run on a device online in production"
echo "Please PRESS Y and push ENTER to proceed, any other key and ENTER to stop >"
read key

if [ $key != "Y" ]; then
	echo "ABORTED on user input"
	exit 0
fi

case "x$TERMINAL_TYPE" in
	xFPD*)
		COMMISIONING_CMD=$AFC_BIN/commission-cobra.sh
	;;
	xGAC*)
		if [ -f $AFC_BIN/vgac ]; then
			COMMISIONING_CMD=$AFC_BIN/commission-gac.sh
		else
			COMMISIONING_CMD=$AFC_BIN/commission-cobra.sh
		fi
	;;
	xEGK*)
		COMMISIONING_CMD=$AFC_BIN/commission-emm.sh
	;;
	*)
		echo "ERROR : Unknown terminal type : $TERMINAL_TYPE"
		exit 4
esac
# Do the UD batching, as it is done by the decomm scripts only for cobra at the mpment (20141103)
# Doing it multiple times is harmless
# need commissioned.ini in place for this to work
$AFC_BIN/transfer_ud.sh

# has decommissioning been done
if  [ ! -f $COMMISSIONED_FILE ]; then
	echo "Please wait, decommissiomning the device"
	/etc/init.d/S80process-manager stop
	# the ud batches now end up in /afc/data/switch/import (ldt or alarm)
	$COMMISIONING_CMD --decommission
	echo "Decommission exit status is $?"
fi


rm -rf /afc/var/tmp/recdata
mkdir -p /afc/var/tmp/recdata/ldt
mkdir -p /afc/var/tmp/recdata/alarm
mv -f $AFC_LDT/*.gz /afc/var/tmp/recdata/ldt 2>/dev/null
mv -f $AFC_ALARM/*.gz /afc/var/tmp/recdata/alarm 2>/dev/null
LDT_COUNT=`ls /afc/var/tmp/recdata/ldt/*.gz 2>/dev/null|wc -l`
ALARM_COUNT=`ls /afc/var/tmp/recdata/alarm/*.gz 2>/dev/null|wc -l`
RECOVERED_FNAME="${TERMINAL_ID}_`date +%Y%m%d`"
if [ $LDT_COUNT -gt 0 ]; then
	echo "Unsent LDT files found, archving ..."
	ORIG_PWD=`pwd`
	cd /afc/var/tmp/recdata
	tar cf - ldt |gzip > LDT_${RECOVERED_FNAME}.tar.gz
	if [ $? -ne 0 ]; then
		echo "ERROR creating archive LDT_${RECOVERED_FNAME}.tar.gz"
		rm -f /afc/var/tmp/recdata/*.tar.gz
		exit 5
	fi
	rm -rf ldt
	cd $ORIG_PWD
	FILE_LIST="LDT_${RECOVERED_FNAME}.tar.gz"
fi
if [ $ALARM_COUNT -gt 0 ]; then
	echo "Unsent EVENT files found, archving ..."
	ORIG_PWD=`pwd`
	cd /afc/var/tmp/recdata
	tar cf - alarm |gzip > ALARM_${RECOVERED_FNAME}.tar.gz
	if [ $? -ne 0 ]; then
		echo "ERROR creating archive ALARM_${RECOVERED_FNAME}.tar.gz"
		rm -f /afc/var/tmp/recdata/*.tar.gz
		exit 5
	fi
	rm -rf alarm
	cd $ORIG_PWD
	FILE_LIST="$FILE_LIST ALARM_${RECOVERED_FNAME}.tar.gz"
fi

echo "Please connect the CFC to the device USB port, and hit ENTER"
read blah
echo "Attempting to detect USB device ....., this could take a minute"
sleep 60

if ! mount|grep usbfs; then
	FailUsb
fi
devfile=`fdisk -l|egrep '^/dev/sdb. '|sed -e 's#\(/dev/sdb[0-9]\) .*#\1#'`
if [ -z $devfile ]; then
	FailUsb
fi
if !mount -t vfat $devfile /mnt; then
	FailUsb
fi

echo "Copying files $FILE_LIST to the USB device ...."
cp -fp $FILE_LIST /mnt


 

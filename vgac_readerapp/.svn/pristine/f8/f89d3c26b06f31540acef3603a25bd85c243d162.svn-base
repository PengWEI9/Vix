#!/bin/sh
#
#   Usage           :   ntpd_cb.sh <ntp-event>
#
#   Parameters      :   <ntp-event>
#                           ntpd event, can either be
#                           "period"    every ~11 minutes
#                           "step"      significant difference between local
#                                       and network time
#                           "stratum"   synchronised with better stratum
#
#   Environments    :   $freq_drift_ppm ?
#                       $offset         local/network time difference
#                       $poll_interval  ?
#                       $stratum        synchronised stratum (1-16)
#
#   Description     :   This script is called by ntpd and is used to determine
#                       if system time is in synchronised with a NTP server.
#                       The last synchronised UTC and $offset are saved in
#                       /afc/data/ntpd_data file.
#
#                       It has been observed and, hence assumed, that while
#                       the device is receiving time synchronisation response
#                       from NTP server, the $offset environment value is
#                       different for each call to this script.
#
#   Subversion      :
#       $Id: $
#       $HeadURL: $
#
file="/afc/data/ntpd_data"
log="/afc/log/ntpd_cb"
VERBOSE=0

AFC_HOME=/afc
ONE_HOUR_IN_SECONDS=3600
ALARM_UTIL=${AFC_HOME}/bin/alarm_command

PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin
export PATH

if [ $# -ne 1 ]
then
    if [ $VERBOSE = 1 ]
    then
        echo "`date` $0: Missing event" >>$log
    fi
    exit 2
fi

if [ $VERBOSE = 1 ]
then
    echo "ntpd_cb: date/time      = `date` (`date +%s`)" >>$log
    echo "       : event          = $1" >>$log
    echo "       : stratum        = $stratum" >>$log
    echo "       : freq_drift_ppm = $freq_drift_ppm" >>$log
    echo "       : poll_interval  = $poll_interval" >>$log
    echo "       : offset         = $offset" >>$log
fi

if [ "$1" = "stratum" ] || [ "$1" = "periodic" ] ; then 
    if [ "$stratum" = "" ] || [ "$stratum" = "16" ] ; then 
        if [ $VERBOSE = 1 ]
        then 
            echo "TimeSyncUpdate : NTP not connected" >> $log
        fi
    else
        if [ "$offset" = "" ] || [ "$offset" = "0" ] ; then 
            if [ $VERBOSE = 1 ]
            then 
                echo "TimeSyncUpdate : NTP losing connection" >> $log
            fi  
        else
            echo "`date +%s` $offset" > $file
        fi
    fi
elif [ "$1" = "step" ] ; then
    if [ "$stratum" = "" ] ; then
        if [ $VERBOSE = 1 ]
        then
            echo "TimeSyncUpdate : NTP not connected" >> $log
        fi
    else
        if [ "$offset" = "" ] ; then
            if [ $VERBOSE = 1 ]
            then
                echo "TimeSyncUpdate : NTP losing connection" >> $log
            fi
        else
            if [ -x ${ALARM_UTIL} ] ; then
                timediff=`echo ${offset} | cut -d '.' -f1`
                if [ ${timediff} -ge ${ONE_HOUR_IN_SECONDS} -o ${timediff} -le -${ONE_HOUR_IN_SECONDS} ] ; then
                    ${ALARM_UTIL} -i /afc/etc/alarm.ini -t DeviceManagementState -a 220 -s warning --problem-description 'Time has been adjusted by more than 1 hour'
                    ${ALARM_UTIL} -i /afc/etc/alarm.ini -t DeviceManagementState -a 220 -s cleared --problem-description 'Time has been adjusted by more than 1 hour'
                fi
            fi
            echo "`date +%s` $offset" > $file
        fi
    fi
fi


exit 0

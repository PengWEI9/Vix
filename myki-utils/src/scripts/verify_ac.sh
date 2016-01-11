#!/bin/bash
#################################################################################
#
# Vix Technology Licensed software
# (C) 2015 All rights reserved
#
#==============================================================================
#
# Project/Product : MVU
# Filename : verify_ac.sh
# Author(s) : Edward Hutchison
#
# Description :
# Shell script to detect changes in active cradle when compared to commissioning.ini
#
# Sample of active cradle content:
#
# POLE_NUMBER=2
# VEHICLE_ID=7984
# TERMINAL_ID=90317539
# SERVICE_PROVIDER_ID=1001
# TRANSPORT_MODE=TRAM
# NTP_SERVER=sitprod1arc.prod1.local
# COMM_SERVER=testprod1arc.prod1.local
# ACCESS_POINT_NAME=nts.mymyki.com.au
# LOCAL_IP_ADDRESS_SUBNET=0 (*)
# AUTOGEN_CRC=0X2CDE
#
# (*) used in development environment to connect different sets
# of devices on the same network.
#
# Subversion :
# $Id: $
# $HeadURL: $
#
# History :
# Vers. Date Aut. Type Description
# ----- ---------- ---- ------- ----------------------------------------
# 1.00  13.10.15 EAH Create
#
#################################################################################
AFC_HOME=/afc
AC_UTIL=${AFC_HOME}/bin/ac
ALARM_UTIL=${AFC_HOME}/bin/alarm_command
ETC_DIR=${AFC_HOME}/etc
INIFILE=${ETC_DIR}/commissioned.ini

PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin
export PATH

TERMINAL_ID=0
SERVICE_PROVIDER_ID=0

# maximum number to retry retrieving the cradle serial number
CRADLE_SERIAL_NUMBER_RETRIES=2

GetIni()
{
    INIENTRY=`getini -f "$1" "$2"`
    if [ "${INIENTRY}" = "" ] ; then
        if [ "$3" = "" ] ; then
            LogError "No entry in $1 for $2"
            exit 1
        fi
        INIENTRY=$3
    fi

}   #   GetIni

# Retrieves the cradle serial number from the output of /afc/bin/ac and writes
# it to the file /afc/etc/cradle_serial_number If the serial number couldn't be
# retrieved, an empty file will be created.
export_serial_number()
{
    # retry for a maximum of $CRADLE_SERIAL_NUMBER_RETRIES
    for i in $(seq 1 $CRADLE_SERIAL_NUMBER_RETRIES); do

        serialNumber=`$AC_UTIL | sed -n 's/.*device serial number \(.*\)/\1/p'`
        
        if [ -n "$serialNumber" ]; then
            break
        fi
    done

    # save serial number to file
    echo "$serialNumber" > /afc/etc/cradle_serial_number

    if [ -z "$serialNumber" ]; then
        echo "Failed to read device serial number." >&2
    fi
}

verify_ac()
{
    test \! -f "$INIFILE" && echo "Device is not commissioned. Exiting" && exit 0 
    test \! -f "$AC_UTIL" && echo "Active Cradle tool not present. Exiting." && exit 0 

    echo "Reading parameters from active cradle..."
    OUTPUT=`${AC_UTIL} -r`
    
    if [ $? -eq 0 ] ; then
        eval $OUTPUT
    else
        echo "Failed to read active cradle. Retry"
        OUTPUT=`${AC_UTIL} -r`
        if [ $? -eq 0 ] ; then
            eval $OUTPUT        
        fi
    fi

    GetIni "${INIFILE}" General:TerminalId ; COMM_TERMINAL_ID="${INIENTRY}"
    GetIni "${INIFILE}" General:ServiceProviderID ; COMM_SERVICE_PROVIDER_ID="${INIENTRY}"

    VERIFIED="TRUE"
    
    test "${TERMINAL_ID}"               != "${COMM_TERMINAL_ID}"                && echo "TERMINAL_ID not verified"                  && VERIFIED="FALSE"
    test "${SERVICE_PROVIDER_ID}"       != "${COMM_SERVICE_PROVIDER_ID}"        && echo "SERVICE_PROVIDER_ID not verified"          && VERIFIED="FALSE"

    if [ "${VERIFIED}" = "FALSE" ] ; then
        echo "Terminal ${COMM_TERMINAL_ID} SP:${COMM_SERVICE_PROVIDER_ID} started on cradle ${TERMINAL_ID} SP:${SERVICE_PROVIDER_ID}. Raising Alarm"
        if [ -x ${ALARM_UTIL} ]
        then
            ${ALARM_UTIL} -i /afc/etc/alarm.ini -t DeviceManagementState -a 210 -s minor --problem-description 'Active cradle data does not match commissioning'
            ${ALARM_UTIL} -i /afc/etc/alarm.ini -t DeviceManagementState -a 210 -s cleared --problem-description 'Active cradle data does not match commissioning'            
            echo $(date -u) " DeviceManagementState-210 Raised" >> /afc/log/verify_ac.log
        fi
    fi

    export_serial_number
}

verify_ac

exit 0

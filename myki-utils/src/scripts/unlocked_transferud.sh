#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   Project/Product : MBU
#   Filename        : transferud.sh
#   Author(s)       : ?
#
#   Description     :
#       Transfers LDTs.
#
#   Subversion      :
#       $Id$
#       $HeadURL$
#
#   History         :
#    Vers.  Date        Aut.  Type     Description
#    -----  ----------  ----  -------  ----------------------------------------
#     1.00  dd.mm.yy    ?     Create
#     1.01  12.05.15    ANT   Modify   MBU-1258: Rectified first shift id not 1
#     1.02  14.05.15    ANT   Modify   MBU-1331: Rectified package activation
#                                      time
#     1.03  01.06.15    ANT   Modify   MBU-1331: Added returning Tariff
#                                      activation date/time
#     1.04  29.06.15    ANT   Modify   MTU-252: Added transferring LDTs and
#                                      forced closing shift at operational period
#                                      commit
#     1.05  14.07.15    ANT   Modify   MTU-161: (a) Added option to force opening/
#                                      closing shift (b) Added encoding user id
#                                      (c) Added coding absolute operational
#                                      period start/end date/time
#     1.06  20.07.15    ANT   Modify   MTU-161: Added backward compatibility
#     1.07  06.08.15    ANT   Modify   MTU-436: Added "-f" (force performing
#                                      operational period commit) option
#                                      Added logging messages to file
#     1.08  20.08.15    ANT   Modify   Added support for generating transaction
#                                      TMI records
#     1.09  25.08.15    ANT   Modify   Added reserving operational log record
#                                      count for next <ShiftOpen>
#     1.10  09.10.15    ANT   Modify   NGBU-590: Disabled generating <Shift>,
#                                      <ShiftOpen> and <ShiftClose> records
#     1.11  13.10.15    ANT   Modify   Rectified retrieving GenerateTmi option
#     1.12  22.10.15    ANT   Modify   MTU-830: Don't force closing empty shift
#                                      and opening new shift if forcing period
#                                      commit enabled (at start-up)
#     1.13  27.11.15    ANT   Modify   NGBU-851: Generating OprLog from MYKI-TMI
#
#################################################################################

self=`basename $0 .sh`

PATH=$PATH:/afc/bin
export PATH
# defaults
SCHEMA_VERSION=7.6
TERMINAL_OWNER=47
LOG_TYPE=batch
START_SHIFT=0                           #   Force opening/closing shift
PERIOD_COMMIT=0

TERMINAL_ID=0
TERMINAL_TYPE=UNK
LOG_NO=1
PERIOD_ID=0
USER_ID=VixGate                         #   User ID used to encode TMI
CURRENT_USER_ID=0                       #   Current logged in driver user ID
DEFAULT_USER_ID=0                       #   Default user ID when no driver logged in
USER_ID_NODRIVER=0                      #   User ID value in user id file when no Driver logged in
FORCED_PERIOD_COMMIT=0                  #   Force performing operational period commit (if not done)

TRX_COUNT=0
REMITTANCE_COUNT=0
SHIFT_REC_COUNT=0                       #   Record number within shift
SHIFT_OP_RECORD_COUNT=                  #   <ShiftOpen> operational log record count
PID_FILE=/tmp/transferud.pid
ALARM_PID_FILE=/tmp/transfer_alarms.pid

LAST_PERIOD_ID=                         #   Last closed operational period id
LAST_PERIOD_DATETIME=                   #   Last closed operational period date/time
AVL_SCRIPT=                             #   AVL script

    #   Displays usage.
    #
usage ()
{

    echo Usage: $self [options] 1>&2
    echo "    -t <terminalid>     Terminal id - The Terminal ID as per commissioning file."
    echo "    -y <terminaltype>   terminal type"
    echo "    -l <logno>          Log number - Absolute operation log count of the respective type since terminal initialization"
    echo "    -p <periodid>       Period id - The number of periods the Cobra has been running for"
    echo "    -u <userid>         User id"
    echo "    -s                  Force opening/closing shift"
    echo "    -f                  Force performing operational period (if not done)"
    echo "    -h                  print this message"
    return 0

}   #   usage


while getopts ":t:y:l:p:u:hsf" opt; do
    case $opt in
        t) TERMINAL_ID="$OPTARG";;
        y) TERMINAL_TYPE="$OPTARG";;
        l) LOG_NO="$OPTARG";;
        p) PERIOD_ID="$OPTARG";;
        u) USER_ID="$OPTARG";;
        h) usage; exit 2;;
        s) START_SHIFT=1;;
        f) FORCED_PERIOD_COMMIT=1;;
        \?)
          echo "Invalid option: -$OPTARG" >&2
          exit 1
          ;;
        :)
          echo "Option -$OPTARG requires an argument." >&2
          exit 1
          ;;
    esac
done

# Paths
BASEDIR=/afc
BIN_DIR=${BASEDIR}/bin
ETC_DIR=${BASEDIR}/etc
LOG_DIR=${BASEDIR}/log
DATA_DIR=${BASEDIR}/data
TMP_DIR=/tmp
FILE_LOG=${LOG_DIR}/transferud.log      #   Log file path (if set)
FILE_LOG_MAX_SIZE=1024000               #   Maximum log file size

INIFILE=${ETC_DIR}/transfer_ud.ini
AVL_INIFILE=${ETC_DIR}/myki-avl.ini
FILE_COMMISSIONED=${ETC_DIR}/commissioned.ini

    #   Rotates log file if required
    #
RotateLog()
{

    if [ ! -z ${FILE_LOG} ] ; then
        FILE_LOG_SIZE=`stat -c %s ${FILE_LOG}`
        if [ ${FILE_LOG_SIZE} -gt ${FILE_LOG_MAX_SIZE} ] ; then
            rm -f ${FILE_LOG}.1
            mv -f ${FILE_LOG} ${FILE_LOG}.1
        fi
    fi

}   #   RotateLog

    #   Logs a message
    #   $*      log message
    #
LogOutput()
{

    if [ ! -z ${FILE_LOG} ] ; then
        echo "`date +'%b %d %H:%M:%S'` transferud $*" | tee -a -i ${FILE_LOG}
    else
        echo "`date +'%b %d %H:%M:%S'` transferud $*"
    fi
    return 0

}   #   LogOutput

#--------------------------------------------------------------------------------------------------
#
# Log Error
#
#--------------------------------------------------------------------------------------------------
LogError()
{

    #$BIN_DIR/dbglog -e -t "$StreamTag" -l ${LOG_FACILITY} "$*"
    LogOutput "(E) $*"

}

    #   Logs information message.
    #   $*      information message
    #
LogInfo()
{

    LogOutput "(I) $*"
    return 0

}   #   LogInfo

    #   Logs debugging message.
    #   $*      debug message
    #
LogDebug()
{

#   LogOutput "(D) $*"
    return 0

}   #   LogDebug

    #   Retrieves INI parameter value
    #   $1      INI file path
    #   $2      INI parameter name
    #   $3      Default value
    #
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

test \! -f "$FILE_COMMISSIONED" && echo "Device is not commissioned." && exit 0

GetIni "${INIFILE}" LDT:Confirmed ; INCOMING_DIR="${INIENTRY}"
GetIni "${INIFILE}" LDT:Transfer ; TRANSFER_DIR="${INIENTRY}"
GetIni "${INIFILE}" LDT:TmiConfirmed ; TMI_CONFIRMED_DIR="${INIENTRY}"
GetIni "${INIFILE}" LDT:TmiTransfer ; TMI_TRANSFER_DIR="${INIENTRY}"
GetIni "${INIFILE}" LDT:SwitchImportDir ; SWITCH_IMPORT_DIR="${INIENTRY}"

TMI_TIMESTAMP="`/bin/date -u +%Y-%m-%dT%H:%M:%S`Z"

GetIni "${INIFILE}" LDT:Header ; TMI_HEADER_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:Shift ; TMI_HEADER_SHIFT_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:ShiftOpen ; TMI_HEADER_SHIFT_OPEN_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:ShiftClose ; TMI_SHIFT_CLOSE_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:Footer ; TMI_FOOTER_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:LdtTemplate ; TMI_LDT_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:PeriodCommit ; TMI_PERIOD_COMMIT_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:PeriodCommitFooter ; TMI_PERIOD_COMMIT_FOOTER_TEMPLATE="${INIENTRY}"
GetIni "${INIFILE}" LDT:VersionSnapshot ; TMI_VERSION_SNAPSHOT_TEMPLATE="${INIENTRY}"

GetIni "${INIFILE}" LDT:BatchSequence ; TMI_BATCH_SEQUENCE_NUMBER="${INIENTRY}"
GetIni "${INIFILE}" LDT:OpRecordCountSequence ; OP_RECORD_COUNT_SEQUENCE_NUMBER="${INIENTRY}"
GetIni "${FILE_COMMISSIONED}" General:TerminalId ; TERMINAL_ID="${INIENTRY}"
GetIni "${FILE_COMMISSIONED}" General:TerminalType ; TERMINAL_TYPE="${INIENTRY}"
GetIni "${FILE_COMMISSIONED}" General:ESN ; ESN="${INIENTRY}"
GetIni "${INIFILE}" LDT:ShiftOpenTimeStamp ; TMI_SHIFT_OPEN_TIMESTAMP="${INIENTRY}"
GetIni "${INIFILE}" LDT:ShiftSequence ; TMI_SHIFT_SEQUENCE_NUMBER="${INIENTRY}"
GetIni "${INIFILE}" LDT:CurrentPeriodId ; CURRENT_PERIOD_ID_FILE="${INIENTRY}"
GetIni "${INIFILE}" LDT:LogNoSequence ; LOG_NO_SEQUENCE_NUMBER="${INIENTRY}"
GetIni "${INIFILE}" LDT:UserId ; PATH_USER_ID="${INIENTRY}"

GetIni "${FILE_COMMISSIONED}" LDT:GenerateTmi N ; GENERATE_TMI="${INIENTRY}"

LogInfo "transferud.sh : GENERATE_TMI=${GENERATE_TMI}"
if [ "${GENERATE_TMI}" = "Y" ] ; then

    #   NOTE:   transferud.sh script is deprecated and replaced with myki-tmi.
    #
    LogInfo "transferud.sh : execute 'tmi createOpLog' command"
    /afc/bin/cmdutil -m -c "tmi createOpLog" /tmp/myki-app.pipe
    exit 0

fi

if [ -f ${AVL_INIFILE} ]; then
    GetIni "${AVL_INIFILE}" "MykiAvl General:GpsLogTransferScript" N ; AVL_SCRIPT="${INIENTRY}"
    if [ -f ${AVL_SCRIPT} ]; then
        sh ${AVL_SCRIPT}
    fi
fi

#          $1  user login name
CheckRunningAsUser()
{

        USER=$1
        USER_ID_CHECK=`grep ^${USER} /etc/passwd| cut -f 3 -d:`
        if [ `/usr/bin/id -u` -eq ${USER_ID_CHECK} ]; then
                return
        fi
        LogError "CheckRunningAsUser : Must run as user ${USER} - not `id`.  Aborting".
        exit 1
}


    #   Converts local date/time to UTC
    #   $1      locate date/time (YYYY-MM-DDThh:mm:ss)
    #
Zuluify()
{

    UNAME_MACHINE=`uname -m | cut -c1-3`
    if [ "${UNAME_MACHINE}" = "arm" -o "${UNAME_MACHINE}" = "ppc" ]; then

        #   Cobra or Viper
        #
        ZULUIFY_TIME_T=`date -d "$1" -D %Y-%m-%dT%H:%M:%S +%s`
        ZULUIFY_RET=`date -u -d "$ZULUIFY_TIME_T" -D %s +%Y-%m-%dT%H:%M:%S`Z

    else

        ZULUIFY_TIME_T=`date -d "$1" +%s`
        ZULUIFY_RET=`date -u -d "@$ZULUIFY_TIME_T" +%Y-%m-%dT%H:%M:%S`Z

    fi

}

    #   Get shift details
    #
GetShift()
{

    LogDebug "GetShift"

    #   FR1 shift details, ie.
    #   NNNN
    #   yyyy-mm-ddThh:mm:ssZ
    #
    SHIFT_SEQ_FR1=0
    SHIFT_OPEN_TIMESTAMP_FR1=
    if [ -f "${TMI_SHIFT_SEQUENCE_NUMBER}" ] ; then
        read SHIFT_SEQ_FR1 < "${TMI_SHIFT_SEQUENCE_NUMBER}"
    fi
    if [ -f "${TMI_SHIFT_OPEN_TIMESTAMP}" ] ; then
        read SHIFT_OPEN_TIMESTAMP_FR1 < "${TMI_SHIFT_OPEN_TIMESTAMP}"
    else
        SHIFT_OPEN_TIMESTAMP_FR1=${TMI_TIMESTAMP}
        echo ${SHIFT_OPEN_TIMESTAMP_FR1} > "${TMI_SHIFT_OPEN_TIMESTAMP}"
    fi

    #   FR2 shift details, ie.
    #   SHIFT_SEQ=NNNN
    #   SHIFT_OPEN_TIMESTAMP=yyyy-mm-ddThh:mm:ssZ
    #   SHIFT_OP_RECORD_COUNT=LLLL
    #   SHIFT_REC_COUNT=MMMM
    #   USER_ID=ABCD
    #
    SHIFT_SEQ=0
    SHIFT_OPEN_TIMESTAMP=${TMI_TIMESTAMP}
    SHIFT_OP_RECORD_COUNT=
    SHIFT_REC_COUNT=0
    USER_ID=${DEFAULT_USER_ID}
    if [ -f "${TMI_SHIFT_SEQUENCE_NUMBER}.2" ] ; then
        eval `cat ${TMI_SHIFT_SEQUENCE_NUMBER}.2`
    fi

    #   Checks if device software has been downgraded or
    #   has just been upgraded
    #
    if [ ${SHIFT_SEQ_FR1} -gt ${SHIFT_SEQ} ] ; then
        SHIFT_SEQ=${SHIFT_SEQ_FR1}
        SHIFT_OPEN_TIMESTAMP=${SHIFT_OPEN_TIMESTAMP_FR1}
        SHIFT_OP_RECORD_COUNT=
        SHIFT_REC_COUNT=0
        USER_ID=${DEFAULT_USER_ID}
    fi
    SHIFT_TIMESTAMP=${SHIFT_OPEN_TIMESTAMP}

    LogInfo "GetShift : SHIFT_SEQ=${SHIFT_SEQ}"
    LogInfo "GetShift : SHIFT_OPEN_TIMESTAMP=${SHIFT_OPEN_TIMESTAMP}"
    LogInfo "GetShift : SHIFT_TIMESTAMP=${SHIFT_TIMESTAMP}"
    LogInfo "GetShift : SHIFT_OP_RECORD_COUNT=${SHIFT_OP_RECORD_COUNT}"
    LogInfo "GetShift : SHIFT_REC_COUNT=${SHIFT_REC_COUNT}"
    LogInfo "GetShift : USER_ID=${USER_ID}"

    return 0

}   #   GetShift

    #   Sets shift details
    #   Note:   Saves as FR1 and FR2 format for backward compatibility
    #
SetShift()
{

    LogDebug "SetShift"
    LogInfo  "SetShift : SHIFT_SEQ=${SHIFT_SEQ}"
    LogInfo  "SetShift : SHIFT_OPEN_TIMESTAMP=${SHIFT_OPEN_TIMESTAMP}"
    LogInfo  "GetShift : SHIFT_OP_RECORD_COUNT=${SHIFT_OP_RECORD_COUNT}"
    LogInfo  "GetShift : SHIFT_REC_COUNT=${SHIFT_REC_COUNT}"
    LogInfo  "SetShift : CURRENT_USER_ID=${CURRENT_USER_ID}"

    #   FR1 shift details, ie.
    #   NNNN
    #   yyyy-mm-ddThh:mm:ssZ
    #
    echo ${SHIFT_SEQ} > "${TMI_SHIFT_SEQUENCE_NUMBER}"
    echo ${SHIFT_OPEN_TIMESTAMP} > "${TMI_SHIFT_OPEN_TIMESTAMP}"

    #   FR2 shift details, ie.
    #   SHIFT_SEQ=NNNN
    #   SHIFT_OPEN_TIMESTAMP=yyyy-mm-ddThh:mm:ssZ
    #   SHIFT_REC_COUNT=NNNN
    #   USER_ID=ABCD
    #
    cat <<EOF > "${TMI_SHIFT_SEQUENCE_NUMBER}.2"
SHIFT_SEQ=${SHIFT_SEQ}
SHIFT_OPEN_TIMESTAMP=${SHIFT_OPEN_TIMESTAMP}
SHIFT_OP_RECORD_COUNT=${SHIFT_OP_RECORD_COUNT}
SHIFT_REC_COUNT=${SHIFT_REC_COUNT}
USER_ID=${CURRENT_USER_ID}
EOF

    return 0

}   #   SetShift

    #   Gets last operational period details
    #
GetLastOperPeriod()
{

    LogDebug "GetLastOperPeriod"

    #   FR1 operational period details, ie.
    #   NNNN yyyy-mm-dd
    #
    LAST_PERIOD_ID_FR1=0
    LAST_PERIOD_DATETIME_FR1=
    if [ -f "${CURRENT_PERIOD_ID_FILE}" ] ; then

        read LAST_PERIOD_ID_FR1 LAST_PERIOD_DATETIME_FR1 < ${CURRENT_PERIOD_ID_FILE}
        if [ "`echo ${LAST_PERIOD_DATETIME_FR1} | grep T`" = "" ] ; then

            #   Old format, ie. without time component
            #   ASSUME last operational period ended at midnight
            #
            LAST_PERIOD_DATETIME_FR1="${LAST_PERIOD_DATETIME_FR1}T00:00:00"

        fi

    fi

    #   FR2 operational period details, ie.
    #   LAST_PERIOD_ID=NNNN
    #   LAST_PERIOD_DATETIME=yyyy-mm-ddThh:mm:ss
    #
    LAST_PERIOD_ID=0
    LAST_PERIOD_DATETIME=
    if [ -f "${CURRENT_PERIOD_ID_FILE}.2" ] ; then

        eval `cat ${CURRENT_PERIOD_ID_FILE}.2`

    fi

    #   Checks if device software has been downgraded or
    #   has just been upgraded
    #
    if [ ${LAST_PERIOD_ID_FR1} -gt ${LAST_PERIOD_ID} ] ; then

        LAST_PERIOD_ID=${LAST_PERIOD_ID_FR1}
        LAST_PERIOD_DATETIME=${LAST_PERIOD_DATETIME_FR1}

    fi

    LogInfo "GetLastOperPeriod : LAST_PERIOD_ID=${LAST_PERIOD_ID}"
    LogInfo "GetLastOperPeriod : LAST_PERIOD_DATETIME=${LAST_PERIOD_DATETIME}"

    return 0

}   #   GetLastOperPeriod

    #   Sets last operational period details
    #   Note:   Saves as FR1 and FR2 format for backward compatibility
    #
SetLastOperPeriod()
{

    LogDebug "SetLastOperPeriod"
    LogInfo  "SetLastOperPeriod : PERIOD_ID=${PERIOD_ID}"
    LogInfo  "SetLastOperPeriod : PERIOD_DATETIME=${PERIOD_DATETIME}"

    #   FR1 operational period details
    #   NNNN yyyy-mm-dd
    #
    PERIOD_DATE=`echo ${PERIOD_DATE} | cut -dT -f1`
    echo "${PERIOD_ID}" "${PERIOD_DATE}" > "${CURRENT_PERIOD_ID_FILE}"

    #   FR2 operational period details
    #   LAST_PERIOD_ID=NNNN
    #   LAST_PERIOD_DATETIME=yyyy-mm-ddThh:mm:ss
    #
    cat <<EOF > "${CURRENT_PERIOD_ID_FILE}.2"
LAST_PERIOD_ID=${PERIOD_ID}
LAST_PERIOD_DATETIME=${PERIOD_DATETIME}
EOF

    return 0

}   #   SetLastOperPeriod

    #   Transfers batch file to SWITCH.
    #   $1      archive file name
    #
PostToSwitch ()
{

    LogDebug "PostToSwitch"

    mv -f $1 ${SWITCH_IMPORT_DIR}/
    RET=$?
    if [ $RET -ne 0 ] ; then

        LogError "PostToSwitch : transferud generation failed while moving file to ${SWITCH_IMPORT_DIR} with error : <${RET}>"
        exit 1

    else

        LogInfo "PostToSwitch : moved '${1}' to '${SWITCH_IMPORT_DIR}'"
        BATCH_SEQ=$((BATCH_SEQ + 1))

        echo ${BATCH_SEQ} > ${TMI_BATCH_SEQUENCE_NUMBER}
        echo ${OP_RECORD_COUNT} > ${OP_RECORD_COUNT_SEQUENCE_NUMBER}

        #   Saves shift details
        #
        SetShift

        #   Saves operational period details
        #
        if [ "${PERIOD_COMMIT}" -eq 1 ]; then
            SetLastOperPeriod
        fi
        rm -f ${TRANSFER_DIR}/*
        rm -f ${TMI_TRANSFER_DIR}/*

    fi
    return 0

}   #   PostToSwitch

    #   Returns current logged in (if any) Driver IDs.
    #   ${DEFAULT_USER_ID}  default user id when no Driver logged in
    #   ${CURRENT_USER_ID}  ${DEFAULT_USER_ID} if no Driver logged in
    #                       Else logged in Driver user id
    #
GetCurrentDriverId()
{

    LogDebug "GetCurrentDriverId"

    #   Determines default user id
    #
    case ${TERMINAL_TYPE} in
        FPDg|FPDs|FPDm)
            DEFAULT_USER_ID="VixFPD"
            ;;
        TDC|TDCSecondary|BDC)
            DEFAULT_USER_ID="VixDC"
            ;;
        SEM)
           DEFAULT_USER_ID="VixSEM"
            ;;
        *)
            DEFAULT_USER_ID="VixGate"
            ;;
    esac

    #   Retrieves current user id
    #
    CURRENT_USER_ID=0
    if [ -f "${PATH_USER_ID}" ] ; then
        read CURRENT_USER_ID < ${PATH_USER_ID}
    fi
    if [ "${CURRENT_USER_ID}" = "" -o "${CURRENT_USER_ID}" = "${USER_ID_NODRIVER}" ] ; then
        CURRENT_USER_ID=${DEFAULT_USER_ID}
    fi
    LogInfo "GetCurrentDriverId : DEFAULT_USER_ID=${DEFAULT_USER_ID}"
    LogInfo "GetCurrentDriverId : CURRENT_USER_ID=${CURRENT_USER_ID}"

}   #   GetCurrentDriverId

    #   Determines current (last) and next operational period id
    #
CheckNewPeriod()
{

    LogDebug "CheckNewPeriod"

    #   Determines the current operational period id
    #
    OPERATIONAL_PERIOD=$((40542 + `/bin/date +%j | sed -e 's/^0*//'`))  # 31 Dec 2010 + day index
    COUNTER=`/bin/date +%Y`
    while [ ${COUNTER} -gt 2011 ]; do
        COUNTER=$((COUNTER - 1))
        OPERATIONAL_PERIOD=$((OPERATIONAL_PERIOD + 365))
        #   Simple leap year check is good enough until year 2400
        if [ $((${COUNTER} / 4 * 4)) -eq ${COUNTER} ]; then
            OPERATIONAL_PERIOD=$((OPERATIONAL_PERIOD + 1))
        fi
    done
    PERIOD_ID=${OPERATIONAL_PERIOD}
    PERIOD_DATETIME=`/bin/date +%Y-%m-%dT%H:%M:%S`

    GetLastOperPeriod
    if [ "${LAST_PERIOD_ID}" -gt 0 ] ; then

        if [ "${FORCED_PERIOD_COMMIT}" = "0" -a "${CURRENT_USER_ID}" != "${DEFAULT_USER_ID}" ] ; then

            #   Driver is currently logged in => Inhibit performing OpPeriod commit
            #
            PERIOD_ID="${LAST_PERIOD_ID}"
            PERIOD_DATETIME="${LAST_PERIOD_DATETIME}"

        fi

    else

        LAST_PERIOD_ID="${PERIOD_ID}"
        LAST_PERIOD_DATETIME="${PERIOD_DATETIME}"
        SetLastOperPeriod

    fi

    if [ "${LAST_PERIOD_ID}" -ne "${PERIOD_ID}" ]; then

        PERIOD_COMMIT=1

    else

        if [ "${FORCED_PERIOD_COMMIT}" = "1" ] ; then

            #   Don't force closing empty shift and opening new shift (on start-up)
            #
            START_SHIFT=0

        fi

    fi

    LogInfo "CheckNewPeriod : FORCED_PERIOD_COMMIT=${FORCED_PERIOD_COMMIT}"
    LogInfo "CheckNewPeriod : PERIOD_ID=${PERIOD_ID}"
    LogInfo "CheckNewPeriod : PERIOD_DATETIME=${PERIOD_DATETIME}"
    return 0

}   #   CheckNewPeriod

# Spin around sleep 2 loop wait for a max of 2 X $2 secs in a retry loop 
# until specified pipe file $1 can be found and can be connected to. Usually used to wait for 
# /tmp/dc.pipe or /tmp/myki-app.pipe
WaitUntilHavePipe ()
{
    if [ "$#" -ne 2 ] || [ -z "$1" ] || [ -z "$2" ]; then
		echo "ERROR : WaitUntilHavePipe must be give two args - the pipe file to look for and an arg for retry count"
        return
    fi
    retry_count=1
    while [ "$retry_count" -lt $2 ]; do
        if [ -r $1 ]; then
			if /afc/bin/cmdutil -m -c "cd info -a" $1; then
				return 0
			fi
		fi
        sleep 2
        retry_count=$((retry_count+1))
    done
	echo "WARNING : retry count of $2 exceeded trying $1 connect, giving up"
	return 1
}


    #   Gets package activation (installation) time.
    #   $1      package name
    #   $2      activation local time (if not null) in "yyyy-mm-ddTHH:MM:SS" format
    #
GetPackageActivationTime()
{

    PACKAGE_NAME=$1
    PACKAGE_ACTIVATION_LOCAL_TIME=$2
    PACKAGE_ACTIVATION_TIME=
    FULL_PATH=

    case ${PACKAGE_NAME} in
        Schedule)
#           FULL_PATH=${DATA_DIR}/config/schedule.db
            ;;
        Tariff)
#           FULL_PATH=${DATA_DIR}/config/productcfg.db
            ;;
        Actionlist)
#           FULL_PATH=${DATA_DIR}/config/actionlist.db
            ;;
        actionlistsdf)
#           FULL_PATH=${DATA_DIR}/config/actionlist_delta.db
            ;;
        sam)
#           FULL_PATH=${FILE_COMMISSIONED}
            PACKAGE_ACTIVATION_TIME=${TMI_TIMESTAMP}
            return 0;
            ;;
        SW)
            FULL_PATH=/var/lib/dpkg/info/${RELEASE_PACKAGE}.list
            ;;
        *)
            FULL_PATH=/var/lib/dpkg/info/${PACKAGE_NAME}.list
            ;;
    esac

    #   Retrieves file last modified local date/time
    #
    if [ ! -z ${FULL_PATH} ] ; then
        PACKAGE_ACTIVATION_LOCAL_TIME=`find ${FULL_PATH} -exec stat -c '%y' {} \; | cut -d. -f1 | sed "s/ /T/"`
    fi

    #   And converts it to UTC
    #
    if [ ! -z ${PACKAGE_ACTIVATION_LOCAL_TIME} ] ; then
        Zuluify ${PACKAGE_ACTIVATION_LOCAL_TIME}
        PACKAGE_ACTIVATION_TIME=${ZULUIFY_RET}
    else
        PACKAGE_ACTIVATION_TIME=${TMI_TIMESTAMP}
    fi
    return 0

}   #   GetPackageActivationTime

#--------------------------------------------------------------------------------------------------
#
# TransferUD
#
#--------------------------------------------------------------------------------------------------
TransferUD ()
{

    LogDebug "TransferUD"

    ACTIVATED_TIMESTAMP=${TMI_TIMESTAMP}

    #   Retrieves (last generated) operational log record number
    #
    OP_RECORD_COUNT=`cat ${OP_RECORD_COUNT_SEQUENCE_NUMBER}`
    if [ "${OP_RECORD_COUNT}" = "" ]; then
        OP_RECORD_COUNT=0
    fi
    OP_RECORD_TYPE_COUNT=0

    #   Retrieves (last generated) batch sequence number
    #
    BATCH_SEQ=`cat ${TMI_BATCH_SEQUENCE_NUMBER}`
    if [ "${BATCH_SEQ}" = "" ]; then
        BATCH_SEQ=0
    fi

    #   Retrieves (last generated) shift sequence number and
    #   user id when last shift was generated (ie. user id of
    #   current shift)
    #
    GetShift
    if [ "${USER_ID}" != "${CURRENT_USER_ID}" ] ; then

        #   Driver has just logged in/out => Forces opening/closing shift
        #
        START_SHIFT=1

    fi

    LOG_NO=`cat ${LOG_NO_SEQUENCE_NUMBER}`
    if [ "${LOG_NO}" = "" ]; then
        LOG_NO=1
    fi

    #   Generates batch file name
    #
    ARCHIVE_FILENAME="UD`/bin/date +%Y%m%d_%H%M%S_``printf %08x_%04x_%04x_%02x_%04x_%08x ${TERMINAL_ID} 0 ${BATCH_SEQ} 1 0 0`.xml"
    ARCHIVE_FILENAME_GZIP="${ARCHIVE_FILENAME}.gz"

    RET=0
    if [ -d "${TRANSFER_DIR}" -a -d "${SWITCH_IMPORT_DIR}" ] ; then

        #   Checks for pending transaction TMI records - OBSOLETE
        #
        PENDING_CONFIRMED_TMIS=
#       if [ "${GENERATE_TMI}" = "Y" ] ; then
#
#           if [ -d "${TMI_TRANSFER_DIR}" ] ; then
#
#               cd ${TMI_TRANSFER_DIR}
#               PENDING_CONFIRMED_TMIS=`ls tmi*.xml 2>/dev/null`
#
#           else
#
#               LogError "TransferUD : ${TMI_TRANSFER_DIR} folder does not exists"
                GENERATE_TMI=N
#
#           fi
#
#       fi
        LogInfo "TransferUD : GENERATE_TMI=${GENERATE_TMI}"

        #   Checks for pending "raw" LDTs
        #
        if [ -d "${INCOMING_DIR}" ] ; then

            #   Moves all generated LDTs (if any) to transfer folder
            #
            cd ${INCOMING_DIR}
            mv -f ${INCOMING_DIR}/ldt* ${TRANSFER_DIR}/ 2>/dev/null

        fi

        cd ${TRANSFER_DIR}
        PENDING_CONFIRMED_LDTS=`ls ldt*.xml 2>/dev/null`
        PENDING_UNCONFIRMED_LDTS=`ls ldt*.unconfirmed 2>/dev/null`

        if [ "${PENDING_CONFIRMED_LDTS}" != "" -o "${PENDING_UNCONFIRMED_LDTS}" != "" -o "${PENDING_CONFIRMED_TMIS}" != "" -o "${PERIOD_COMMIT}" = 1 -o "${START_SHIFT}" = 1 ] ; then

            #   <OpLog>
            #
            CreateTMIOperationalLog ${TRANSFER_DIR}/ ${SCHEMA_VERSION} ${TERMINAL_OWNER} ${TERMINAL_TYPE} ${TERMINAL_ID} ${LOG_TYPE} ${LOG_NO} ${TMI_TIMESTAMP} ${LAST_PERIOD_ID} ${LAST_PERIOD_DATETIME} ${ARCHIVE_FILENAME}

            if [ "${SHIFT_OP_RECORD_COUNT}" = "" ] ; then
                OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                SHIFT_OP_RECORD_COUNT=${OP_RECORD_COUNT}
            fi
            SHIFT_SEQ=$((SHIFT_SEQ + 1))
#           if [ "${GENERATE_TMI}" != "Y" ] ; then

                #   <Shift>
                #
                CreateTMIShiftStart ${TRANSFER_DIR}/ ${SHIFT_SEQ} ${SHIFT_TIMESTAMP} ${USER_ID} ${ARCHIVE_FILENAME}

                #   <ShiftOpen>
                #
                LogInfo "TransferUD : ShiftOpen ${SHIFT_SEQ} ${USER_ID}"
                CreateTMIShiftOpen ${TRANSFER_DIR}/ ${SHIFT_OP_RECORD_COUNT} ${SHIFT_SEQ} ${SHIFT_TIMESTAMP} ${USER_ID} ${ARCHIVE_FILENAME}
                


#           fi

            #   Sets next shift open date/time
            #
            SHIFT_OPEN_TIMESTAMP=${TMI_TIMESTAMP}

        fi

#       if [ "${PENDING_CONFIRMED_TMIS}" != "" ] ; then
 
            #   Confirmed TMIs - OBSOLETE
            #
#           for i in ${PENDING_CONFIRMED_TMIS} ; do
#
#               cat ${TMI_TRANSFER_DIR}/${i} >> ${TRANSFER_DIR}/${ARCHIVE_FILENAME}
#
#           done
 
#       fi

        if [ "${PENDING_CONFIRMED_LDTS}" != "" ] ; then

            #   Confirmed LDTs
            #
            DATA_BLOCK_NAME=CscValidation
            for i in ${PENDING_CONFIRMED_LDTS} ; do

                SHIFT_REC_COUNT=$((SHIFT_REC_COUNT + 1))
                OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                OP_RECORD_TYPE_COUNT=${OP_RECORD_COUNT}
                CARDEVENT_TIMESTAMP=`cat "$i" | grep timestamp= | head -1 | sed -e 's/.*timestamp="*//' -e 's/[^-:.TZ0-9].*//'`
                if [ "$CARDEVENT_TIMESTAMP" = "" ]; then
                    CARDEVENT_TIMESTAMP=${TMI_TIMESTAMP}
                elif echo "$CARDEVENT_TIMESTAMP" | grep '.0000000$' > /dev/null; then
                    Zuluify `echo $CARDEVENT_TIMESTAMP | sed -e 's/.0000000$//'`
                    CARDEVENT_TIMESTAMP=${ZULUIFY_RET}
                fi
                CreateTMICardEvent ${TRANSFER_DIR}/ ${OP_RECORD_COUNT} ${OP_RECORD_TYPE_COUNT} ${SHIFT_SEQ} ${i} ${ARCHIVE_FILENAME} ${CARDEVENT_TIMESTAMP} ${DATA_BLOCK_NAME} ${SHIFT_REC_COUNT}

            done

        fi

        if [ "${PENDING_UNCONFIRMED_LDTS}" != "" ] ; then

            #   Unconfirmed LDTs
            #
            DATA_BLOCK_NAME=UnConfirmed_CscValidation
            for i in ${PENDING_UNCONFIRMED_LDTS} ; do

                SHIFT_REC_COUNT=$((SHIFT_REC_COUNT + 1))
                OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                OP_RECORD_TYPE_COUNT=${OP_RECORD_COUNT}
                CARDEVENT_TIMESTAMP=`cat "$i" | grep timestamp= | head -1 | sed -e 's/.*timestamp="*//' -e 's/[^-:.TZ0-9].*//'`
                if [ "$CARDEVENT_TIMESTAMP" = "" ]; then
                    CARDEVENT_TIMESTAMP=${TMI_TIMESTAMP}
                elif echo "$CARDEVENT_TIMESTAMP" | grep '.0000000$' > /dev/null; then
                    Zuluify `echo $CARDEVENT_TIMESTAMP | sed -e 's/.0000000$//'`
                    CARDEVENT_TIMESTAMP=${ZULUIFY_RET}
                fi
                CreateTMICardEvent ${TRANSFER_DIR}/ ${OP_RECORD_COUNT} ${OP_RECORD_TYPE_COUNT} ${SHIFT_SEQ} ${i} ${ARCHIVE_FILENAME} ${CARDEVENT_TIMESTAMP} ${DATA_BLOCK_NAME} ${SHIFT_REC_COUNT}

            done

        fi

        if [ "${PENDING_CONFIRMED_LDTS}" != "" -o "${PENDING_UNCONFIRMED_LDTS}" != "" -o "${PENDING_CONFIRMED_TMIS}" != "" -o "${PERIOD_COMMIT}" = 1 -o "${START_SHIFT}" = 1 ] ; then

            SHIFT_CLOSE_TIMESTAMP="${TMI_TIMESTAMP}"
            OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
            OP_RECORD_TYPE_COUNT=${OP_RECORD_COUNT}
#           if [ "${GENERATE_TMI}" != "Y" ] ; then

                #   <ShiftClose>
                #
                LogInfo "TransferUD : ShiftClose ${SHIFT_SEQ} ${USER_ID}"            
                
                #USER_ID=`sed -n 1p /afc/etc/transactionCount | awk 'BEGIN{FS="="} {print $2}'`
                TRX_COUNT=`sed -n 2p /afc/etc/transactionCount | awk 'BEGIN{FS="="} {print $2}'`
                
                CreateTMIShiftClose ${TRANSFER_DIR}/ ${SHIFT_CLOSE_TIMESTAMP} ${OP_RECORD_COUNT} ${OP_RECORD_TYPE_COUNT} ${SHIFT_SEQ} ${SHIFT_TIMESTAMP} ${USER_ID} ${TRX_COUNT} ${REMITTANCE_COUNT} ${SHIFT_REC_COUNT} ${ARCHIVE_FILENAME}
#           fi

            if [ "${PERIOD_COMMIT}" = 1 ] ; then

                #   <PeriodCommit>
                #
                LogInfo "TransferUD : PeriodCommit ${LAST_PERIOD_ID} ${LAST_PERIOD_DATETIME}"
                CreateTMIPeriodCommit ${TRANSFER_DIR}/ ${LAST_PERIOD_ID} ${LAST_PERIOD_DATETIME} ${PERIOD_DATETIME} ${TERMINAL_TYPE} ${ARCHIVE_FILENAME}

                #   Retrieves module versions and activation date/times
                #
                RELEASE_PACKAGE=`echo $TERMINAL_TYPE|tr A-Z a-z`
                if [ "${RELEASE_PACKAGE}" = "tdcsecondary" ]; then
                    RELEASE_PACKAGE=tdc
                fi
                dpkg -l | grep mbu | tr -s ' ' | cut -d' ' -f2,3|sed "s/^${RELEASE_PACKAGE} \+/SW /" > /tmp/versions.$$
                # Expected to avoid situations where readerapp isn't up yet, just post-startup
                WaitUntilHavePipe "/tmp/myki-app.pipe" "10"
                if [ -r /tmp/myki-app.pipe -a -x /afc/bin/cmdutil ]; then

                    /afc/bin/cmdutil -m -c "cd info -a" /tmp/myki-app.pipe | grep -i "tariff" >> /tmp/versions.$$
                    /afc/bin/cmdutil -m -c "actionlist info -a" /tmp/myki-app.pipe | grep -i "actionlist" >> /tmp/versions.$$
                    /afc/bin/cmdutil -m -c "sam info -a" /tmp/myki-app.pipe | grep -i "sam" >> /tmp/versions.$$

                fi
                
                if [ "${TERMINAL_TYPE}" = "TDC" ] || [ "${TERMINAL_TYPE}" = "BDC" ]  ; then
                    # Expected to avoid situations where dc isn't up yet, just post-startup
                    WaitUntilHavePipe "/tmp/dc.pipe" "10"
                    /afc/bin/cmdutil -m -c "schedule info -a" /tmp/dc.pipe | grep -i "schedule" >> /tmp/versions.$$
                fi

                #   Adds module versions and activation date/times to operational period commit
                #
                while read PACKAGE VERSION ACTIVATED_TIMESTAMP; do

                    OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                    GetPackageActivationTime "${PACKAGE}" ${ACTIVATED_TIMESTAMP}
                    CreateTMIVersionSnapshot ${TRANSFER_DIR}/ ${PACKAGE_ACTIVATION_TIME} "${PACKAGE}" "${VERSION}" ${ARCHIVE_FILENAME}

                done < /tmp/versions.$$
                rm -f /tmp/versions.$$

                #   </OprLog>
                #
                OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                CreateTMIPeriodCommitFooter ${TRANSFER_DIR}/ ${OP_RECORD_COUNT} ${ARCHIVE_FILENAME}

                OP_RECORD_COUNT=0

            fi

            #   </OpLog>
            #
            CreateTMIFileFooter ${TRANSFER_DIR}/ ${ARCHIVE_FILENAME}

            #   Starts new shift.
            #   Reserved operational log record count for next <ShiftOpen>
            #
            OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
            SHIFT_OP_RECORD_COUNT=${OP_RECORD_COUNT}
            SHIFT_REC_COUNT=0

            #   And transfers generated operational log
            #
            /bin/gzip -9 ${ARCHIVE_FILENAME}
            PostToSwitch ${ARCHIVE_FILENAME_GZIP}

        fi

    else

        test ! -d "${TRANSFER_DIR}"         && echo "'${TRANSFER_DIR}' directory does not exist"
        test ! -d "${SWITCH_IMPORT_DIR}"    && echo "'${SWITCH_IMPORT_DIR}' directory does not exist"

    fi

    if [ ${RET} -ne 0 ]; then
        RET=1
    fi
    return ${RET}

}   #   TransferUD

    #   Generates <OprLog> element
    #   $1      archive folder path
    #   $2      schema version
    #   $3      terminal owner
    #   $4      terminal type
    #   $5      terminal id
    #   $6      log type
    #   $7      log number
    #   $8      current date/time
    #   $9      operational period id
    #   $10     operational period start date/time
    #   $11     archive file name
    #
CreateTMIOperationalLog()
{

    LogDebug "CreateTMIOperationalLog"

    Zuluify "${10}"
    OPEN_TIMESTAMP=${ZULUIFY_RET}

    echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>" >  $1/${11}

    echo "s/##schema_version/$2/
s/##terminal_owner/$3/
s/##terminal_type/$4/
s/##terminal/$5/
s/##log_type/$6/
s/##log_no/$7/
s/##log_timestamp/$8/
s/##period_open/${OPEN_TIMESTAMP}/
s/##period_id/$9/
"  > ${TMP_DIR}/tmihdr.sed
    cat ${TMI_HEADER_TEMPLATE} | sed -f ${TMP_DIR}/tmihdr.sed >> $1/${11}

    # Log number increment and persistance has been placed here to ensure log number increases regardless of
    # whether the batching is successful or not.
    if [ "${LOG_NO}" -ge "9999999999" ] ; then
        LOG_NO = 1
    else
        LOG_NO=$((LOG_NO + 1))
    fi
    echo ${LOG_NO} > ${LOG_NO_SEQUENCE_NUMBER}

}

#--------------------------------------------------------------------------------------------------
#
# File header - shift open
#
#--------------------------------------------------------------------------------------------------
CreateTMIShiftOpen()
{

    LogDebug "CreateTMIShiftOpen"

    echo "" >> $1/$6
    echo "s/##shift_id/$3/
s/##shift_open/$4/
s/##user_id/$5/
s/##timestamp/$4/
s/##op_record_count/$2/
s/##op_record_type_count/1/
"  > ${TMP_DIR}/tmishiftopen.sed
    cat ${TMI_HEADER_SHIFT_OPEN_TEMPLATE} |sed -f ${TMP_DIR}/tmishiftopen.sed >> $1/$6

}

    #   Generates <ShiftClose> element
    #   $1      archive folder path
    #   $2      timestamp
    #   $3      op_record_count
    #   $4      op_record_type_count
    #   $5      shift_id
    #   $6      shift_open
    #   $7      user_id
    #   $8      trx_count
    #   $9      remittance_count
    #   $10     ldt_container_count
    #   $11     archive file name
    #
CreateTMIShiftClose()
{

    LogDebug "CreateTMIShiftClose"

    echo "s/##timestamp/$2/
s/##op_record_count/$3/
s/##op_record_type_count/$4/
s/##shift_id/$5/
s/##shift_open/$6/
s/##user_id/$7/
s/##trx_count/$8/
s/##remittance_count/$9/
s/##ldt_container_count/$10/
"  > ${TMP_DIR}/tmiftr.sed
    cat ${TMI_SHIFT_CLOSE_TEMPLATE} | sed -f ${TMP_DIR}/tmiftr.sed >> $1/${11}

    cat <<EOF > "/afc/etc/transactionCount"
USER_ID=${7}
SHIFT_REC_COUNT=0
EOF
}   #   CreateTMIShiftClose

#--------------------------------------------------------------------------------------------------
#
# File header - shift start
#
#--------------------------------------------------------------------------------------------------
CreateTMIShiftStart()
{

    LogDebug "CreateTMIShiftStart"

    echo "" >> $1/$5
    echo "s/##shift_id/$2/
s/##shift_open/$3/
s/##user_id/$4/
" > ${TMP_DIR}/tmishift.sed
    cat ${TMI_HEADER_SHIFT_TEMPLATE} |sed -f ${TMP_DIR}/tmishift.sed >> $1/$5

}


#--------------------------------------------------------------------------------------------------
#
# Card Event
#
#--------------------------------------------------------------------------------------------------
CreateTMICardEvent()
{

    LogDebug "CreateTMICardEvent"

#Remove line breaks and tabs
sed 's/^[ \t]*//;s/[ \t]*$//' ${TRANSFER_DIR}/$5 | tr -d '\r\n' > ${TMP_DIR}/tmicardevent.sed.$$


FULLTEXT=`cat ${TMP_DIR}/tmicardevent.sed.$$`
#echo "'$FULLTEXT'"

ldtlength=$(stat -c '%s' ${TMP_DIR}/tmicardevent.sed.$$)
echo "ldtlength: '$ldtlength'"

rm -f "${TMP_DIR}/tmicardevent.sed.$$"

echo "
<LDTContainer timestamp=\"$7\" op_record_count=\"$2\" op_record_type_count=\"$3\" shift_ref=\"$4\" shift_rec_count=\"$9\">" >> $1/$6

sequenceNo=0
a=1
b=3988
# Create an LDT element with maximum length of 4000 characters, including the enclosing
# <![CDATA[   and   ]]>  tags.  See MBU-974.
while [ $a -le $ldtlength ]
do
   echo -n "<LDT name=\"${8}\" sequence_no=\"${sequenceNo}\" encoding=\"utf-8\"><![CDATA[" >> $1/$6

   echo "${FULLTEXT}" | cut -c$a-$b | tr -d '\n' >>  $1/$6
   echo "]]></LDT>"  >> $1/$6

   a=`expr $a + 3988`
   b=`expr $b + 3988`
   sequenceNo=`expr $sequenceNo + 1`
done

echo "</LDTContainer>"  >> $1/$6

}

    #   </OpLog>
    #   $1      archive folder path
    #   $2      archive file name
    #
CreateTMIFileFooter()
{

    LogDebug "CreateTMIFileFooter"

    cat ${TMI_FOOTER_TEMPLATE} >> ${1}/${2}
    return 0

}   #   CreateTMIFileFooter

    #   Generates <PeriodCommit> element.
    #   $1      archive folder path
    #   $2      operational period id
    #   $3      operational period start date/time
    #   $4      operational period end date/time
    #   $5      assembly type
    #   $6      archive file name
    #
CreateTMIPeriodCommit()
{

    LogDebug "CreateTMIPeriodCommit"

    Zuluify "${3}"
    OPEN_TIMESTAMP=${ZULUIFY_RET}
    Zuluify "${4}"
    CLOSE_TIMESTAMP=${TMI_TIMESTAMP}

    echo "" >> $1/$6
    echo "s/##period_id/$2/
s/##period_open/${OPEN_TIMESTAMP}/
s/##period_close/${CLOSE_TIMESTAMP}/
s/##assembly_type/$5/
s/##serial_number/${ESN}/
"  > ${TMP_DIR}/tmiperiodcommit.sed
    cat ${TMI_PERIOD_COMMIT_TEMPLATE} |sed -f ${TMP_DIR}/tmiperiodcommit.sed >> $1/$6

}

#--------------------------------------------------------------------------------------------------
#
# Version Snapshot
#
#--------------------------------------------------------------------------------------------------
CreateTMIVersionSnapshot()
{

    LogDebug "CreateTMIVersionSnapshot"

    echo "" >> $1/$5
    echo "s/##activatedTimestamp/$2/
s/##name/$3/
s/##value/$4/
"  > ${TMP_DIR}/tmiversionsnapshot.sed
    cat ${TMI_VERSION_SNAPSHOT_TEMPLATE} |sed -f ${TMP_DIR}/tmiversionsnapshot.sed >> $1/$5

}

#--------------------------------------------------------------------------------------------------
#
# Period Commit Footer
#
#--------------------------------------------------------------------------------------------------
CreateTMIPeriodCommitFooter()
{

    LogDebug "CreateTMIPeriodCommitFooter"
    echo "" >> $1/$3
    echo "s/##ldt_containers/$2/
"  > ${TMP_DIR}/tmiperiodcommitfooter.sed
    cat ${TMI_PERIOD_COMMIT_FOOTER_TEMPLATE} |sed -f ${TMP_DIR}/tmiperiodcommitfooter.sed >> $1/$3

}

export BASEDIR BIN_DIR ETC_DIR SCRIPTS_DIR LOG_DIR
export EOD_INI_FILE GETINI
export LOG_FACILITY

test -z "${TERMINAL_ID}" -o "{$TERMINAL_ID}" = "0" && echo "Device is not commissioned." && exit 0

CheckRunningAsUser root
GetCurrentDriverId                      #   Retrieves current logged in (if any) Driver user id
CheckNewPeriod

TransferUD
RotateLog                               #   Checks for log file rotation

RET=$?

exit $RET

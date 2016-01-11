#!/bin/bash
####################################################################################################

PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin
export PATH

INIFILE=/afc/etc/transfer_alarms.ini
INCOMING_DIR=`getini -f ${INIFILE} Alarms:Confirmed`
mkdir -p "$INCOMING_DIR"
TRANSFER_DIR=`getini -f ${INIFILE} Alarms:Transfer`
mkdir -p "$TRANSFER_DIR"
SWITCH_IMPORT_DIR=`getini -f ${INIFILE} Alarms:SwitchImportDir`
mkdir -p "$SWITCH_IMPORT_DIR"
TMI_TIMESTAMP="`/bin/date -u +%Y-%m-%dT%H:%M:%S`Z"
TMI_HEADER_TEMPLATE=`getini -f ${INIFILE} Alarms:Header`
TMI_FOOTER_TEMPLATE=`getini -f ${INIFILE} Alarms:Footer`
TMI_BATCH_SEQUENCE_NUMBER=`getini -f ${INIFILE} Alarms:BatchSequence`
OP_RECORD_COUNT_SEQUENCE=`getini -f ${INIFILE} Alarms:OpRecordCountSequence`
LOG_NO_SEQUENCE_NUMBER=`getini -f ${INIFILE} Alarms:LogNoSequence`
CURRENT_PERIOD_ID_FILE=`getini -f ${INIFILE} Alarms:CurrentPeriodId`
TERMINAL_ID=`getini -f ${INIFILE} General:TerminalId`
TERMINAL_TYPE=`getini -f ${INIFILE} General:TerminalType`
GENERATE_TMI=`getini -f ${INIFILE} LDT:GenerateTmi`

PID_FILE=/tmp/transfer_alarms.pid
UD_PID_FILE=/tmp/transferud.pid

####################################################################################################

    #   Counters
    #
periodOpenDate=0            #   Operational period open date/time (epoch)
periodId=0                  #   Operational period id
opLogCount=0                #   Operational log count
operationalRecordCount=0    #   Operational record count
alertCount=0                #   Alert/Alarm record type count
valueChangeCount=0          #   Value change record type count

####################################################################################################

    #   Logs a message
    #   $*      log message
    #
LogOutput()
{

    echo "`date +'%b %d %H:%M:%S'` transfer_alarms $*"

}   #   LogOutput

LogError()
{

    #$BIN_DIR/dbglog -e -t "$StreamTag" -l ${LOG_FACILITY} "$*"
    echo "ERROR : $*"

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

    LogOutput "(D) $*"
    return 0

}   #   LogDebug

#          $1  user login name
CheckRunningAsUser()
{

   USER=$1
   if [ "`/usr/bin/id -u ${USER}`" = "`/usr/bin/id -u`" ]; then
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

TransferAlarms ()
{

    SCHEMA_VERSION=1.0
    TERMINAL_OWNER=47
    LOG_TYPE=batch
    LOG_NO=1

    #   Determines current operational period details
    #
    LogInfo "TransferAlarms : GENERATE_TMI=${GENERATE_TMI}"
    if [ "${GENERATE_TMI}" = "Y" ] ; then

        #   Retrieves counter values
        #
        eval `/afc/bin/cmdutil -m -c "counter get periodId periodOpenDate" /tmp/myki-app.pipe 2>/dev/null`

        #   And sets script variables
        #
        PERIOD_ID=${periodId}
        PERIOD_DATETIME=`date -u -d @${periodOpenDate} +%Y-%m-%dT%H:%M:%S`

        LogInfo "TransferAlarms : GET PERIOD_ID=${PERIOD_ID}"
        LogInfo "TransferAlarms : GET PERIOD_DATETIME=${PERIOD_DATETIME}"

        if [ "${PERIOD_ID}" = "0" -o "${PERIOD_ID}" = "" ] ; then

            LogInfo "TransferAlarms : no operational period current, delay sending Alarm/ValueChange"
            return  1

        fi

    else

        GetLastOperPeriod
        if [ "${LAST_PERIOD_ID}" -ne 0 ] ; then

            PERIOD_ID=${LAST_PERIOD_ID}
            PERIOD_DATETIME=${LAST_PERIOD_DATETIME}

        else

            OPERATIONAL_PERIOD=$((40542 + `/bin/date +%j | sed -e 's/^0*//'`))  # 31 Dec 2010 + day index
            COUNTER=`/bin/date +%Y`
            while [ $COUNTER -gt 2011 ]; do
                COUNTER=$(($COUNTER - 1))
                OPERATIONAL_PERIOD=$(($OPERATIONAL_PERIOD + 365))
                # simple leap year check is good enough until year 2400
                if [ $(($COUNTER / 4 * 4)) -eq $COUNTER ]; then
                    OPERATIONAL_PERIOD=$((OPERATIONAL_PERIOD + 1))
                fi
            done
            PERIOD_ID=${OPERATIONAL_PERIOD}
            PERIOD_DATETIME=`/bin/date +%Y-%m-%dT%H:%M:%S`

        fi

        BATCH_SEQ=`cat ${TMI_BATCH_SEQUENCE_NUMBER}`
        if [ "$BATCH_SEQ" = "" ]; then
            BATCH_SEQ=0
        fi
        OP_RECORD_COUNT=`cat ${OP_RECORD_COUNT_SEQUENCE}`
        if [ "$OP_RECORD_COUNT" = "" ]; then
            OP_RECORD_COUNT=0
        fi

        LOG_NO=`cat ${LOG_NO_SEQUENCE_NUMBER}`
        if [ "$LOG_NO" = "" ]; then
            LOG_NO=1 # Default Log number to one
        fi

    fi

    RET=0

    if [ -d "$TRANSFER_DIR" -a -d "$INCOMING_DIR" -a -d "$SWITCH_IMPORT_DIR" ]; then

        cd "${INCOMING_DIR}"
        mv ${INCOMING_DIR}/EV*.xml "${TRANSFER_DIR}/"
        RET=$?
        if [ $RET -ne 0 ]; then

            LogError "transfer_alarms.sh: no files to move in ${TRANSFER_DIR} : <${RET}>"

        else

            cd ${TRANSFER_DIR}

            #   Reserves record counts
            #
            if [ "${GENERATE_TMI}" = "Y" ] ; then

                NUMBER_OF_EVENTS=`ls -l EV*.xml | wc -l`
                NUMBER_OF_VALUECHANGES=`grep -l ValueChange EV*.xml | wc -l`
                NUMBER_OF_ALERTS=$((NUMBER_OF_EVENTS - NUMBER_OF_VALUECHANGES))

                eval `/afc/bin/cmdutil -m -c "counter inc opLogCount 1 operationalRecordCount ${NUMBER_OF_EVENTS} alertCount ${NUMBER_OF_ALERTS} valueChangeCount ${NUMBER_OF_VALUECHANGES}" /tmp/myki-app.pipe 2>/dev/null`

                OP_RECORD_COUNT=$((operationalRecordCount - NUMBER_OF_EVENTS))
                LOG_NO=${opLogCount}
                BATCH_SEQ=${LOG_NO}
                ALERT_RECORD_COUNT=$((alertCount - NUMBER_OF_ALERTS))
                VALUE_CHANGE_RECORD_COUNT=$((valueChangeCount - NUMBER_OF_VALUECHANGES))

                LogInfo "TransferAlarms : SET LOG_NO = ${LOG_NO} (+1)"
                LogInfo "TransferAlarms : SET OP_RECORD_COUNT = ${OP_RECORD_COUNT} + ${NUMBER_OF_EVENTS} = ${operationalRecordCount}"
                LogInfo "TransferAlarms : SET ALERT_RECORD_COUNT = ${ALERT_RECORD_COUNT} + ${NUMBER_OF_ALERTS} = ${alertCount}"
                LogInfo "TransferAlarms : SET VALUE_CHANGE_RECORD_COUNT = ${VALUE_CHANGE_RECORD_COUNT} + ${NUMBER_OF_VALUECHANGES} = ${valueChangeCount}"

            fi

            ARCHIVE_FILENAME="/tmp/EV`/bin/date +%Y%m%d_%H%M%S_``printf %08x_%04x_%04x_%02x_%04x_%08x ${TERMINAL_ID} 0 ${BATCH_SEQ} 1 0 0`.xml"
            ARCHIVE_FILENAME_GZIP="${ARCHIVE_FILENAME}.gz"

            CreateTMIOperationalLog "${ARCHIVE_FILENAME}" ${SCHEMA_VERSION} ${TERMINAL_OWNER} ${TERMINAL_TYPE} ${TERMINAL_ID} ${LOG_TYPE} ${LOG_NO} ${TMI_TIMESTAMP} ${PERIOD_ID} ${PERIOD_DATETIME}
            for i in $(ls EV*.xml); do

                if [ "${i}" != "${ARCHIVE_FILENAME}" ]; then

                    #   Determines op_record_type_count value
                    #
                    OP_RECORD_TYPE_COUNT=$((OP_RECORD_COUNT + 1))
                    if [ "${GENERATE_TMI}" = "Y" ] ; then

                        if grep -q ValueChange "${i}" ; then

                            #   <ValueChange> TMI record
                            #
                            VALUE_CHANGE_RECORD_COUNT=$((VALUE_CHANGE_RECORD_COUNT + 1))
                            OP_RECORD_TYPE_COUNT=${VALUE_CHANGE_RECORD_COUNT}

                        else

                            #   <Alarm> or <Alert> TMI record
                            #
                            ALERT_RECORD_COUNT=$((ALERT_RECORD_COUNT + 1))
                            OP_RECORD_TYPE_COUNT=${ALERT_RECORD_COUNT}

                        fi

                    fi

                    OP_RECORD_COUNT=$((OP_RECORD_COUNT + 1))
                    CreateTMIAlarm "${ARCHIVE_FILENAME}" ${i} ${OP_RECORD_COUNT} ${OP_RECORD_TYPE_COUNT}

                fi

            done
            CreateTMIFileFooter "${ARCHIVE_FILENAME}"

            /bin/gzip -9 "${ARCHIVE_FILENAME}"
            /bin/mv -f "${ARCHIVE_FILENAME_GZIP}" "${SWITCH_IMPORT_DIR}"
            RET=$?
            if [ $RET -ne 0 ]; then

                LogError "transfer_alarms.sh: move to $SWITCH_IMPORT_DIR failed with error : <${RET}>"

            else

                if [ ! "${GENERATE_TMI}" = "Y" ] ; then

                    BATCH_SEQ=$(($BATCH_SEQ+1))
                    echo ${BATCH_SEQ} > ${TMI_BATCH_SEQUENCE_NUMBER}
                    echo ${OP_RECORD_COUNT} > ${OP_RECORD_COUNT_SEQUENCE}

                fi
                rm -f ${TRANSFER_DIR}/EV*.xml

            fi
        fi

    else

        LogError "transfer_alarms.sh: directories missing"
        RET=1

    fi

    if [ $RET -ne 0 ]; then

        RET=1

    fi
    return $RET

}

    #   Generates <OprLog> element
    #   $1      archive full path
    #   $2      schema version
    #   $3      terminal owner
    #   $4      terminal type
    #   $5      terminal id
    #   $6      log type
    #   $7      log number
    #   $8      current date/time
    #   $9      operational period id
    #   $10     operational period start date/time
    #
CreateTMIOperationalLog()
{

    Zuluify "${10}"
    OPEN_TIMESTAMP=${ZULUIFY_RET}

    echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>" >  "$1"

    echo "s/##schema_version/$2/
s/##terminal_owner/$3/
s/##terminal_type/$4/
s/##terminal/$5/
s/##log_type/$6/
s/##log_no/$7/
s/##log_timestamp/$8/
s/##period_open/${OPEN_TIMESTAMP}/
s/##period_id/$9/
"  > /tmp/alarm_tmi_header.sed.$$
    cat ${TMI_HEADER_TEMPLATE} | sed -f /tmp/alarm_tmi_header.sed.$$ >> "$1"
    rm /tmp/alarm_tmi_header.sed.$$

    # Log number increment and persistance has been placed here to ensure log number increases regardless of
    # whether the batching is successful or not.
    if [ "${LOG_NO}" -ge "9999999999" ] ; then
        LOG_NO = 1
    else
       LOG_NO=$(($LOG_NO+1))
    fi
    echo ${LOG_NO} > ${LOG_NO_SEQUENCE_NUMBER}

}

#--------------------------------------------------------------------------------------------------
#
# Alarm
#
#--------------------------------------------------------------------------------------------------
CreateTMIAlarm()
{

    # append the alarm but with no white-space at the start/end of lines
    # and with no new line characters. Also replace the op_record_count
    # and op_record_type_count with the right number
    cat "$2" | \
    sed -e 's/^[ \t]*//' \
        -e 's/[ \t]*$//' \
        -e "s/op_record_count=\"1\"/op_record_count=\"${3}\"/" \
        -e "s/op_record_type_count=\"1\"/op_record_type_count=\"${4}\"/" | \
    tr -d '\r\n' >> "$1"

}

#--------------------------------------------------------------------------------------------------
#
# File footer - shift close
#
#--------------------------------------------------------------------------------------------------
CreateTMIFileFooter ()
{
cat ${TMI_FOOTER_TEMPLATE} >> "$1"
}

test -z "$TERMINAL_ID" -o "$TERMINAL_ID" = "0" && echo "Device is not commissioned." && exit 0

CheckRunningAsUser root

TransferAlarms
RET=$?

exit $RET

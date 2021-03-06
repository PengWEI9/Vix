#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   Project/Product : MBU
#   Filename        : transfer_audit.sh
#   Author(s)       : ?
#
#   Description     :
#       Transfers EMV Transaction logs to TLS.
#
#   Subversion      :
#       $Id$
#       $HeadURL$
#
#   History         :
#    Vers.  Date        Aut.  Type     Description
#    -----  ----------  ----  -------  ----------------------------------------
#     1.00  dd.mm.yy    ?     Create
#
#################################################################################

self=`basename $0 .sh`

PATH=$PATH:/afc/bin
export PATH
LD_LIBRARY_PATH=/afc/bin:$LD_LIBRARY_PATH
# defaults
PID_FILE=/tmp/transfer_audit.pid
COMPRESS_FILES_DFLT=0            # Flag to indicate files should be compressed before sending/archiving. DEFAULT: 0
RETRY_TIMEOUT_SEC_DLFT=120       # Number of seconds to wait if the transfer of some files fails. DEFAULT: 120
RETRY_ATTEMPTS_DFLT=10           # Number of times to attempt to transfer files. DEFAULT: 10
MAX_TRANSFER_TIME_DFLT=300       # Maximum number of seconds permitted to transfer the file. DEFAULT: 300

#Logging options
LOG_WARNING=1               # Flag to show/log warning messages
LOG_DEBUG=0                 # Flag to show/log debug messages
LOG_INFO=0                  # Flag to show/log info messages
LOG_CONSOLE=0               # Flag whether to output messages to stdout

MV_CP_VERBOSE_FLAG_DFLT=""       # Increase the verbosity of mv, cp, curl, gzip

FIRST_SEND_ATTEMPT=1        # Whether this is the first run through this script.
AUDIT_FILE_COUNTER=1        # Counter to append to filename for the file send order

    #   Displays usage.
    # 
usage()
{
    echo Usage: $self [options] 1>&2
    echo "" 1>&2
    echo "    -c         enable console logging" 1>&2
    echo "    -d         enable debug level logging" 1>&2
    echo "    -i file    use the configuration file specified" 1>&2
    echo "    -h         this help message" 1>&2
    echo "    -r retries number of attempts to transmit all files" 1>&2
    echo "    -s server  the server name to transmit files to" 1>&2
    echo "    -t timeout number of seconds to wait before attempting to resend failed files" 1>&2
    echo "    -v         enable info level logging" 1>&2
    echo "    -w         DISABLE warning level logging" 1>&2
    echo "    -x timeout maximum number of seconds to attempt to send file for" 1>&2
    echo "    -z         enable compression (gzip) of files" 1>&2


    return 0
}   #   usage

while getopts ":cdhi:r:s:t:vwx:z" opt; do
    case $opt in
    c)
        LOG_CONSOLE=1
    ;;
    d)
        LOG_WARNING=1
        LOG_DEBUG=1
        MV_CP_VERBOSE_FLAG="-v"
    ;;
    i)
        INIFILE="$OPTARG"
    ;;
    h)
        usage;
        exit 2
    ;;
    r)
        RETRY_ATTEMPTS=$OPTARG
        case ${RETRY_ATTEMPTS} in
            (*[^0-9]*|'')
                echo "Option for -r must be a number. '$OPTARG' is not a number." >&2
                exit 1
            ;;
            (*)
                #Is a number do nothing.
            ;;
        esac
    ;;
    s)
        CURRENT_SERVER="$OPTARG"
    ;;
    t)
        RETRY_TIMEOUT_SEC=$OPTARG
        case ${RETRY_TIMEOUT_SEC} in
            (*[^0-9]*|'')
                echo "Option for -t must be a number. '$OPTARG' is not a number." >&2
                exit 1
            ;;
            (*)
                #Is a number do nothing.
            ;;
        esac
    ;;
    v)
        LOG_WARNING=1
        LOG_INFO=1
        MV_CP_VERBOSE_FLAG="-v"
    ;;
    w)
        LOG_WARNING=0
    ;;
    x)
        MAX_TRANSFER_TIME=$OPTARG
        case ${MAX_TRANSFER_TIME} in
            (*[^0-9]*|'')
                echo "Option for -t must be a number. '$OPTARG' is not a number." >&2
                exit 1
            ;;
            (*)
                #Is a number do nothing.
            ;;
        esac
    ;;
    z)
        COMPRESS_FILES=1
    ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        echo "run '$self -h' for help"
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
FILE_LOG=${LOG_DIR}/transfer_audit.log      #   Log file path (if set)
FILE_LOG_MAX_SIZE=1024000               #   Maximum log file size
AUDIT_CURRENT_DIR=${DATA_DIR}/audit/current
AUDIT_CURRENT_FILE=emvtransaction.csv
AUDIT_UPLOAD_DIR=${DATA_DIR}/audit/upload
AUDIT_ARCHIVE_DIR=${DATA_DIR}/audit/archive
AUDIT_ARCHIVE_DIR_MAX_SIZE=1024      #   Maximum size of archive directory in kB
AUDIT_COUNTER_FILE=${DATA_DIR}/audit/audit.counter

if [[ -z ${INIFILE} ]] ; then
    INIFILE=${ETC_DIR}/transfer_audit.ini
fi
FILE_COMMISSIONED=${ETC_DIR}/commissioned.ini

PRIVATE_KEY_FILE_DFLT=${ETC_DIR}/sftp_rsa.pem

CURRENT_SERVER_DFLT=qtem@tls
TRANSFER_FTPPUT_DFLT=curl
TRANSFER_FTPPUT_OPTIONS_DFLT="-m %MAX_TRANSFER_TIME% -C -"
TRANSFER_FTPPUT_USER_OPTIONS_DFLT="--key %PRIVATE_KEY_FILE%"
TRANSFER_PROTOCOL_DFLT=sftp
TRANSFER_PATH_DFLT=/TxnLog

#--------------------------------------------------------------------------------------------------
#
# Loggers
#
#--------------------------------------------------------------------------------------------------
LogOutput()
{
    log_msg="`date +'%b %d %H:%M:%S'` $self $*"

    if [[ ! -z ${FILE_LOG} ]] ; then
        echo "$log_msg" >> ${FILE_LOG}
    fi
    
    if [[ $LOG_CONSOLE -ne 0 ]] ; then
        echo "$log_msg"
    fi

    return 0

}   #   LogOutput

    #   Logs error message.
    #   $*      error message
    #
LogError()
{
    LogOutput "(E) $*"

}   #   LogError

    #   Logs warning message. If LOG_WARNING flag is set.
    #   $*      information message
    #
LogWarning()
{
    if [[ $LOG_WARNING -ne 0 ]] ; then
        LogOutput "(W) $*"
    fi

}   #   LogInfo

    #   Logs information message. If LOG_INFO flag is set.
    #   $*      information message
    #
LogInfo()
{
    if [[ $LOG_INFO -ne 0 ]] ; then
        LogOutput "(I) $*"
    fi

}   #   LogInfo

    #   Logs debugging message. If LOG_DEBUG flag is set.
    #   $*      debug message
    #
LogDebug()
{
    if [[ $LOG_DEBUG -ne 0 ]] ; then
        LogOutput "(D) $*"
    fi

}   #   LogDebug


#--------------------------------------------------------------------------------------------------
#
# Helper functions
#
#--------------------------------------------------------------------------------------------------

    #   Rotates log file if required
    #
RotateLog()
{

    if [[ ! -z ${FILE_LOG} ]] ; then
        FILE_LOG_SIZE=`stat -c %s ${FILE_LOG}`
        if [[ ${FILE_LOG_SIZE} -gt ${FILE_LOG_MAX_SIZE} ]] ; then
            rm -f ${FILE_LOG}.1
            mv -f ${MV_CP_VERBOSE_FLAG} ${FILE_LOG} ${FILE_LOG}.1
        fi
    fi

}   #   RotateLog

    #   Retrieves INI parameter value
    #   $1      INI file path
    #   $2      INI parameter name
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

    #   Converts local date/time to UTC in a compatible format to be used in filenames
    #   $1      locate date/time (YYYYMMDDThhmmssZ)
    #
Zuluify()
{

    UNAME_MACHINE=`uname -m | cut -c1-3`
    if [ "${UNAME_MACHINE}" = "arm" -o "${UNAME_MACHINE}" = "ppc" ]; then

        #   Cobra or Viper
        #
        ZULUIFY_TIME_T=`date -d "$1" -D %Y-%m-%dT%H:%M:%S +%s`
        ZULUIFY_RET=`date -u -d "$ZULUIFY_TIME_T" -D %s +%Y%m%dT%H%M%S`Z

    else

        ZULUIFY_TIME_T=`date -d "$1" +%s`
        ZULUIFY_RET=`date -u -d "@$ZULUIFY_TIME_T" +%Y%m%dT%H%M%S`Z

    fi

}   #   Zuluify


#--------------------------------------------------------------------------------------------------
#
# Setup functions
#
#--------------------------------------------------------------------------------------------------

    #   Check external commands are present in the path
    #
TestForTools()
{
##Taken from http://wiki.bash-hackers.org/scripting/style
    my_needed_commands="cat cut date du flock getini gzip id ls mv rm stat sed tee uname ${TRANSFER_FTPPUT}"

    missing_counter=0
    for needed_command in $my_needed_commands; do
      hash "$needed_command" >/dev/null 2>&1
      needed_command_exists=$?  #get return code from hash
      if [[ ${needed_command_exists} -ne 0 ]] ; then
        printf "Command not found in PATH: %s\n" "$needed_command" >&2
        missing_counter=$((missing_counter+1))
      fi
    done

    if [[ $missing_counter -gt 0 ]]; then
      printf "Minimum %d commands are missing in PATH, aborting\n" "$missing_counter" >&2
      exit 1
    fi

}   #   TestForTools

    #   Verifies the user this script runs under
    #   $1  user login name
CheckRunningAsUser()
{
        USER=$1
        USER_ID_CHECK=`grep ^${USER} /etc/passwd| cut -f 3 -d:`
        if [[ `/usr/bin/id -u` -eq ${USER_ID_CHECK} ]] ; then
                return
        fi
        LogError "CheckRunningAsUser : Must run as user ${USER} - not `id`.  Aborting".
        exit 1

}   #   CheckRunningAsUser

    #   Ensures only one instances of this script runs
CheckOnlyInstance()
{
   LogOutput "(V) transfer_audit.sh PID         : $$"

   if [[ -f "$PID_FILE" ]]; then
      if [[ -d /proc/`cat "$PID_FILE"` ]] ; then
         LogError "CheckOnlyInstance : Script already running, PID `cat "$PID_FILE"`"
         exit 0
      fi
   fi
   echo $$ > "$PID_FILE"

}   #   CheckOnlyInstance

    # Removes PID file for future runs
ClearOnlyInstance()
{
   rm -f "$PID_FILE"

}   #   ClearOnlyInstance

    #   Logs the script and run settings used for debug purposes
    #
LogThisRunSettings()
{
    LogOutput "(V) transfer_audit.sh Date        : $Date: 2015-12-20 13:55:14 +1100 (Sun, 20 Dec 2015) $"
    LogOutput "(V) transfer_audit.sh Revision    : $Revision: 87383 $"
    LogOutput "(V) transfer_audit.sh Log File    :'${FILE_LOG}'"
    LogOutput "(V) transfer_audit.sh Log size    : ${FILE_LOG_MAX_SIZE}"
    LogOutput "(V) transfer_audit.sh Upload dir  :'${AUDIT_UPLOAD_DIR}'"
    LogOutput "(V) transfer_audit.sh Archive dir :'${AUDIT_ARCHIVE_DIR}'"
    LogOutput "(V) transfer_audit.sh Archive size: ${AUDIT_ARCHIVE_DIR_MAX_SIZE}"
    LogOutput "(V) transfer_audit.sh Compress    : ${COMPRESS_FILES}"
    LogOutput "(V) transfer_audit.sh TLS server  : ${CURRENT_SERVER}"
    LogOutput "(V) transfer_audit.sh transport   : ${TRANSFER_FTPPUT}"
    LogOutput "(V) transfer_audit.sh private key : ${PRIVATE_KEY_FILE}"

}   #   LogThisRunSettings

    #   Creates or retrieves data from the file counter persistent state.   
    #
RetrieveFileCounter()
{
    if [[ -f "${AUDIT_COUNTER_FILE}" ]] ; then
        read AUDIT_FILE_COUNTER < "${AUDIT_COUNTER_FILE}"
    fi
    
}   #   RetrieveFileCounter

MakeFileNameGoodForWindows()
{
    # Issue with previous version where it would keep appending data inc date and esn to filename
    # If upload is unsuccessful for a period of time the filenames could be longer than permitted
    # Windows filename lenght 255 characters. Check for this and rename file if required.
    MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET="$1"
    MAKE_FILE_NAME_GOOD_FOR_WINDOWS_EXT="${MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET##*.}"

    if [[ ${#MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET} -gt 100 ]] ; then
        RAND_STRING_FOR_WINDOWS="`mktemp  -u | sed 's/.*\.//'`"
        # Truncate the name to 100 characters, add a random string, and add the extension back.
        MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET="${MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET:0:100}_${RAND_STRING_FOR_WINDOWS}.${MAKE_FILE_NAME_GOOD_FOR_WINDOWS_EXT}"

    fi

}

#--------------------------------------------------------------------------------------------------
#
# Worker functions
#
#--------------------------------------------------------------------------------------------------

    #   Moves the current audit file to the upload dir
MoveCurrentFile()
{
    if [[ -f "${AUDIT_CURRENT_DIR}/${AUDIT_CURRENT_FILE}" ]] ; then
        AUDIT_CURRENT_FILE_SANS_EXT="${AUDIT_CURRENT_FILE%.*}"
        AUDIT_CURRENT_FILE_EXT="${AUDIT_CURRENT_FILE##*.}"
        Zuluify "`date +%Y-%m-%dT%H:%M:%S`" 
        CURRENT_AUDIT_FILE_DT_NAME="${AUDIT_CURRENT_FILE_SANS_EXT}.${ZULUIFY_RET}.${ESN}.${AUDIT_FILE_COUNTER}.${AUDIT_CURRENT_FILE_EXT}"

        mv ${MV_CP_VERBOSE_FLAG} "${AUDIT_CURRENT_DIR}/${AUDIT_CURRENT_FILE}" "${AUDIT_UPLOAD_DIR}/${CURRENT_AUDIT_FILE_DT_NAME}"

        #increment the transfer file counter
        AUDIT_FILE_COUNTER=$((AUDIT_FILE_COUNTER+1))
    fi

}   #   MoveCurrentFile

    #   Transfers locks the file and sends a copy to the TLS
    #   on success moves the file to the archive directory
    #   on failure leaves the file in the upload directory
    #   $1  filename to transfer including path.
TransmitFile()
{
    LogInfo "Transmitting file: '$1'"

    TRANSFER_FILE_NAME="$1"
##    TRANSFER_FILE_NAME_SANS_EXT="${TRANSFER_FILE_NAME##*/}"             #Remove path to final / i.e. /remove/path/of/file.ext -> file.ext
##    TRANSFER_FILE_NAME_SANS_EXT="${TRANSFER_FILE_NAME_SANS_EXT%.*}"     #Remove ext for file name i.e.  file.rmExt -> file
##    TRANSFER_FILE_EXT="${TRANSFER_FILE_NAME##*.}"
    Zuluify "`date +%Y-%m-%dT%H:%M:%S`" 
    TRANSFER_FILE_CURRENT_DATETIME="${ZULUIFY_RET}"

    TRANSFER_FILE_LOCAL_NAME="${TRANSFER_FILE_NAME##*/}"                    #Remove path to final / i.e. /remove/path/of/file.ext -> file.ext

    if [[ ! -f "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}" ]] ; then
        LogError "Transmit file failed, not a file: '$TRANSFER_FILE_NAME'"
    else
        #################################################################################
        #                       START FLOCK
        #################################################################################
        LogInfo "About to lock ${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}"
        exec 8>>"${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}"
        if flock -x 8 ; then 
            # Lock the file using file descriptor 8.
            LogDebug "Locked ${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}"
  
            ## From a previous verion of this script the file name might be too long. make it shorter
            ## As the remote name.
            MakeFileNameGoodForWindows ${TRANSFER_FILE_LOCAL_NAME}
            TRANSFER_FILE_REMOTE_NAME=${MAKE_FILE_NAME_GOOD_FOR_WINDOWS_RET}

            TRANSFER_FILE_NAME_SANS_EXT="${TRANSFER_FILE_REMOTE_NAME%.*}"  #Remove ext from file name i.e. file.rmExt -> file
            TRANSFER_FILE_EXT="${TRANSFER_FILE_REMOTE_NAME##*.}"           #Retrieve the ext from file name i.e. file.ext -> ext

            #Only attempt to rename the file on the first attempt for this script run 
#           if [[ ${FIRST_SEND_ATTEMPT} -ne 0 ]] ; then
#               # Add date / time to file name
#               mv ${MV_CP_VERBOSE_FLAG} "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}" "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_DT_NAME}"
#           else
#               TRANSFER_FILE_DT_NAME=${TRANSFER_FILE_NAME}
#           fi

            if [[ $COMPRESS_FILES -ne 0 ]] ; then
                LogDebug "Compressing file"
                gzip -9 ${MV_CP_VERBOSE_FLAG} "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_DT_NAME}"
                #gzip doesn't append a new .gz if the file is already zipped.
                if [[ ! "${TRANSFER_FILE_EXT}" = "gz" ]] ; then
                    TRANSFER_FILE_LOCAL_NAME=${TRANSFER_FILE_LOCAL_NAME}.gz
                    TRANSFER_FILE_EXT="${TRANSFER_FILE_EXT}.gz"
                fi
            fi

            TRANSFER_FILE_REMOTE_NAME="${TRANSFER_FILE_NAME_SANS_EXT}-${TRANSFER_FILE_CURRENT_DATETIME}.${TRANSFER_FILE_EXT}"

            #Copy below line but replacing ${TRANSFER_FTPPUT_USER_OPTIONS} with some static text so user/pass doesn't show up in logs
            LogInfo "Executing: ${TRANSFER_FTPPUT} ${MV_CP_VERBOSE_FLAG} TRANSFER_FTPPUT_USER_OPTIONS ${TRANSFER_FTPPUT_OPTIONS} -T \"${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_LOCAL_NAME}\" \"${TRANSFER_PROTOCOL}://${CURRENT_SERVER}${TRANSFER_PATH}/${TRANSFER_FILE_REMOTE_NAME}\""
            ${TRANSFER_FTPPUT} ${MV_CP_VERBOSE_FLAG} ${TRANSFER_FTPPUT_USER_OPTIONS} ${TRANSFER_FTPPUT_OPTIONS} -T "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_LOCAL_NAME}" "${TRANSFER_PROTOCOL}://${CURRENT_SERVER}${TRANSFER_PATH}/${TRANSFER_FILE_REMOTE_NAME}"
            C_RET=$?
            LogDebug "Curl returned ($C_RET)"

            if [[ $C_RET -eq 0 ]] ; then
                #increment the success transfer file counter
                FILES_SUCCESS_TRANSFER=$((FILES_SUCCESS_TRANSFER+1))
                #move to archive dir.
                mv ${MV_CP_VERBOSE_FLAG} "${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_LOCAL_NAME}" "${AUDIT_ARCHIVE_DIR}/${TRANSFER_FILE_REMOTE_NAME}"
            else
                if [[ $C_RET -eq 51 ]] ; then
                    LogError "Unable to verify Peers fingerprint ensure --hostpubmd5 is supplied or the server is in known_hosts file."
                fi
                FILES_FAIL_TRANSFER=$((FILES_FAIL_TRANSFER+1))
            fi

            #Remove the lock
            flock -u 8
            LogDebug "Released lock of ${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_DT_NAME}"
        #################################################################################
        #                       END FLOCK
        #################################################################################
        else
            LogError "Failed to flock ${AUDIT_UPLOAD_DIR}/${TRANSFER_FILE_NAME}"
        fi
        exec 8>&-
    fi

}   #   TransmitFile

    #   Removes old files when the archive dir exceeds $AUDIT_ARCHIVE_DIR_MAX_SIZE
CleanArchiveDir()
{
    ARCHIVE_DIR_SIZE=`du -s ${AUDIT_ARCHIVE_DIR} | cut -f1`
    if [[ ${ARCHIVE_DIR_SIZE} -gt ${AUDIT_ARCHIVE_DIR_MAX_SIZE} ]] ; then
        LogInfo "Cleaning up archive dir. ${ARCHIVE_DIR_SIZE} > ${AUDIT_ARCHIVE_DIR_MAX_SIZE}"

        #Figure out unix time at the start of the day today.
        DATE_YYYYMMDD_LOCAL_TODAY=`date +%Y-%m-%d`
        DATE_TIME_T_LOCAL_TODAY=`date -d ${DATE_YYYYMMDD_LOCAL_TODAY} +%s`

        for rmfile in `ls -rt ${AUDIT_ARCHIVE_DIR}/` ; do
            if [[ -f "${AUDIT_ARCHIVE_DIR}/${rmfile}" ]] ; then
                if [[ `stat -c %Y "${AUDIT_ARCHIVE_DIR}/${rmfile}"` -ge ${DATE_TIME_T_LOCAL_TODAY} ]] ; then
                    LogInfo "Ignoring less than a day old. File: ${AUDIT_ARCHIVE_DIR}/${rmfile}"
                else
                    rm -f ${MV_CP_VERBOSE_FLAG} "${AUDIT_ARCHIVE_DIR}/${rmfile}"
                fi
            else
                LogDebug "CleanArchiveDir not a file: ${AUDIT_ARCHIVE_DIR}/${rmfile}"
            fi

            ARCHIVE_DIR_SIZE=`du -s ${AUDIT_ARCHIVE_DIR} | cut -f1`
            if [[ ${ARCHIVE_DIR_SIZE} -le ${AUDIT_ARCHIVE_DIR_MAX_SIZE} ]] ; then
                LogInfo "Deleted sufficient files from archive dir. ${ARCHIVE_DIR_SIZE} <= ${AUDIT_ARCHIVE_DIR_MAX_SIZE}"
                break
            fi
        done
    fi
    LogDebug "Archive dir. Actual: ${ARCHIVE_DIR_SIZE} Max Size: ${AUDIT_ARCHIVE_DIR_MAX_SIZE}"

}   #   CleanArchiveDir

#--------------------------------------------------------------------------------------------------
#
# Main
#
#--------------------------------------------------------------------------------------------------

test \! -f "$FILE_COMMISSIONED" && echo "Device is not commissioned." && exit 1 

GetIni "${FILE_COMMISSIONED}" General:TerminalId ; TERMINAL_ID="${INIENTRY}"
GetIni "${FILE_COMMISSIONED}" General:TerminalType ; TERMINAL_TYPE="${INIENTRY}"
GetIni "${FILE_COMMISSIONED}" General:ESN ; ESN="${INIENTRY}"

if [[ -z ${COMPRESS_FILES} ]]       ; then GetIni ${INIFILE} Audit:CompressFiles   ${COMPRESS_FILES_DFLT} ;       COMPRESS_FILES="${INIENTRY}";        fi
if [[ -z ${RETRY_TIMEOUT_SEC} ]]    ; then GetIni ${INIFILE} Audit:ResendTimeout   ${RETRY_TIMEOUT_SEC_DLFT} ;    RETRY_TIMEOUT_SEC="${INIENTRY}";     fi
if [[ -z ${RETRY_ATTEMPTS} ]]       ; then GetIni ${INIFILE} Audit:ResendAttempts  ${RETRY_ATTEMPTS_DFLT} ;       RETRY_ATTEMPTS="${INIENTRY}";        fi
if [[ -z ${MAX_TRANSFER_TIME} ]]    ; then GetIni ${INIFILE} Audit:MaxTransferTime ${MAX_TRANSFER_TIME_DFLT} ;    MAX_TRANSFER_TIME="${INIENTRY}";     fi

if [[ -z ${CURRENT_SERVER} ]]       ; then GetIni ${INIFILE} Audit:tlsServer      "${CURRENT_SERVER_DFLT}" ;      CURRENT_SERVER="${INIENTRY}";        fi
if [[ -z ${TRANSFER_FTPPUT} ]]      ; then GetIni ${INIFILE} Audit:transferApp    "${TRANSFER_FTPPUT_DFLT}" ;     TRANSFER_FTPPUT="${INIENTRY}";       fi
if [[ -z ${TRANSFER_FTPPUT_OPTIONS} ]] ; then GetIni ${INIFILE} Audit:transferOptions "${TRANSFER_FTPPUT_OPTIONS_DFLT}" ;  TRANSFER_FTPPUT_OPTIONS="${INIENTRY}";    fi
if [[ -z ${TRANSFER_FTPPUT_USER_OPTIONS} ]] ; then GetIni ${INIFILE} Audit:transferUserPass "${TRANSFER_FTPPUT_USER_OPTIONS_DFLT}" ;  TRANSFER_FTPPUT_USER_OPTIONS="${INIENTRY}";    fi
if [[ -z ${TRANSFER_PROTOCOL} ]]    ; then GetIni ${INIFILE} Audit:transferProtocol "${TRANSFER_PROTOCOL_DFLT}" ; TRANSFER_PROTOCOL="${INIENTRY}";     fi
if [[ -z ${TRANSFER_PATH} ]]        ; then GetIni ${INIFILE} Audit:transferPath   "${TRANSFER_PATH_DFLT}" ;       TRANSFER_PATH="${INIENTRY}";         fi
if [[ -z ${PRIVATE_KEY_FILE} ]]     ; then GetIni ${INIFILE} Audit:PrivateKey  "${PRIVATE_KEY_FILE_DFLT}" ;         PRIVATE_KEY_FILE="${INIENTRY}";         
fi
if [[ -z ${HOST_FINGERPRINT} ]]     ; then GetIni ${INIFILE} Audit:HostFingerprint "0" ; HOST_FINGERPRINT="${INIENTRY}"; fi

## TRANSFER_FTPPUT_OPTIONS may contain the user specified keywords which we are to replace
## with the corresponding variables. Supported variables:
##      %PRIVATE_KEY_FILE%
##      %MAX_TRANSFER_TIME%
##      %HOST_FINGERPRINT%
TRANSFER_FTPPUT_OPTIONS=`echo "${TRANSFER_FTPPUT_OPTIONS}" | sed -e 's|%PRIVATE_KEY_FILE%|'${PRIVATE_KEY_FILE}'|g' -e 's|%MAX_TRANSFER_TIME%|'${MAX_TRANSFER_TIME}'|g' -e 's|%HOST_FINGERPRINT%|'${HOST_FINGERPRINT}'|g'`
## Expect PRIVATE_KEY_FILE to be in user options. But parse all variables regardless.
TRANSFER_FTPPUT_USER_OPTIONS=`echo "${TRANSFER_FTPPUT_USER_OPTIONS}" | sed -e 's|%PRIVATE_KEY_FILE%|'${PRIVATE_KEY_FILE}'|g' -e 's|%MAX_TRANSFER_TIME%|'${MAX_TRANSFER_TIME}'|g' -e 's|%HOST_FINGERPRINT%|'${HOST_FINGERPRINT}'|g'`

TestForTools

#CheckRunningAsUser root
##TODO revert.
CheckOnlyInstance

LogThisRunSettings
RetrieveFileCounter

INITIAL_AUDIT_FILE_COUNT=${AUDIT_FILE_COUNTER}
MoveCurrentFile

FILES_SUCCESS_TRANSFER=0
FILES_FAIL_TRANSFER=1
#no DO loop so set to 1 to ensure executed at least once
while [ ${FILES_FAIL_TRANSFER} -ne 0 -a ${RETRY_ATTEMPTS} -gt 0 ] ; do
    FILES_FAIL_TRANSFER=0
    for uploadFile in `ls ${AUDIT_UPLOAD_DIR}/` ; do
        TransmitFile "${uploadFile}"
    done
    FIRST_SEND_ATTEMPT=0

    RETRY_ATTEMPTS=$((RETRY_ATTEMPTS-1))
    if [[ $FILES_FAIL_TRANSFER -ne 0 ]] ; then
        LogWarning "${FILES_FAIL_TRANSFER} file(s) failed to transfer. Waiting ${RETRY_TIMEOUT_SEC} seconds. Retries left ${RETRY_ATTEMPTS}"
        if [[ $RETRY_ATTEMPTS -gt 0 ]] ; then
            sleep ${RETRY_TIMEOUT_SEC}
        fi #Don't bother sleeping if we're going to exit anyway.
    fi
done

LogOutput "(V) Finished main processing FILES_FAIL_TRANSFER: ${FILES_FAIL_TRANSFER}. Remaining RETRY_ATTEMPTS: ${RETRY_ATTEMPTS}. TOTAL_FILE_SUCCESS_TRANSFER: ${FILES_SUCCESS_TRANSFER}. AUDIT_FILE_COUNTER: ${INITIAL_AUDIT_FILE_COUNT} ---> ${AUDIT_FILE_COUNTER}"

CleanArchiveDir
RotateLog                   #   Checks for log file rotation

ClearOnlyInstance

#Persist the count of files successfully transferred
echo ${AUDIT_FILE_COUNTER} > "${AUDIT_COUNTER_FILE}"

if [[ ${FILES_FAIL_TRANSFER} -ne 0 ]] ; then 
    exit 4
fi

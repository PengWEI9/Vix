#!/bin/sh
# Utility script - used to ensure that only a single instance of a given process
#                  is running at any one time.
#
# Arguments:
#  $1 - The first

SELF_NAME=$0
DEFAULT_WAIT_SECS=0

# Dumps the usage of this command to standard output and exits.
usage_exit()
{
    echo "Usage: ${SELF_NAME} -f <singleton_file1> [-w wait_secs1] [-F singleton_file2] [-W wait_secs2] command..."
    echo ""
    echo "Arguments:"
    echo "  -h                   Show this help message."
    echo "  -f <singleton_file1> The name of the file to lock before running the command."
    echo "                       The script waits for up to <wait_secs1> trying to lock this"
    echo "                       file; if it cannot be locked the command is not run and"
    echo "                       this script terminates."
    echo "  -w <wait_secs1>      The maximum number of seconds to wait for the"
    echo "                       <singleton_file1> to become available.  Defaults to ${DEFAULT_WAIT_SECS} seconds."
    echo "  -F <singleton_file2> The name of the second file to lock before running the command."
    echo "                       The script waits for up to <wait_secs2> trying to lock this"
    echo "                       file; if it cannot be locked the command is not run and"
    echo "                       this script terminates."
    echo "  -W <wait_secs2>      The maximum number of seconds to wait for the"
    echo "                       <singleton_file2> to become available.  Defaults to ${DEFAULT_WAIT_SECS} seconds."
    echo ""
    echo "When locking multiple files it is critical that all scripts lock then in exactly"
    echo "the same order or a deadlock can occur where no script makes progress."

    exit 2
}

# Setup logging functinos
if [ "${DSC_UNITTEST}" != "" ]; then
    # Unit test case - just write to standard out with 'echo'.
    log_err() { echo "[ERR ] " "$*"; }
    log_ok() { echo "[INFO] " "$*"; }
else
    # Normal case - write to system log.
    logger="/usr/bin/logger -t lockrun -s -p local3."
    log_err() { ${logger}err "$*"; }
    log_ok() { ${logger}info "$*"; }
fi

# Parse the command line arguments.
unset lock1
lock1_secs=${DEFAULT_WAIT_SECS}
unset lock2
lock2_secs=${DEFAULT_WAIT_SECS}
while getopts hf:w:F:W: name; do
    case ${name} in
        f)  lock1="${OPTARG}";;
        F)  lock2="${OPTARG}";;
        w)  lock1_secs="${OPTARG}";;
        W)  lock2_secs="${OPTARG}";;
        ?)  usage_exit;;
    esac
done
if [ "${lock1}" = "" ]; then
    usage_exit
fi
shift $((${OPTIND} - 1))
if [ $# -le 0 ]; then
    usage_exit
fi

(
    flock -n 9
    ec=$?

    # We check every second to see if we can take the lock until the lock
    # wait time has dropped to 0.
    to=${lock1_secs}
    while [ ${ec} -ne 0 ]; do
        if [ ${to} -le 0 ]; then
            log_err "Locked run unable to claim lock for ${lock1} in ${lock1_secs} seconds; command was: " "$@"
            exit 201
        fi
        to=$((to-1))
        sleep 1

        flock -n 9
        ec=$?
    done

    # Reaching here means we have claimed lock1.  Now we must claim lock2 if needed.
    #  - Note that the 9<&- causes the file descriptor to be closed *for the command*, not within
    #    this subshell.  We do this so that the lock is not inherited and held by any child
    #    processes.  However it will still be held until this sub-shell terminates.
    if [ "${lock2}" = "" ]; then
        # Lock2 is not required we can run the command directly.
        log_ok "Locked run holding ${lock1} to execute command: " "$@"
        "$@" 9<&-
        ec=$?
        log_ok "Locked run finished holding ${lock1} for command: " "$@"
        exit ${ec}
    else
        (
            flock -n 8
            ec=$?

            to=${lock2_secs}
            while [ ${ec} -ne 0 ]; do
                if [ ${to} -le 0 ]; then
                    log_err "Locked run unable to claim lock for ${lock2} in ${lock2_secs} seconds after ${lock1} already taken; command was: " "$@"
                    exit 202
                fi
                to=$((to-1))
                sleep 1

                flock -n 8
                ec=$?
            done

            # Reaching here means that we have both lock1 and lock2.  We can run the process.
            log_ok "Locked run holding ${lock1} and ${lock2} to execute command: " "$@"
            "$@" 9<&- 8<&-
            ec=$?
            log_ok "Locked run finished holding ${lock1} and ${lock2} for command: " "$@"
            exit ${ec}
        )8>${lock2}
        exit $?
    fi

)9>${lock1}
exit $?



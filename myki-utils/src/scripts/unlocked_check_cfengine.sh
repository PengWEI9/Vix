#!/bin/sh
#set -vx


PATH=$PATH:/bin:/usr/bin:/sbin:/usr/sbin:/afc/bin:/var/lib/cfengine3/bin:/var/lib/cfengine/bin
export PATH


LOG="/afc/log/checkcfe.`date +%Y%m%d`"
# Housekeep logs oldr than 2 days
find /afc/log -name checkcfe\* -mtime +2 -print|xargs rm -f 

log_error()
{
    log "ERROR:" "$*"
}

log_info()
{
    log "INFO:" "$*"
}

log()
{
    echo "`date` $*" >> $LOG
}

IfTimeToRun()
{
    RESULT=0
    MINUTE_NOW=`date +%M`
    case $TERMINAL_TYPE in
    BDC|TDC|TDCSecondary|FPDm)
        IPV4_ADDR=`ifconfig eth0|grep 'inet addr'|sed 's/^ \+inet addr:\([0-9.]\+\) .*/\1/'`
        LAST_BYTE=`echo "$IPV4_ADDR"|sed 's/^.*\.\([0-9]\+\)/\1/'`

        if ! echo "$LAST_BYTE"|egrep '^[0-9]{1,3}'; then
            log_error "Failed to get valid last byte of IP address, will continue without randomisation"
        fi
        # FPDm's have IP's from 192.168.0.1 (PTDC), 192.168.0.2 (STDC) and FPDm's from 192.168.0.8 thru to 0.27
        TARGET_GROUP=$((LAST_BYTE%4))
        TARGET_MINUTE=$((MINUTE_NOW%(TARGET_GROUP+10)))
        #log_info "TARGET_GROUP is $TARGET_GROUP, MINUTE_NOW is $MINUTE_NOW"
        # split into 4 groups based on last byte of ip
        if [ "$MINUTE_NOW" -eq 0 ] || [ "$TARGET_MINUTE" -ne 0 ]; then
            RESULT=1
            log_info "This node will run a check on the multiples of $((TARGET_GROUP+10)) minutes, now is $MINUTE_NOW"
        fi
    ;;

    *)
        #log_info "IfTimeToRun : TERMINAL_TYPE is $TERMINAL_TYPE, should run every 10 minutes"
        if [ "$MINUTE_NOW" -eq 0 ] || [ $((MINUTE_NOW%15)) -ne 0 ]; then
            RESULT=1
        fi
    esac
    return $RESULT
}

EnsureStopCFE()
{
    $CF_SCRIPT stop
    sleep 30
    killall -KILL cf-agent
    killall -KILL cf-execd
    killall -KILL cf-serverd
    killall -KILL cf-twin
}


AFC=/afc
AFC_ETC="$AFC/etc"
AFC_BIN="$AFC/bin"


COMMISSIONED_FILE="$AFC_ETC/commissioned.ini"
TERMINAL_TYPE=`$AFC_BIN/getini -f $COMMISSIONED_FILE General:TerminalType`
if [ -z "$TERMINAL_TYPE" ]; then
    log_error "Failed to determine TERMINAL_TYPE"
    exit 1
fi

if ! IfTimeToRun; then
    log_info "Exiting, not time to run yet"
    exit 0
fi

log_info "Starting $0 run now on a $TERMINAL_TYPE"

EXIT_RET=0
if uname -m|egrep 'armv5tejl|ppc'; then
    PSOPT=''
else
    PSOPT='-e'
fi
##################################
# MTU-770 forcecomms scripts are not running at boot
# skip the entire action if the bootup script "forcecomms" is running
if /bin/ps $PSOPT |grep -v grep|grep forcecomms; then
    log_info "Found forcecomms running, will not proceed"
    exit 0
fi
###################################
# for COBRA & viper nodes
CF_SCRIPT=/etc/init.d/init.d/cfengine3

if [ ! -x $CF_SCRIPT ]; then
    # for ubuntu nodes
    CF_SCRIPT=/etc/init.d/cfengine3
fi

log_info "Checking cfengine processes"
CFEXEC_UP=1
if ! /bin/ps $PSOPT |grep -v grep |grep cf-execd; then
    log_error "cf-execd not running"
    CFEXEC_UP=0
fi

CFSERVER_UP=1
if [ -f /etc/default/cfengine3 ]; then
    if egrep 'SERVERD=1' /etc/default/cfengine3 ; then
        if ! /bin/ps $PSOPT |grep -v grep |grep cf-serverd; then
            log_error "cf-serverd not running"
            CFSERVER_UP=0
        fi
    fi
fi

if [ $CFEXEC_UP -ne 1 ] || [ $CFSERVER_UP -ne 1 ]; then

    EnsureStopCFE
        
    log_error "CF-ENGINE status : cf-execd=$CFEXEC_UP, cf-serverd=$CFSERVER_UP, attempting restart"
    # remove the lock db file (it might be corrupted causing start to fail)
    rm -f /var/lib/cfengine3/state/cf_lock.db
    $CF_SCRIPT start
    RET=$?
    if [ $RET -ne 0 ]; then
        log_error "CF-ENGINE restart returned $RET"
    fi
fi

############ 
# Check validity of policies using a dry run of a small policy that requires
# comms with the policy hub on any agent node
###############
CF_VERS_3_5="3.5"
CF_VERS_INSTALLED=`dpkg -l|grep cfengine|head -1|awk '{ print $3; }'`
CF_WORKDIR="/var/cfengine"
CFB_OPT=
if dpkg --compare-versions "$CF_VERS_INSTALLED" lt "$CF_VERS_3_5"; then
    CF_WORKDIR="/var/lib/cfengine3"
    CFB_OPT='-s'
fi

DC_SWAPPED=N
case $TERMINAL_TYPE in
TDCSecondary|FPDm)
    FILE_HWADDR_DC="$AFC_ETC/hwaddr_dc.dat"
    HWADDR_DC=`arp -i eth0 dc|sed 's/.*at \([0-9A-F:]\+\) .*eth0/\1/'`
    echo "$HWADDR_DC"|egrep '^[0-9A-F][0-9A-F]:[0-9A-F][0-9A-F]:[0-9A-F][0-9A-F]:[0-9A-F][0-9A-F]:[0-9A-F][0-9A-F]:[0-9A-F][0-9A-F]$'
    VALID_HWADDR=$?
    if [ -z "$HWADDR_DC" ] || [ "$VALID_HWADDR" -ne 0 ]; then
        log_error "Failed to obtain HW adress of DC on terminal type $TERMINAL_TYPE, got <HWADDR_DC>"
    fi
    if [ ! -f "$FILE_HWADDR_DC" ]; then
        echo "$HWADDR_DC" > "$FILE_HWADDR_DC"
    else
        PREVIOUS_HWADDR_DC=`cat "$FILE_HWADDR_DC"`
        if [ "$PREVIOUS_HWADDR_DC" != "$HWADDR_DC" ]; then
            log_info "Previous hw address of dc was <$PREVIOUS_HWADDR_DC>, current hw address is <$HWADDR_DC>"
            log_info "It appears DC has been swapped out, should re-bootstrap cfengine"
            DC_SWAPPED=Y
        else
            log_info "Previous hw address of dc was <$PREVIOUS_HWADDR_DC>, current hw address is <$HWADDR_DC>"
        fi
    fi
    ;;
*)
    log_info "TERMINAL_TYPE is $TERMINAL_TYPE, skipping DC swapped check"
    exit 0
    ;;
esac

log_info "Checking if DC hub has been swapped"
CF_HUB=`cat $CF_WORKDIR/policy_server.dat`
if [ -z "$CF_HUB" ]; then
    CF_HUB='nexus'
fi


if [ "$DC_SWAPPED" = "Y" ]; then
    EnsureStopCFE
    # Let's try and keep the cf db updated, but if this fails, blow away the keys anyway
    if [ -f $CF_WORKDIR/policy_server.dat ]; then
        cf-key --force-removal --remove-keys "$CF_HUB"
    fi
    rm -f /var/lib/cfengine3/ppkeys/root-*
    rm -f /var/cfengine/ppkeys/root-*
    log_info "Attempting re-bootstrap with policy hub as hub has been swapped"
    cf-agent -B  $CFB_OPT "$CF_HUB"
    BOOTSTRAP_STATE=$?
    if [ $BOOTSTRAP_STATE -ne 0 ] ; then
        log_error "re-bootstrap to $CF_HUB failed, returned $BOOTSTRAP_STATE, will attempt restore from backup copy"
        # try to restore from backup folder (if any)
        if [ -d $CF_WORKDIR/backup ]; then
            cp -Rp "$CF_WORKDIR"/backup/* "$CF_WORKDIR"/inputs
            RESTORE_STATE=$?
            if [ $RESTORE_STATE -ne 0 ]; then
                log_error "restoration of inputs folder failed with status $RESTORE_STATE"
                EXIT_RET=1
            else
                log_info "Restored policies from $CF_WORKDIR/backup to $CF_WORKDIR/inputs"
            fi
        else
            log_error "Re-bootstrap with hub failed, No backup folder to restore policies from"
            EXIT_RET=1
        fi
    else
        log_error "re-bootstrap to $CF_HUB succesfull, returned $BOOTSTRAP_STATE"
        if [ "$DC_SWAPPED" = "Y" ]; then
            log_info "Updating DC hw address in $FILE_HWADDR_DC"
            # update hw addr of dc now that we bootstrapped succesfully
            echo "$HWADDR_DC" > "$FILE_HWADDR_DC"
        fi
    fi
fi
"$CF_SCRIPT" start
exit $EXIT_RET


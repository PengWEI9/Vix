#!/bin/sh
#
# Gets info on ITF kits.
#
# Copyright Vix Technology 2015-Present
# @author benjamin.brcan@vixtechnology.com

KIT1_IP=10.96.32.14
KIT2_IP=10.97.254.37
KIT3_IP=10.96.32.13

KIT1_NUM_FPDM=4
KIT2_NUM_FPDM=2
KIT3_NUM_FPDM=2

CHECK_ALL=true # enable by default, unless a kit is specified as an argument
CHECK_KIT1=false
CHECK_KIT2=false
CHECK_KIT3=false

KIT1_SUBNET=0
KIT2_SUBNET=2
KIT3_SUBNET=3

# - UserKnownHostsFile=/dev/null - Stops ssh from storing ssh identity keys, so
#   we can prevent possible identity mismatch errors.
# - StrictHostKeyChecking=no - always accept first-time connections.
# - LogLevel=ERROR - Hides unnecessary warnings, messages, etc.
SSH_OPTIONS="-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no \
    -o LogLevel=ERROR"

SSH_KEY=~/.ssh/kit_rsa

TRY_HELP_STR="Try 'device_check.sh --help' for more information."

# First in FPDM host sequence, ie 8, 9, 10, 11, etc...
FIRST_FPDM_HOST=8

#
# Starts an SSH session on the remote server. This is so that we can keep
# connecting to the server without having to start new connections each time.
#
ssh_session_start() {
    local jump_host="$1"; shift
    local target_host="$1"; shift

    local port=22
    local socket="/tmp/root@$target_host:$port"

    if [ ! -z "$socket" ]; then
        ssh -f -N -M -S $socket -i $SSH_KEY $SSH_OPTIONS \
            -o ProxyCommand="ssh -q -i $SSH_KEY root@$jump_host \
            nc $target_host $port" root@$target_host
    fi
}

#
# Ends the session to the remote server started via ssh_session_start().
#
ssh_session_end() {
    local jump_host="$1"; shift
    local target_host="$1"; shift

    local socket="/tmp/root@$target_host:$port"

    if [ -z "$socket" ]; then
        ssh -S $socket -O exit -o ProxyCommand="ssh -q -i $SSH_KEY \
            root@$jump_host nc $target_host 22" root@$target_host
    fi
}

#
# SSH to a host via a jump host.
#
ssh_jump() {
    local jump_host="$1"; shift
    local target_host="$1"; shift
    local cmd="$1"; shift

    local socket="/tmp/root@$target_host:$port"
    local socket_option=""

    if [ -z "$socket" ]; then
        local socket_option="-S $socket"
    fi

    ssh $socket_option -i $SSH_KEY $SSH_OPTIONS -o ProxyCommand="ssh -q \
        -i $SSH_KEY root@$jump_host nc $target_host 22" root@$target_host "$cmd"
}

#
# SCP to/from a host via a jump host.
#
scp_jump() {
    local jump_host="$1"; shift
    local target_host="$1"; shift
    local source="$1"; shift
    local destination="$1"; shift

    local port=22
    local socket="/tmp/root@$target_host:$port"
    local socket_option=""

    if [ -z "$socket" ]; then
        local socket_option="-S $socket"
    fi

    scp $socket_option -r -i $SSH_KEY $SSH_OPTIONS -o ProxyCommand="ssh -q \
        -i $SSH_KEY root@$jump_host nc $target_host 22" \
        $source $destination 
}

#
# Prints details for actionlist and CD file on given device.
#
check_device_files() {
    local jump_host="$1"; shift
    local target_host="$1"; shift

    local cmd="ls -l /afc/data/alimport && ls -l /afc/data/cdimport"

    echo "$target_host:"

    ssh_jump $jump_host $target_host "$cmd" 2>&1 | grep -v "guid_list"
}

#
# Prints details for actionlists and CD files for devices in kit.
#
check_kit_files() {
    local primary_ip="$1"; shift
    local subnet="$1"; shift
    local num_fpdms="$1"; shift

    # check primary (self)
    check_device_files $primary_ip 192.168.${subnet}.1

    # check the secondary
    check_device_files $primary_ip 192.168.${subnet}.2

    local last_fpdm_host=`expr $FIRST_FPDM_HOST + $num_fpdms - 1`

    # check all the FPDMs
    for i in `seq $FIRST_FPDM_HOST $last_fpdm_host`; do
        check_device_files $primary_ip 192.168.${subnet}.$i
    done
}

#
# Downloads all the log files for a given device.
#
download_device_logs() {
    local jump_host="$1"; shift
    local target_host="$1"; shift
    local destination="$1"; shift

    local destination="$destination/$target_host"

    mkdir -p $destination

    echo "$target_host:"

    local logfile="logs_`date +%Y%m%d%H%M%S`.tgz"

    # start session, so the following ssh & scp commands don't each create 
    # individual sessions.
    ssh_session_start $jump_host $target_host

    # compress log files 
    ssh_jump $jump_host $target_host "find /afc/log -type f \
        ! -name '*[[:digit:]]' | xargs tar -czf $logfile"

    # download log files
    scp_jump $jump_host $target_host root@$target_host:$logfile $destination

    # remove the log file on remote host
    ssh_jump $jump_host $target_host "rm $logfile"

    # end session
    ssh_session_end $jump_host $target_host

}

#
# Downloads all the log files for a given kit.
#
download_kit_logs() {
    local primary_ip="$1"; shift
    local subnet="$1"; shift
    local num_fpdms="$1"; shift
    local destination="$1"; shift

    local destination="$destination/$primary_ip"
    mkdir -p $destination

    # check primary (self)
    download_device_logs $primary_ip 192.168.${subnet}.1 $destination

    # check the secondary
    download_device_logs $primary_ip 192.168.${subnet}.2 $destination

    local last_fpdm_host=`expr $FIRST_FPDM_HOST + $num_fpdms - 1`

    # check all the FPDMs
    for i in `seq $FIRST_FPDM_HOST $last_fpdm_host`; do
        download_device_logs $primary_ip 192.168.${subnet}.$i $destination
    done
}

#
# Prints info on actionlist/CD files on all devices on all kits.
#
check_all_kit_files() {

    if [ "$CHECK_ALL" = true -o "$CHECK_KIT1" = true ]; then
        check_kit_files $KIT1_IP $KIT1_SUBNET $KIT1_NUM_FPDM
    fi

    if [ "$CHECK_ALL" = true -o "$CHECK_KIT2" = true ]; then
        check_kit_files $KIT2_IP $KIT2_SUBNET $KIT2_NUM_FPDM
    fi

    if [ "$CHECK_ALL" = true -o "$CHECK_KIT3" = true ]; then
        check_kit_files $KIT1_IP $KIT3_SUBNET $KIT3_NUM_FPDM
    fi
}

#
# Downloads all the logs for all devices on all kits.
#
download_all_kit_logs() {
    local destination="$1"; shift

    if [ "$CHECK_ALL" = true -o "$CHECK_KIT1" = true ]; then
        download_kit_logs $KIT1_IP $KIT1_SUBNET $KIT1_NUM_FPDM "$destination"
    fi
    if [ "$CHECK_ALL" = true -o "$CHECK_KIT2" = true ]; then
        download_kit_logs $KIT2_IP $KIT2_SUBNET $KIT2_NUM_FPDM "$destination"
    fi
    if [ "$CHECK_ALL" = true -o "$CHECK_KIT3" = true ]; then
        download_kit_logs $KIT3_IP $KIT3_SUBNET $KIT3_NUM_FPDM "$destination"
    fi
}

print_help() {

    echo "Usage: device_check.sh [options] [command]"
    echo ""
    echo "Get info on ITF kits."
    echo ""
    echo "If no kits are specified, all will be selected by default."
    echo ""
    echo "Options:"
    echo "-k1, -k2, -k3 Perform action on kit 1, kit 2, or kit 3 respectively."
    echo ""
    echo "Commands:"
    echo "-h, --help       Show this help message and exit"
    echo "--check-files    Prints info on action list and CD files for kits"
    echo "--download-logs  Downloads all the log files from kits"
}

main() {

    if [ -z "$1" ]; then
        echo "No arguments provided."
        echo "$TRY_HELP_STR"
    fi

    while [ ! -z "$1" ]; do
        case $1 in
            -k1)
                CHECK_KIT1=true
                CHECK_ALL=false
                ;;
            -k2)
                CHECK_KIT2=true
                CHECK_ALL=false
                ;;
            -k3)
                CHECK_KIT3=true
                CHECK_ALL=false
                ;;
            --check-files)
                check_all_kit_files
                exit 0
                ;;
            --download-logs)
                shift
                local destination=$1

                if [ -z "$destination" ]; then
                    echo "Missing destination path."
                    echo "$TRY_HELP_STR"
                    exit 1
                fi

                download_all_kit_logs "$destination"
                exit 0
                ;;
            -h|--help)
                print_help
                exit 0
                ;;
            *)
                echo "Invalid option provided."
                echo "$TRY_HELP_STR"
                exit 0
                ;;
        esac

        shift
    done
}

main "$@"

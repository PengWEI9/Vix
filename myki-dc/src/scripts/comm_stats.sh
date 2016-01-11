#!/bin/sh
#
#Check /var/cfengine/comms_reports/ directory & last latest export from switch.
#Pick the latest one & update the lastsuccesscomms file with the time.
#
##########################################################    
CF_LAST_COMMS=0
SWITCH_LAST_COMMS=0
CF_COMMS_REPORTS="/var/cfengine/comms_reports"
if [ -d $CF_COMMS_REPORTS ]; then
    CF_LATTEST=`ls -tr /var/cfengine/comms_reports/*|tail -1`
    CF_ITEM_NAME=`basename $CF_LATTEST`
    CF_LAST_COMMS=`/bin/stat -c %Y $CF_LATTEST`
fi

if [ -r /tmp/switch.pipe -a -x /afc/bin/cmdutil ]; then
    SWITCH_LAST_COMMS=`/afc/bin/cmdutil -m -c "totals lastexport" /tmp/switch.pipe`
fi

if [ $CF_LAST_COMMS -gt $SWITCH_LAST_COMMS ]; then
    echo $CF_LAST_COMMS > /afc/commstatus/lastsuccesscomms
    echo $CF_ITEM_NAME > /afc/commstatus/lastcommsitem
else
    echo $SWITCH_LAST_COMMS > /afc/commstatus/lastsuccesscomms
    echo "Transferred LDT/Alarms" > /afc/commstatus/lastcommsitem
fi

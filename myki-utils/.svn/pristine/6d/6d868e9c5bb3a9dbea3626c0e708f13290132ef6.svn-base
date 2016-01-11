#!/bin/bash
####################################################################################################

/afc/bin/cmdutil -m -c "totals unsent" /tmp/switch.pipe > /afc/data/unsentfilecount
##
## On reboot / restart of switch "totals lastexport" is reset to zero.
## Never overwrite the last comms file with zero
LASTCOMMS=`/afc/bin/cmdutil -m -c "totals lastexport" /tmp/switch.pipe`

if [[ $LASTCOMMS -eq 0 ]] ; then
#    echo "Last comms is $LASTCOMMS - ignoring"
    touch /afc/data/lastcomms
else
    echo "$LASTCOMMS" > /afc/data/lastcomms
fi


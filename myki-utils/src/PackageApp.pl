########################################################################
# Copyright (C) 2009 ERG Limited, All rights reserved
#
# Module name   : PackageApp
# Module type   : Makefile system
# Compiler(s)   : n/a
# Environment(s): jats
#
# Description   : Script to be processed by the Debian Packager
#                 This script is not directly processed by JATS
#
#   This file is processed by the Debian Packager in order to construct
#   the package. It contains simple directives to construct the package
#
#......................................................................#

#
#   Specify the DEBIAN packaging files
#
PackageDescription ("myki_utils");
DebianFiles ('--PostInst=postinst');
DebianFiles ('--PreRm=prerm');

CopyBinFile ('getini', '/afc/bin', '--FromPackage');
CopyBinFile ('dbglog', '/afc/bin', '--FromPackage');
CopyBinFile ('cmdutil', '/afc/bin', '--FromPackage');
CopyBinFile ('ipc_clean', '/afc/bin', '--FromPackage');
CopyBinFile ('alarm_command', '/afc/bin', '--FromPackage');

CopyFile ('scripts/ch_emm.sh', '/afc/bin');
CopyFile ('scripts/ch_gac.sh', '/afc/bin');
CopyFile ('scripts/ch_reader.sh', '/afc/bin');
CopyFile ('scripts/check_cfengine.sh', '/afc/bin');
CopyFile ('scripts/clear_alarms.sh', '/afc/bin');
CopyFile ('scripts/check_install.sh', '/afc/bin');
CopyFile ('scripts/lockrun.sh', '/afc/bin');
CopyFile ('scripts/recover_data.sh', '/afc/bin');
CopyFile ('scripts/transfer_alarms.sh', '/afc/bin');
CopyFile ('scripts/transfer_audit.sh', '/afc/bin');
CopyFile ('scripts/transferud.sh', '/afc/bin');
CopyFile ('scripts/unlocked_check_cfengine.sh', '/afc/bin');
CopyFile ('scripts/unlocked_transfer_alarms.sh', '/afc/bin');
CopyFile ('scripts/unlocked_transferud.sh', '/afc/bin');
CopyFile ('scripts/unlocked_update_stats.sh', '/afc/bin');
CopyFile ('scripts/update_stats.sh', '/afc/bin');
CopyFile ('scripts/verify_ac.sh', '/afc/bin');

CopyFile ('etc/transfer_ud.ini', '/afc/etc');
CopyFile ('etc/utils.crontab', '/afc/etc');
CopyFile ('etc/transfer_alarms.ini', '/afc/etc');
CopyFile ('etc/transfer_audit.ini', '/afc/etc');
CopyFile ('etc/alarm_tmi_footer.tmpl', '/afc/etc', '--FromPackage');
CopyFile ('etc/alarm_tmi_header.tmpl', '/afc/etc', '--FromPackage');
CopyFile ('ipc_clean.pmi', '/afc/procmgr');
CopyFile ('clear_alarms.pmi', '/afc/procmgr');
CopyFile ('verify_ac.pmi', '/afc/procmgr');
CopyFile ('afc_lib.conf', '/etc/ld.so.conf.d');

# Cobra is missing this:
CreateDir ( '/var/spool/cron/crontabs' );

CopyFile ('etc/tmifooter.tmpl', '/afc/etc');
CopyFile ('etc/tmihdr.tmpl', '/afc/etc');
CopyFile ('etc/tmiperiodcommitfooter.tmpl', '/afc/etc');
CopyFile ('etc/tmiperiodcommit.tmpl', '/afc/etc');
CopyFile ('etc/tmishiftopen.tmpl', '/afc/etc');
CopyFile ('etc/tmishiftclose.tmpl', '/afc/etc');
CopyFile ('etc/tmishift.tmpl', '/afc/etc');
CopyFile ('etc/tmiversionsnapshot.tmpl', '/afc/etc');

SetFilePerms('a+rx', '/afc/bin', '--Recurse');

# Finalise install by marking all files executable
#Message ("Mark files as executable");

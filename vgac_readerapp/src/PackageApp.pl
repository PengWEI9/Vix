# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#

# The name of the application.
PackageDescription ("Myki GAC Reader Application");
DebianFiles('--PreRm=scripts/prerm');
#DebianFiles('--Name=gacrreaderapp');
DebianFiles('--PostInst=scripts/postinst');

# create directory structure
CreateDir ( '/afc/reader' );
CreateDir ( '/afc/reader/UD' );
CreateDir ( '/afc/reader/UD/tmp' );
CreateDir ( '/afc/reader/UD/pending' );
CreateDir ( '/afc/reader/UD/sent' );
CreateDir ( '/afc/reader/pending' );
CreateDir ( '/afc/reader/installed' );
CreateDir ( '/afc/reader/misc' );
CreateDir ( '/afc/log' );
CreateDir ( '/afc/data' );
CreateDir ( '/afc/data/images' );
CreateDir ( '/afc/data/ldt' );
CreateDir ( '/afc/data/alarm/confirmed' );
CreateDir ( '/afc/data/alarm/pending' );
CreateDir ( '/afc/data/ldt/confirmed' );
CreateDir ( '/afc/data/ldt/pending' );
CreateDir ( '/afc/procmgr' );

# Lists the files that are installed as part of the application.
CopyBinFile ( 'gacreaderapp',     '/afc/bin' );
MakeSymLink ( '/afc/bin/gacreaderapp', '/afc/bin/readerapp' );
CopyBinFile ( 'log_client',    '/afc/bin' );
CopyBinFile ( 'xml2ini',      '/afc/bin', '--FromPackage' );
CopyBinFile ( 'import_cd.sh', '/afc/bin', '--FromPackage' );
SetFilePerms( 'a+rx','/afc/bin/import_cd.sh' );
CopyFile    ( 'scripts/ntpd_cb.sh',     '/afc/bin/' );
SetFilePerms( 'a+rx',                   '/afc/bin/ntpd_cb.sh' );

CopyBinFile ( 'pwrfailtimestamp',       '/afc/bin', '--FromPackage' ) if (IsPlatform('PPC_603E'));

CopyFile ( 'scripts/Melbourne',  '/afc/reader/misc' );
CopyFile ( 'scripts/etc/serviceproviders.json',  '/afc/data/config' );
CopyFile ( 'scripts/etc/roles.json',  '/afc/data/config' );
CopyFile ( 'scripts/bbsram.conf',  '/etc' );

# startup init.d scripts
CopyFile ( 'scripts/reader', '/etc/init.d/init.d' );
CopyFile ( 'scripts/ntpd', '/etc/init.d/init.d' );

# Start up script
AddInitScript ( 'scripts/ntpd', 81, 06 );

# startup permissions
SetFilePerms( 'a+rx','/etc/init.d/init.d/reader' );
SetFilePerms( 'a+rx','/etc/init.d/init.d/ntpd' );

# startup procmgr scripts
CopyFile ( 'scripts/gacreaderapp.pmi', '/afc/procmgr' );

#
# all the misc configuration file defaults to misc (so as not to overwrite anything there)
CopyFile ( 'scripts/INI/gacreaderapp.ini', '/afc/etc' );
CopyFile ( 'scripts/INI/tpe_default.ini', '/afc/etc' );
#CopyFile ( 'etc/productcfg.db', '/afc/reader/', '--FromPackage' );
#CopyFile ( 'etc/actionlist.db', '/afc/reader/', '--FromPackage' );

MakeSymLink ( '/afc/etc/gacreaderapp.ini', '/afc/etc/readerapp.ini' );
# this sets the localtime properly
# may need to copy in some locale stuff here
MakeSymLink ( '/afc/reader/misc/Melbourne', '/etc/localtime' );

# required by myki-tmi
CopyFile('etc/counter_manifest.json', '/afc/data/config', '--FromPackage');

# Copyright (C) 2013 Vix Products Pty Ltd, All Rights Reserved
#

# The name of the application.
PackageDescription ("Driver Console Application");
DebianFiles('--PreRm=scripts/prerm');
DebianFiles('--PostInst=scripts/postinst');

# create directory structure
CreateDir ( '/afc/bin' );
CreateDir ( '/afc/procmgr' );
CreateDir ( '/afc/log' );
CreateDir ( '/afc/etc' );
CreateDir ( '/afc/test' );
CreateDir ( '/afc/var' );
CreateDir ( '/afc/var/run' );
CreateDir ( '/afc/cores' );
CreateDir ( '/afc/commstatus' );
CreateDir ( '/afc/images' );

# Lists the files that are installed as part of the application.
CopyBinFile ( 'dc',       '/afc/bin' );

# startup init.d scripts
CopyFile ( 'scripts/init.d/dc', '/etc/init.d/init.d' ); 



# scripts
CopyFile ( 'scripts/comm_stats.sh', '/afc/bin' );

# startup procmgr scripts
CopyFile ( 'dc.pmi', '/afc/procmgr' );

#
# all the misc configuration file defaults to misc (so as not to overwrite anything there)
CopyFile ( 'etc/dc.ini',                '/afc/etc' );
CopyFile ( 'etc/serviceproviders.json', '/afc/etc' );
CopyFile ( 'etc/roles.json',            '/afc/etc' );
CopyFile ( 'etc/dc.crontab',		'/afc/etc' );

# images
CopyFile ( 'images/upgrade_splash.ppm', '/afc/images' );

SetFilePerms( 'a+rx', '/etc/init.d/init.d/dc' );
SetFilePerms( 'a+rx', '/afc/bin/comm_stats.sh' );

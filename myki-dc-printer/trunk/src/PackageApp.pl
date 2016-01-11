# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#

# The name of the application.
PackageDescription ("Myki DC Printer Application");
#DebianFiles('--PreRm=scripts/prerm');
#DebianFiles('--PostInst=scripts/postinst');

# create directory structure
CreateDir ( '/afc/bin' );
CreateDir ( '/afc/procmgr' );
CreateDir ( '/afc/log' );
CreateDir ( '/afc/etc' );


# Lists the files that are installed as part of the application.
CopyBinFile ( 'dcprinter',  '/afc/bin' );

CopyFile ( 'Receipts.json', '/afc/etc' );
# startup init.d scripts
CopyFile ( 'scripts/dcprinter', '/etc/init.d/init.d' );
SetFilePerms( 'a+rx','/etc/init.d/init.d/dcprinter' );

# startup procmgr scripts
CopyFile ( 'dcprinter.pmi', '/afc/procmgr' );

#
# all the misc configuration file defaults to misc (so as not to overwrite anything there)
CopyFile ( 'etc/dcprinter.ini', '/afc/etc' );


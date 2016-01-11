# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved #
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";


# Define the set of platforms that this component is built for.  By
# defining them as '*' we use the same platforms as defined in the
# build.pl file.
SubDir      ('test');
SubDir      ('exceptions');
Platform    ( '*' );

# These are flags passed to the C/C++ compiler.  The flags specified
# here are required for building QT applications.
AddFlags    ('*', '-D_REENTRANT'    );

AddFlags    ('COBRA', '-DCOBRA' );
AddFlags    ('*', '-DNOCRYPT' );

#define a module and a level in each makefile
AddFlags    ('*', '-DMODULE=COMMON' );
AddFlags    ('*', '-DCSDEBUG=9' );

Src('*', 'backupmanager.h', 
        'filebackupmanager.h',
        'bbrambackupmanager.h');

InstallHdr('*', '--Prefix=backupmanager', @CHDRS);
PackageHdr('*', '--Prefix=backupmanager', @CHDRS);

# Boiler-plate for the build system, do not modify.
MakefileGenerate(); 1;

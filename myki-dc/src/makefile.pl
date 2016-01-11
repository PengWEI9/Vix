# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

SubDir('txnlist');
SubDir('printerbackup');

# Define the set of platforms that this component is built for.  By
# defining them as '*' we use the same platforms as defined in the
# build.pl file.
Platform    ( '*' );

#
#   Platform-independent modules
#

AddFlags    ('*', '-DLWS_NO_EXTENSIONS' );
AddFlags    ('*', '-D_DEBUG' );
AddFlags    ('UBUNTU12', '-D__HOS_LINUX__' );
AddFlags    ('LINUX_I386', '-D__HOS_LINUX__' );
AddFlags    ('PPC_603E', '-D__HOS_LINUX__' );
AddFlags    ('COBRA', '-D__HOS_LINUX__' );


################################################################################
#
#   Build the Library
#
################################################################################

InstallHdr( '*', 'dc.h');
AddSrcDir('*', 'messages');

Prog        ( '*', 'dc', 
             'main.cpp',
             'productsales.cpp',
             'productsalesreversal.cpp',
             'dc.cpp',
             'CCEEventProcessingThread.cpp',
             'WebsocketProcessingThread.cpp',
             'myki_time_conversion.c',
             'pwdofday.cpp',
             'utils.cpp',
             'version.c', '-llibwebsockets', '-Lpthread',  '-Lstdc++', '-ljsoncpp',
             '-lCceApi', '-lpil', '-lmbugate-x',
             '-lreader_common',
             '-lMyki_alarm',
             '-lMyki_CD', 
             '-lcommonlib',
             '-ldc_txnlist',
             '-ldc_printerbackup',
             '-lcrc',
             '-lmyki_schedule',
             '-ldatertswis',  
             '-laserpent-util',            
             '-laserpent-util',
             '-laserpent-fram',
             '-laserpent-sam',
             '-Ldl',             
             '-lcsf',
             '-lcsx',
             '-lcs',
             '-losa',
             '--MultiScan');


#InstallProg( '*', 'dc' );


Prog        ( '*', 'potd', 
             'potd.cpp',
             'pwdofday.cpp',
             '-Lpthread',  
             '-Lstdc++',
             '-lcsf',
             '-lcsx',
             '-lcs',         
             '-llibwebsockets', # For MD5
             '--MultiScan'
             );
             
#InstallProg( '*', 'potd' );

################################################################################
#
#   Package the Library
#
################################################################################

#   Package-internal libraries and headers

InstallLib( '*', @LIBS );
InstallHdr( '*', @CHDRS );
#InstallProg ( '*', @PROGS );


#   Package-external (ie, API) libraries and headers

PackageLib( '*', @LIBS );
PackageProg( '*', @PROGS );



################################################################################
#
#   Create the debian package
#
################################################################################

MakeDebianPackage ('*', '--Script=PackageApp.pl' );

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

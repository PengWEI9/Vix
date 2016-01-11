# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";


# Define the set of platforms that this component is built for.  By
# defining them as '*' we use the same platforms as defined in the
# build.pl file.
SubDir      ( 'backupmanager' );
Platform    ( '*' );

# These are flags passed to the C/C++ compiler.  The flags specified
# here are required for building QT applications.
AddFlags    ('*', '-D_REENTRANT'    );

AddFlags    ('COBRA', '-DCOBRA' );
AddFlags    ('*', '-DNOCRYPT' );

#define a module and a level in each makefile
AddFlags    ('*', '-DMODULE=COMMON' );


AddFlags    ('*', '-DCSDEBUG=9' );

AddSrcDir('*', 'backupmanager');

Src			( '*', 'CommonConversion.cpp' );
Src			( '*', 'CommonFunctions.cpp' );
Src			( '*', 'CRC.cpp' );
Src			( '*', 'cmd.cpp' );
Src			( '*', 'CommonTimer.cpp' );
Src			( '*', 'inifile.c' );
Src			( '*', 'ioapi.c' );
Src			( '*', 'crc32.c' );
Src         ( '*', 'zip.c' );
Src	     ( '*', 'zip_api.cpp' );
Src         ( '*', 'msg_init.cc' );
Src         ( '*', 'ipc_send.cc' );
Src         ( '*', 'ipc_flush.cc' );
Src         ( '*', 'ipc_util.cc' );
Src         ( '*', 'msg_crea.cc' );
Src         ( '*', 'msg_open.cc' );
Src         ( '*', 'msg_clos.cc' );
Src         ( '*', 'msg_msg.cc' );
Src         ( '*', 'msg_get.cc' );
Src         ( '*', 'msg_ctl.cc' );
Src         ( '*', 'msg_join.cc' );
Src         ( '*', 'msg_snd.cc' );
Src         ( '*', 'msg_rcv.cc' );
#Src         ( '*', 'olsmsg.h' );
Src         ( '*', 'CommonConversion.h' );
Src         ( '*', 'CommonFunctions.h' );
Src         ( '*', 'CRC.h' );
Src         ( '*', 'cmd.h' );
Src         ( '*', 'CommonTimer.h' );
Src         ( '*', 'inifile.h' );
Src         ( '*', 'zip_api.h' );
Src         ( '*', 'ioapi.h' );
Src         ( '*', 'crc32.h' );
Src         ( '*', 'zip.h' );
Src         ( '*', 'crypt.h' );
Src         ( '*', 'ipc.h' );
Src         ( '*', 'message_base.h' );
Src         ( '*', 'message_assert.h' );
Src         ( '*', 'signals.h' );
Src         ( '*', 'signals.cpp' );
Src         ( '*', 'comm.h' );
Src         ( '*', 'comm_linux.cpp' );
Src         ( '*', 'filebackupmanager.cpp' );
Src         ( 'COBRA, PPC_603E, UBUNTU12', 'bbrambackupmanager.cpp' );

# Currently serpent-api is currently built for COBRA, PPC and UBUNTU
Src         ( 'COBRA, PPC_603E, UBUNTU12', 'Serial.h' );
Src         ( 'COBRA, PPC_603E, UBUNTU12', 'Serial.cpp' );

Prog( '*', 'ipc_clean', 'ipc_clean.c',
			'-lcs',
			'-lreader_common',
			'-Lpthread',
			'-Lstdc++',
			'--MultiScan');

Prog( '*', 'ipc_test', 'ipc_test.c',
			'-lcs',
			'-lreader_common',
			'-Lpthread',
			'-Lstdc++',
			'--MultiScan');

Lib        ( '*'    , "reader_common", @OBJS  );
InstallLib ( '*'    , "reader_common" );
InstallHdr ( '*'    , @CHDRS ,"--Strip" );
PackageHdr  ( '*', @CHDRS ,"--Strip" );
PackageLib  ( '*', @LIBS );
PackageProg( '*', 'ipc_clean');

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

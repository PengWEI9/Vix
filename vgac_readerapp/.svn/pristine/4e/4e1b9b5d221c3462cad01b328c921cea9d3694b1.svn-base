# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

SubDir( 'cli' );
SubDir( 'userid' );

# build for cobra and pc platforms
Platform    (   'COBRA' , 'LINUX_I386', 'UBUNTU12', 'PPC_603E' );

# These are flags passed to the C/C++ compiler.  The flags specified
# here are required for building QT applications.
AddFlags    (   '*',        '-D_REENTRANT' );
AddFlags    (   'COBRA',    '-DCOBRA' );
AddFlags    (   'PPC_603E', '-DVIPER' );
AddFlags    (   '*',        '-DDAF_TIMINGPOINT' );

# trigger the compile guards in the imported ipc_monitor.cpp, to disable handling of
# some IPC messages (that are not  required [and are not found in dc-messages package])
AddFlags    (   '*',        '-DIS_GAC_READER' );	

AddSrcDir   (   '*',        'userid' );

Src         (   '*',        'version.h' );
Src         (   '*',        'persistentuserid.cpp' );
Src         (   '*',        'gac_processing_thread.cpp' );
Src         (   '*',        'ipc_monitor.cpp', '--FromPackage' );
Src         (   '*',        'easywsclient.cpp' );
Src         (   '*',        'datastring.h',         'datastring.cpp' );
Src         (   '*',        'datacurl.h',           'datacurl.cpp' );
Src         (   '*',        'dataws.h',             'dataws.cpp' );
Src         (   '*',        'dataxfer.h',           'dataxfer.cpp' );
Src         (   '*',        'gentmi.h',             'gentmi.cpp' );
Src         (   '*',        'cardfunctions.h',      'cardfunctions.cpp' );
Src         (   '*',        'wsshift.h',            'wsshift.cpp' );
Src         (   '*',        'wssales.h',            'wssales.cpp' );
Src         (   '*',        'roles.h',              'roles.cpp' );
Src         (   '*',        'serviceproviders.h',   'serviceproviders.cpp' );
Src         (   '*',        'cardsnapshot.h',       'cardsnapshot.cpp' );
Src         (   '*',        'ipcextend.cpp' );

Lib         (   '*',
                'version',  'version.c' );

# Headers to install
InstallHdr  (   '*',        'version.h',
                            'datastring.h',
                            'dataxfer.h',
                            'cardsnapshot.h' );

# Libraries to install
InstallLib( '*', 'version' );

Prog        (   '*',
                'log_client',
                    'log_client.cpp',
                    '-Lstdc++'
            );

Prog        (   'UBUNTU12',
                'log_client', 
                    '-Lm',
            );


Prog        (   '!UBUNTU12',
                'gacreaderapp',
                    @OBJS,
                    '-Lboost_filesystem',
                    '-Lboost_system',
                    '-lreader',
                    '-Lrt',
                    '-lversion',
                    '-lreader_common',
                    '-lud',
                    '-lud_api',
                    # '-llogserver',
                    '-lcli',
                    '-lMyki_BR',
                    '-lMyki_CD', 
                    '-lMyki_LDT',
                    '-lMyki_alarm',
                    '-lmyki-tmi',
                    '-lcommonlib',
                    '-lcsf',
                    '-lcsx',
                    '-lcs',
                    '-losa',
                    '-ltimingpoint',
                    '-lct_viper_mcr_comm',
                    '-lct_viper_mcrmulti_comm',
                    '-lmcr_host_protocol',
                    '-ldaf_ct_unified',
                    '-ldaf_ct_unified_mcr_driver',
                    '-ldaf_myki_cardservices',
                    '-ldaf_myki_crypto',
                    '-ldaf_myki_fs',
                    '-ldaf_utils_date_time',
                    '-ldaf_utils_bitstream',
                    '-ldaf_utils_serial_comms',
                    '-ldaf_utils_timer',
                    '-ldaf_desfire',
                    '-lbertlv',
                    '-ldaf_osa_mos',
                    '-llinkInterfaceDevice',
                    '-lcrypto-lock',
                    '-ldatertswis',
                    '-Lstdc++',
                    '-Lpthread',
                    '-Ldl',
                    '-lcrc',
                    '-Lssl',
                    '-Lcrypto',
                    '-Lcurl',
                    '-Lz',
                '--MultiScan'
            );

Prog        (   'UBUNTU12',
                'gacreaderapp',
                    @OBJS,
                    '-Lboost_filesystem',
                    '-Lboost_system',
                    '-lreader',
                    '-Lrt',
                    '-lversion',
                    '-lreader_common',
                    '-lud',
                    '-lud_api',
                    # '-llogserver',
                    '-lcommonlib',
                    '-lcli',
                    '-lMyki_BR',
                    '-lMyki_CD', 
                    '-lMyki_LDT',
                    '-lMyki_alarm',
                    '-lmyki-tmi',
                    '-lcsf',
                    '-lcsx',
                    '-lcs',
                    '-losa',
                    '-ltimingpoint',
                    '-lct_desfire_virtual',
                    '-ldaf_myki_cardservices',
                    '-ldaf_ct_crypto_stubs',
                    '-ldaf_myki_fs',
                    '-ldaf_utils_date_time',
                    '-ldaf_utils_bitstream',
                    '-ldaf_utils_serial_comms',
                    '-ldaf_utils_timer',
                    '-ldaf_desfire',
                    '-lbertlv',
                    '-ldaf_osa_mos',
                    '-llinkInterfaceDevice',
                    '-lcrypto-lock',
                    '-ldatertswis',
                    '-Lstdc++',
                    '-Lpthread',
                    '-Ldl',
                    '-lcrc',
                    '-Lssl',
                    '-Lz',
                '--MultiScan'
            );

Prog        (   'UBUNTU12',
                'gacreaderapp',
                    '-Lboost_filesystem',
                    '-Lboost_system',
                    '-Lpthread',
                    '-LllcommsD',
                    '-laserpent-lcd',
                    '-laserpent-util',
                    '-laserpent-pip',
                    '-laserpent-fram',
                    '-laserpent-sam',
                    '-laserpent-pmc',
                    '-ljsoncpp',
                    '-Lrt',
                    '-Lz',
                    '-Lm',
                    '--MultiScan'
            );

Prog        (   '!UBUNTU12',
                'gacreaderapp',
                    '-Lboost_filesystem',
                    '-Lboost_system',
                    '-lsmcrfirmware',
                    '-LllcommsD',
                    '-laserpent-led',
                    '-laserpent-lcd',
                    '-laserpent-util',
                    '-laserpent-pip',
                    '-laserpent-fram',
                    '-laserpent-sam',
                    '-laserpent-pmc',
                    '-laserpent-bbsram',
                    '-ljsoncpp',
                    '-Lz',
                    '--MultiScan'
            );

Prog        (   '*',
                'wstest',
                    'example-client.cpp',
                    'easywsclient.cpp',
                    '-Lstdc++'
            );



# Package the imageviewer program in a directory that allows the
# Debian script to find the program.


PackageProg ( '*', 'gacreaderapp' );
PackageProg ( '*', 'log_client' );

# Create a Debian package that can be installed on the target by
# running the PackageApp.pl script.
MakeDebianPackage ('!UBUNTU12', '--Script=PackageApp.pl', '--Name=GACReaderApp' );
MakeDebianPackage ('UBUNTU12', '--Script=PackageTestApp.pl', '--Name=GACReaderApp' );

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

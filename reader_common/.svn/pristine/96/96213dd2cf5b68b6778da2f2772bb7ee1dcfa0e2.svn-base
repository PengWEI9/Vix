# Copyright (C) 2015-Present Vix Products Pty Ltd, All Rights Reserved
#
# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

Platform('*');

AddFlags('PPC_603E', '-DBBRAM_ENABLED');

Src('PPC_603E, COBRA, UBUNTU12', 'main.cpp',
        'bbrambackupmanagertestcase.cpp',
        'filebackupmanagertestcase.cpp');

Prog('PPC_603E, COBRA, UBUNTU12', 'backupmanagertests', @OBJS,
        '-lreader_common',
        '-lutf',
        '-lcs',
        '-lcsf',
        '-lcsx',
        '-losa',
        '-Lrt',
        '-Lpthread',
        '-Lm',
        '-Ldl',
        '-laserpent-bbsram',
        '-Lstdc++');

MakefileGenerate();
1;






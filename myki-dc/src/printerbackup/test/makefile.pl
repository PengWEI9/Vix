# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

Platform('*');

Src('*', 'main.cpp',
         'printerbackuptestcase.cpp');

Prog('*', 'dc_printerbackup_tests', @OBJS, 
        '-ldc_printerbackup',
        '-lreader_common',
        '-lutf',
        '-ljsoncpp',
        '-lcrc',
        '-ldatertswis',  
        '-lcs',
        '-lcsf',
        '-lcsx',
        '-losa',
        '-Lrt',
        '-Lpthread',
        '-Lm',
        '-Ldl',
        '-Lstdc++');

MakefileGenerate();
1;

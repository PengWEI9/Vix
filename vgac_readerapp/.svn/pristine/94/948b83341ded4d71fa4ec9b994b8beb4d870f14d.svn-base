# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

# build for cobra and pc platforms
Platform('*');

Src('*', 'persistentuseridtestcase.cpp',
        '../persistentuserid.cpp',
        'main.cpp');

Prog('*', 'userid_tests', @OBJS,
        '-lcsf',
        '-lcs',
        '-lutf',
        '-Lpthread',
        '-Lboost_filesystem',
        '-Lboost_system',
        '-Lstdc++');

PackageProg('*', 'userid_tests');

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

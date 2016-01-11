# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the 
# BusinessRules libary.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

# Define the set of platforms that this component is built for.  By 
# defining them as '*' we use the same platforms as defined in the 
# build.pl file.
Platform    ( '*' );

AddFlags    ('COBRA', '-DCOBRA' );

# List all the source files that make up the application.
Src( '*', "perftest.cpp" ); 
Src( '*', "runall.cpp" );

#
#   Build and install the program
Prog ( '*', "perftest", "perftest",
    "-lMyki_CD",
    "-ldaf_myki_cardservices",
    "-ldatertswis",
    "-ljsoncpp",
    "-lcsx" );
Prog ( '!WIN32', "perftest",
    "-Lstdc++",
    "-Lpthread",
    "-Ldl" );
Prog ( '*', "runall", "runall",
    "-lMyki_CD",
    "-ldaf_myki_cardservices",
    "-ljsoncpp",
    "-ldatertswis",
    "-lcsx" );
Prog ( '!WIN32', "runall",
    "-Lstdc++",
    "-Lpthread",
    "-Ldl" );

Prog ( 'UBUNTU12', "perftest","-Lm");
Prog ( 'UBUNTU12', "runall","-Lm");


#.............................................................................
# Packaging definitions
#


#.............................................................................
# Finally generate the makefile
#
MakefileGenerate();

#..  Successful termination
1;


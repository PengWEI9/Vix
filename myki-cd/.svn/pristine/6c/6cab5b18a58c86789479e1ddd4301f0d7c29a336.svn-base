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
Platform    (   '*' );

# List all the source files that make up the application.
Src         (   '*',    'main.cpp'                                                     );
Src         (   '*',    'TestCaseProducts.h',          'TestCaseProducts.cpp'          );
Src         (   '*',    'TestCaseDeviceParameters.h',  'TestCaseDeviceParameters.cpp'  );

#
#   Build and install the program
Prog        (   '*',
                'unit-tests',
                    @OBJS,
                    '-lMyki_CD',
                    '-ldaf_myki_cardservices',
                    '-ldatertswis',
                    '-ljsoncpp',
                    '-lutf',
                    '-lcs',
                    '-lcsf',
                    '-lcsx' );

Prog        (   '!WIN32',
                'unit-tests',
                    '-Lstdc++',
                    '-Lpthread',
                    '-Ldl' );

Prog        (   'UBUNTU12',
                'unit-tests',
                    '-Lm' );


#.............................................................................
# Packaging definitions
#


#.............................................................................
# Finally generate the makefile
#
MakefileGenerate();

#..  Successful termination
1;

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

Src('*', 'ActivatedCD.sh');

#
GenerateFiles ( 'UBUNTU12',
    '--Script=ActivatedCD.sh',
    '--Var(InterfaceBinDir,--AbsPath)',
    '--Prerequisite(productcfg_empty.db)',
    '--GeneratedCommon(productcfg_empty_activated.db)' );

PackageFile('UBUNTU12', 'productcfg.db', '--Dir=etc');
PackageFile('UBUNTU12', 'productcfg_empty.db', '--Dir=etc');
PackageFile('UBUNTU12', 'productcfg_empty_activated.db', '--Dir=etc');
PackageFile('UBUNTU12', 'actionlist.db', '--Dir=etc');
PackageFile('UBUNTU12', 'actionlist_empty.db', '--Dir=etc');

#
MakefileGenerate();


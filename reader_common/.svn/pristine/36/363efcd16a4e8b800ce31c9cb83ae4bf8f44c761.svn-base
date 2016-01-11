# Vix Technology    Licensed Software
# (C) 2015          All rights reserved


die "Usage: Makefile.pl rootdir Makelib.pl/n"
unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

# build for all platforms
Platform ('*');

Src('*', 'exceptions.h',
        'fileexceptions.h',
        'bbramexceptions.h');

InstallHdr('*', '--Prefix=backupmanager', @CHDRS);
PackageHdr('*', '--Prefix=backupmanager', @CHDRS);

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

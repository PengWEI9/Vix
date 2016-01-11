#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
#===============================================================================

# Build system
die "Usage: Makefile.pl rootdir Makelib.pl\n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";


# Platform configuration
Platform            ('*');


# Test helper scripts
Script              ('NATIVE', 'unittest.sh');


# Test cases: lockrun.
Script              ('NATIVE', 'lockrun/1lock-run.sh');
RunTest             ('NATIVE', '1lock-run.sh', '--Name=lockrun_1lock-run', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/1lock-deny.sh');
RunTest             ('NATIVE', '1lock-deny.sh', '--Name=lockrun_1lock-deny', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/1lock-wait.sh');
RunTest             ('NATIVE', '1lock-wait.sh', '--Name=lockrun_1lock-wait', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-run.sh');
RunTest             ('NATIVE', '2locks-run.sh', '--Name=lockrun_2locks-run', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/1lock-cannot-make-file.sh');
RunTest             ('NATIVE', '1lock-cannot-make-file.sh', '--Name=lockrun_1lock-cannot-make-file', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/1lock-not-inherited.sh');
RunTest             ('NATIVE', '1lock-not-inherited.sh', '--Name=lockrun_1lock-not-inherited', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-deny-first.sh');
RunTest             ('NATIVE', '2locks-deny-first.sh', '--Name=lockrun_2locks-deny-first', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-deny-second.sh');
RunTest             ('NATIVE', '2locks-deny-second.sh', '--Name=lockrun_2locks-deny-second', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-wait-first.sh');
RunTest             ('NATIVE', '2locks-wait-first.sh', '--Name=lockrun_2locks-wait-first', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-wait-second.sh');
RunTest             ('NATIVE', '2locks-wait-second.sh', '--Name=lockrun_2locks-wait-second', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-wait-both.sh');
RunTest             ('NATIVE', '2locks-wait-both.sh', '--Name=lockrun_2locks-wait-both', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-cannot-make-file.sh');
RunTest             ('NATIVE', '2locks-cannot-make-file.sh', '--Name=lockrun_2locks-cannot-make-file', '--Var(LocalBinDir,--abspath)', '--Auto');
Script              ('NATIVE', 'lockrun/2locks-not-inherited.sh');
RunTest             ('NATIVE', '2locks-not-inherited.sh', '--Name=lockrun_2locks-not-inherited', '--Var(LocalBinDir,--abspath)', '--Auto');


# Debian package used for testing on devices
MakeDebianPackage   ('COBRA,PPC_603E', '--Script=PackageApp.pl', '--Variant=unittest');


# Generate the makefile and complete
MakefileGenerate    ();
1;


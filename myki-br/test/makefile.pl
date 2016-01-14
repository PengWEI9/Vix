###############################################################################
#
#   Makefile for generic libraries
#
###############################################################################

#
#   Check arguments
#

die "Usage: Makefile.pl rootdir Makelib.pl\n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

#
#   Platforms
#

Platform( '*' );

#
#   Compiler flags (use of these should be minimised)
#

CompileOptions ( 'WIN32', "subsystem:console" );

#   Convenience definitions
#
AddCFlags( 'COBRA'      , '-DCOBRA'             , '-DSERPENT' );
AddCFlags( 'PPC_603E'   , '-DVIPER'             , '-DSERPENT' );
AddCFlags( 'UBUNTU12'   , '-DLINUX_DESKTOP'     , '-DDESKTOP' );
AddCFlags( 'UBUNTU12_INSTRUMENT'   , '-DLINUX_DESKTOP'     , '-DDESKTOP' );

#
#   LDT is not built for WIN32, and UD is not completely built for either WIN32 or PPC_603E
#

AddCFlags( '*', '-DLDT_SUPPORTED' );

###############################################################################
#
#   Source files
#
###############################################################################

#
#   Platform-independent modules
#

Src(    '*',
        'main.c',
        'MYKI_CD_Stubs.c',
        'MYKI_CS_Stubs.c',
        'test_common.c',
        'test_BR_LLSC_0_A.c',
        'test_BR_LLSC_0_B.c',
        'test_BR_LLSC_0_1.c',
        'test_BR_LLSC_1_1.c',
        'test_BR_LLSC_1_2.c',
        'test_BR_LLSC_1_3.c',
        'test_BR_LLSC_1_5.c',
        'test_BR_LLSC_1_6.c',
        'test_BR_LLSC_1_7.c',
        'test_BR_LLSC_1_8.c',
        'test_BR_LLSC_1_9.c',
        'test_BR_LLSC_1_10.c',
        'test_BR_LLSC_2_2.c',
        'test_BR_LLSC_2_5.c',
        'test_BR_LLSC_2_6.c',
        'test_BR_LLSC_2_7.c',
        'test_BR_LLSC_2_11.c',
        'test_BR_LLSC_2_13.c',
        'test_BR_LLSC_2_14.c',
        'test_BR_LLSC_4_1.c',
        'test_BR_LLSC_4_11.c',
        'test_BR_LLSC_4_14.c',
        'test_BR_LLSC_4_15.c',
        'test_BR_LLSC_4_17.c',
        'test_BR_LLSC_4_19.c',
        'test_BR_LLSC_5_2.c',
        'test_BR_LLSC_5_3.c',
        'test_BR_LLSC_5_9.c',
        'test_BR_LLSC_6_1.c',
        'test_BR_LLSC_6_3.c',
        'test_BR_LLSC_6_10.c',
        'test_BR_LLSC_6_18.c',
        'test_BR_LLSC_6_19.c',
        'test_BR_LLSC_6_20.c',
        'test_BR_LLSC_6_21.c',
        'test_BR_LLSC_6_22.c',
        'test_BR_LLSC_6_23.c',
        'test_BR_LLSC_7_4.c',
        'test_BR_LLSC_7_5.c',
        'test_BR_LLSC_9_1.c',
        'test_BR_LLSC_10_1.c',
        'test_BR_LLSC_10_3.c',
        'test_BR_LLSC_10_4.c',
        'test_BR_LLSC_10_6.c',
        'test_BR_LLSC_99_1.c',

        'test_BR_OSC_0_B.c',
        'test_BR_OSC_1_4.c',

        'test_BR_VIX_OSC_2_1.c',
        'test_BR_VIX_OSC_2_3.c',
        'test_BR_VIX_OSC_2_5.c',

        'test_BR_VIX_OSC_3_1.c',
        'test_BR_VIX_OSC_3_2.c',
        'test_BR_VIX_OSC_3_3.c',
        'test_BR_VIX_OSC_3_4.c',
        'test_BR_VIX_OSC_3_5.c',

        'test_BR_VIX_LLSC_20_1.c',
        'test_BR_VIX_LLSC_20_2.c',
        
        'test_BR_VIX_LLSC_21_1.c',
        'test_BR_VIX_LLSC_21_2.c',
        
        'test0nn.c',
        'test1nn.c',
        'test2nn.c',
        'test_SEQ_OSC_Pin.c',
        'test_SEQ_AddValue.c',
        'test_SEQ_AddValueReversal.c',
);

################################################################################
#
#   Build the Test Application
#
################################################################################

TestProg(   '*',
            'test-myki-br',
            @OBJS,

            '-lMyki_BR',
            '-lMyki_LDT',
            '-lud',
            '-lud_api',
            '-Lpthread',
            '-lcs',
            '-ldatertswis',
            '-Lstdc++',
            '--MultiScan',
    );

################################################################################
#
#   Run the Test Application
#
################################################################################

#RunTest( 'UBUNTU12',    'test-myki-br', '--Auto' );
RunTest( 'UBUNTU12',    'test-myki-br', '--Auto', '-d99' );
RunTest( 'UBUNTU12_INSTRUMENT',    'test-myki-br', '--Auto', '-d99' );

################################################################################
#
#   Generate the makefile and exit
#
################################################################################

MakefileGenerate();
1;


#!/bin/bash

BCM_TOOLCAIN_DIR=/opt/softs/compilers/others/Broadcom/BCM_LNX_1_0_0
CHANALD_TOOLCAIN_DIR=/opt/softs/toolchains/CANALD_KAONCO1200C_BCM7356_LNUX_01/v0.5_REL.8/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_22
NEWCO_TOOLCHAIN_DIR=/opt/softs/toolchains/NEWCO_HUMAXHDR1002S_BCM7356_LNUX_01/I.18.01.03.01_Evolution_MOR_Release6_Integration_ioctl_hijack/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_11
REF_TOOLCHAIN_DIR=/opt/softs/toolchains/REF_BCM97425_BCM7425_LNUX_01/M6.0.67_6.1_ioctl_hijack/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_12
ST40_CC14_0_1_TOOLCHAIN_DIR=/opt/softs/compilers/st/st40/LINUX_4.2.4.jan0711/STLinux-2.3/devkit/sh4_uclibc
ST40_CC15_0_0_TOOLCHAIN_DIR=/opt/softs/compilers2/st/st40/LINUX_4.2.4_0.8.0/STLinux-2.4/devkit/sh4_uclibc
UPCH_TOOLCHAIN_DIR=/opt/softs/toolchains/UPCH_SAMVGW_INTELGRV_LNUX_01/M16.0_126_7.1/platform_cfg/linux/compiler/i686_gcc_x86_linux_01/i686-linux-elf
VOO_TOOLCHAIN_DIR=/opt/softs/toolchains/VOO_CISCOISB8K_BCM7241_LNUX_01/M6.0.75_M3_8.2/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_22


function SetPlatformVariable
{
	TMP_MHA_TARGET=$1
	echo "In SetPlatformVariable "${TmpTarget}
	# Set variables for the configure
	case $TMP_MHA_TARGET in
		desktop)
			export CC=gcc
			export CXX=g++
			export LINK=g++
			;;
		BCM)
			export CC=${BCM_TOOLCAIN_DIR}/bin/mips-linux-uclibc-gcc
			export CXX=${BCM_TOOLCAIN_DIR}/bin/mips-linux-uclibc-g++
			export LINK=${BCM_TOOLCAIN_DIR}/bin/mips-linux-uclibc-g++
			;;
		BCMEL)
			export CC=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			;;
		CHANALD)
			export CC=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			;;
		NEWCO)
			export CC=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			;;
		REF)
			export CC=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			;;
		ST40_CC14_0_1)
			export CC=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-gcc
			export CXX=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export LINK=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			;;
		ST40_CC15_0_0)
			export CC=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-gcc
			export CXX=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export LINK=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			;;
		UPCH)
			export CC=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-gcc
			export CXX=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-g++
			export LINK=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-g++
			;;
		VOO)
			export CC=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			;;
	esac
}


TARGET_LIST=$1
MODE_LIST=$2
OUTDIR=$3

if [ -z "$TARGET_LIST" ]
then
	echo "TARGET = desktop"
	TARGET_LIST="desktop"
fi

if [ "$TARGET_LIST" = "all" ]
then
	echo "TARGET = all"
	TARGET_LIST="desktop BCM CHANALD NEWCO REF ST40_CC14_0_1 ST40_CC15_0_0 UPCH VOO"
fi

if [ -z "$MODE_LIST" ]
then
	echo "MODE = release_dbg release"
	MODE_LIST="release_dbg release"
fi

if [ -z "$OUTDIR" ]
then
	OUTDIR=`pwd`
fi

SUPPORTED_TARGETS="desktop,BCM,CHANALD,NEWCO,REF,ST40_CC14_0_1,ST40_CC15_0_0,UPCH,VOO"
SUPPORTED_MODES="release_dbg,release"

for TmpTarget in $TARGET_LIST
do	
	targets=($(echo $SUPPORTED_TARGETS | sed 's/,/\n/g' | grep -x $TmpTarget))
	if [ ! $targets ]
	then
		echo "Target \"$TmpTarget\" is not supported."
		exit -1;
	fi
	for TmpMode in $MODE_LIST
	do	
		modes=($(echo $SUPPORTED_MODES | sed 's/,/\n/g' | grep -x $TmpMode))
		if [ ! $modes ]
		then
			echo "Mode \"$TmpMode\" is not supported."
			exit -1;
		fi
		echo -e "\n************************************************"
		echo "Building" ${TmpTarget} ${TmpMode}
		echo "*************************************************"
		SetPlatformVariable $TmpTarget
		rm -f wd.gypi
		cp wd.gypi.${TmpTarget} wd.gypi
		if [ ! -f wd.gypi ]
		then
    		echo "Create default wd.gypi"
    		cp wd.gypi.common wd.gypi
		fi 
		./build.sh $OUTDIR ${TmpTarget} ${TmpMode} ${QT_DIR}
	done
done
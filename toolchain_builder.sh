#!/bin/bash

BCM_TOOLCAIN_DIR=/opt/softs/compilers/others/Broadcom/BCM_LNX_1_0_0
BCM_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.2-wk116000.9_d_WEBKITQT48_MAIN_MHA_Int/BCM_LNX_1_0_2/release


CHANALD_TOOLCAIN_DIR=/opt/softs/toolchains/CANALD_KAONCO1200C_BCM7356_LNUX_01/v0.5_REL.8/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_22
CHANALD_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.2-wk116000.9_d_WEBKITQT48_MAIN_MHA_Int/BCM_LNX_1_0_2/release


NEWCO_TOOLCHAIN_DIR=/opt/softs/toolchains/NEWCO_HUMAXHDR1002S_BCM7356_LNUX_01/I.18.01.03.01_Evolution_MOR_Release6_Integration_ioctl_hijack/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_11
NEWCO_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.2-wk116000.9_d_WEBKITQT48_MAIN_MHA_Int/BCM_LNX_1_0_2/release


REF_TOOLCHAIN_DIR=/opt/softs/toolchains/REF_BCM97425_BCM7425_LNUX_01/M6.0.67_6.1_ioctl_hijack/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_12
REF_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.2-wk116000.9_d_WEBKITQT48_MAIN_MHA_Int/BCM_LNX_1_0_2/release


ST40_CC14_0_1_TOOLCHAIN_DIR=/opt/softs/toolchains/ST40_CC14_0_1/STLinux-2.3-gcc4.2.4-uclibc_05
ST40_CC14_0_1_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-0.9.REL_WEBKITQT48_MAIN_MHA_Int/ST40_CC14_0_1/release


ST40_CC15_0_0_TOOLCHAIN_DIR=/opt/softs/toolchains/ST40_CC15_0_0/STLinux-2.4-gcc4.5.3-uclibc_03
ST40_CC15_0_0_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.3-wk116000.10_b_WEBKITQT48_MAIN_MHA_Int/ST40_CC15_0_0/debug


UPCH_TOOLCHAIN_DIR=/opt/softs/toolchains/UPCH_SAMVGW_INTELGRV_LNUX_01/M16.0_126_7.1/platform_cfg/linux/compiler/i686_gcc_x86_linux_01/i686-linux-elf
UPCH_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/UPCH_SAMVGW_INTELGRV_LNUX_01/release


VOO_TOOLCHAIN_DIR=/opt/softs/toolchains/VOO_CISCOISB8K_BCM7241_LNUX_01/M6.0.75_M3_8.2/platform_cfg/linux/compiler/mips4k_gcc_x86_linux_22
VOO_QT_DIR=/home/jenkins/jobs/BUILD_MVM_All/workspace/build/TAMIAS/WEBKITQT48/cisco-4.8-cmc-openmha-qt4.8.4.2-wk116000.9_d_WEBKITQT48_MAIN_MHA_Int/BCM_LNX_1_0_2/release


ARMV7_TOOLCHAIN_DIR=/opt/softs/compilers/st/arm/LINUX_4.6.3_112/armv7

GNUARM_QT_DIR=/opt/android/Qt5.1/5.1.0/android_armv7

ARM_ANGSTROM_TOOLCHAIN_DIR=/opt/softs/compilers/st/arm/LINUX_4.6.3_112/arm-angstrom
ARM_ANGSTROM_QT_DIR=/opt/android/Qt5.1/5.1.0/android_armv7



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
			export QT_DIR=${BCM_QT_DIR}
			;;
		BCMEL)
			export CC=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${BCM_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export QT_DIR=${BCM_QT_DIR}
			;;
		CHANALD)
			export CC=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${CHANALD_TOOLCAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export QT_DIR=${CHANALD_QT_DIR}
			;;
		NEWCO)
			export CC=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${NEWCO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export QT_DIR=${NEWCO_QT_DIR}
			;;
		REF)
			export CC=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${REF_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export QT_DIR=${REF_QT_DIR}
			;;

		ST40_CC14_0_1)
			export CC=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-gcc
			export CXX=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export LINK=${ST40_CC14_0_1_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export QT_DIR=${ST40_CC14_0_1_QT_DIR}
			;;

		ST40_CC15_0_0)
			export CC=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-gcc
			export CXX=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export LINK=${ST40_CC15_0_0_TOOLCHAIN_DIR}/bin/sh4-linux-uclibc-g++
			export QT_DIR=${ST40_CC15_0_0_QT_DIR}
			;;

		UPCH)
			export CC=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-gcc
			export CXX=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-g++
			export LINK=${UPCH_TOOLCHAIN_DIR}/bin/i686-cm-linux-g++
			export QT_DIR=${UPCH_QT_DIR}
			;;

		VOO)
			export CC=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-gcc
			export CXX=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export LINK=${VOO_TOOLCHAIN_DIR}/bin/mipsel-linux-uclibc-g++
			export QT_DIR=${VOO_QT_DIR}
			;;
		ARMV7)
			export CC=${ARMV7_TOOLCHAIN_DIR}/bin/armv7-linux-gcc
			export CXX=${ARMV7_TOOLCHAIN_DIR}/bin/armv7-linux-g++
			export LINK=${ARMV7_TOOLCHAIN_DIR}/bin/armv7-linux-g++
			export QT_DIR=${ARMV7_QT_DIR}
			;;

		ARM_ANGSTROM)
			export CC=${ARM_ANGSTROM_TOOLCHAIN_DIR}/bin/arm-angstrom-linux-gnueabi-gcc
			export CXX=${ARM_ANGSTROM_TOOLCHAIN_DIR}/bin/arm-angstrom-linux-gnueabi-g++
			export LINK=${ARM_ANGSTROM_TOOLCHAIN_DIR}/bin/arm-angstrom-linux-gnueabi-g++
			export QT_DIR=${ARM_ANGSTROM_QT_DIR}
			;;
	
		GNUARM)
			export CC=arm-linux-gnu-gcc
			export CXX=arm-linux-gnu-cpp
			export LINK=arm-linux-gnu-cpp
			export QT_DIR=${GNUARM_QT_DIR}
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
	TARGET_LIST="desktop BCM CHANALD NEWCO REF ST40_CC14_0_1 ST40_CC15_0_0 UPCH VOO ARMV7 ARM_ANGSTROM GNUARM"
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

SUPPORTED_TARGETS="desktop,BCM,CHANALD,NEWCO,REF,ST40_CC14_0_1,ST40_CC15_0_0,UPCH,VOO,ARMV7,ARM_ANGSTROM,GNUARM"
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

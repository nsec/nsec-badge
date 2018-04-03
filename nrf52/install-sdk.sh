#!/bin/sh

set -e

SDK_DIR=$1
SDK_VER=$2

GNU_PREFIX=arm-none-eabi

if test -z "${SDK_VER}" || test -z "${SDK_DIR}"; then
	echo "usage: $(basename $0) install_dir sdk_version"
	exit 1
fi

set -u

if test -d "${SDK_VER}" || test -d "${SDK_DIR}"; then
    rm -rf "${SDK_VER}" "${SDK_DIR}"
fi

SDK_MAJOR="$(echo "${SDK_VER}" | sed -r 's/^nRF5_SDK_([0-9]+).*/\1/')"

wget -c "http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v${SDK_MAJOR}.x.x/${SDK_VER}.zip"
wget -c "https://github.com/ARM-software/CMSIS/raw/master/CMSIS/Lib/GCC/libarm_cortexM4l_math.a"

unzip -n "${SDK_VER}.zip" "${SDK_VER}/components/*" \
	"${SDK_VER}/external/*" \
	"${SDK_VER}/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar" \
	"${SDK_VER}/documentation/nRF5_Nordic_license.txt" \
	"${SDK_VER}/documentation/nRF5_Dynastream_license.txt"

mv "${SDK_VER}/components" "${SDK_DIR}"
mv "${SDK_VER}/external" "${SDK_DIR}/"
mv "${SDK_VER}/external_tools" "${SDK_DIR}/"
mv "${SDK_VER}/documentation/nRF5_Nordic_license.txt" "${SDK_DIR}/"
mv "${SDK_VER}/documentation/nRF5_Dynastream_license.txt" "${SDK_DIR}/"
mv "libarm_cortexM4l_math.a" "${SDK_DIR}/toolchain/cmsis/dsp/GCC/"
cp "${SDK_DIR}/softdevice/s132/doc/s132_nrf52_5.0.0_licence-agreement.txt" "${SDK_DIR}/"

echo "GNU_PREFIX ?= ${GNU_PREFIX}
GNU_INSTALL_ROOT ?= \$(shell dirname \$(shell which \$(GNU_PREFIX)-gcc))/
GNU_VERSION ?= \$(shell \"\$(GNU_INSTALL_ROOT)\$(GNU_PREFIX)-gcc\" -dumpversion)" > "${SDK_DIR}/toolchain/gcc/Makefile.posix"

# Patch
patch -p0 --binary < fix_fstorage_init.patch

# Cleanup
rmdir "${SDK_VER}/documentation"
rmdir "${SDK_VER}"
rm -f "${SDK_VER}.zip"
rm -f "libarm_cortexM4l_math.a"

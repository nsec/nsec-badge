#!/bin/sh

set -e

SCRIPTPATH=$(dirname $0)

SDK_DIR=$1
SDK_MAJOR_VER=$2
SDK_MINOR_VER=$3
SDK_PATCH_VER=$4
SDK_COMMIT_VER=$5

SDK_VER="nRF5_SDK_${SDK_MAJOR_VER}.${SDK_MINOR_VER}.${SDK_PATCH_VER}_${SDK_COMMIT_VER}"

GNU_PREFIX=arm-none-eabi

if test -z "${SDK_VER}" || test -z "${SDK_DIR}"; then
	echo "usage: $(basename $0) install_dir sdk_version"
	exit 1
fi

set -u

if test -d "${SDK_VER}" || test -d "${SDK_DIR}"; then
    rm -rf "${SDK_VER}" "${SDK_DIR}"
fi

mkdir "${SDK_DIR}"

wget -c "http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v${SDK_MAJOR_VER}.x.x/${SDK_VER}.zip"

unzip -n "${SDK_VER}.zip" -d "${SDK_VER}" \
	"components/*" \
	"modules/*" \
	"external/*" \
	"external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar" \
	"documentation/nRF5_Nordic_license.txt"

mv "${SDK_VER}/components" "${SDK_DIR}/"
mv "${SDK_VER}/modules" "${SDK_DIR}/"
mv "${SDK_VER}/external" "${SDK_DIR}/"
mv "${SDK_VER}/external_tools" "${SDK_DIR}/"
mv "${SDK_VER}/documentation/nRF5_Nordic_license.txt" "${SDK_DIR}/"
cp "${SDK_DIR}/components/softdevice/s132/doc/s132_nrf52_7.0.1_licence-agreement.txt" "${SDK_DIR}/"

echo "GNU_PREFIX ?= ${GNU_PREFIX}
GNU_INSTALL_ROOT ?= \$(shell dirname \$(shell which \$(GNU_PREFIX)-gcc))/
GNU_VERSION ?= \$(shell \"\$(GNU_INSTALL_ROOT)\$(GNU_PREFIX)-gcc\" -dumpversion)" > "${SDK_DIR}/components/toolchain/gcc/Makefile.posix"

# Patch
#patch -p0 --binary < "$SCRIPTPATH/../patches/fix_fstorage_init.patch"
#patch -p0 --binary < "$SCRIPTPATH/../patches/fix-ringbuf.patch"
#patch -p0 --binary < "$SCRIPTPATH/../patches/fix-uart-error-handling.patch"
#patch -p0 --binary < "$SCRIPTPATH/../patches/fix-uart-cli-overrun-handling.patch"
#patch -p0 --binary < "$SCRIPTPATH/../patches/cli-make-backspace-and-delete-work.patch"

# Cleanup
rmdir "${SDK_VER}/documentation"
rmdir "${SDK_VER}"
rm -f "${SDK_VER}.zip"

#!/usr/bin/env bash
set -euo pipefail

# Build Debian (.deb) package for LinuxShot
VERSION="${1:-0.1.0}"
ARCH="amd64"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
PKG_DIR="${BUILD_DIR}/deb/linuxshot_${VERSION}_${ARCH}"
OUTPUT_DIR="${ROOT_DIR}/packaging/deb"

echo "=== Building LinuxShot Debian Package (v${VERSION}) ==="

mkdir -p "${OUTPUT_DIR}"
rm -rf "${PKG_DIR}"
mkdir -p "${PKG_DIR}/DEBIAN"
mkdir -p "${PKG_DIR}/usr/bin"
mkdir -p "${PKG_DIR}/usr/share/applications"
mkdir -p "${PKG_DIR}/usr/share/icons/hicolor/scalable/apps"
mkdir -p "${PKG_DIR}/usr/share/icons/hicolor/256x256/apps"

# Ensure binary is built
if [ ! -f "${BUILD_DIR}/linuxshot" ]; then
    echo "Error: ${BUILD_DIR}/linuxshot not found. Build the project first."
    exit 1
fi

cp "${BUILD_DIR}/linuxshot" "${PKG_DIR}/usr/bin/"
chmod 755 "${PKG_DIR}/usr/bin/linuxshot"

cp "${ROOT_DIR}/packaging/linuxshot.desktop" "${PKG_DIR}/usr/share/applications/"
cp "${ROOT_DIR}/resources/icons/linuxshot.svg" "${PKG_DIR}/usr/share/icons/hicolor/scalable/apps/"
cp "${ROOT_DIR}/resources/icons/linuxshot.png" "${PKG_DIR}/usr/share/icons/hicolor/256x256/apps/"

# Determine dependencies based on Debian/Ubuntu version
# Ubuntu 24.04 (Noble) uses libqt6core6t64, Ubuntu 22.04 (Jammy) uses libqt6core6
QT_CORE_DEP="libqt6core6 | libqt6core6t64"
QT_GUI_DEP="libqt6gui6 | libqt6gui6t64"
QT_WIDGETS_DEP="libqt6widgets6 | libqt6widgets6t64"
QT_DBUS_DEP="libqt6dbus6 | libqt6dbus6t64"
QT_NET_DEP="libqt6network6 | libqt6network6t64"

cat <<EOF > "${PKG_DIR}/DEBIAN/control"
Package: linuxshot
Version: ${VERSION}
Section: graphics
Priority: optional
Architecture: ${ARCH}
Maintainer: Ameya Deshpande <https://github.com/ameya-deshpande-5056/linuxshot>
Homepage: https://github.com/ameya-deshpande-5056/linuxshot
Depends: libc6, libgcc-s1, libgl1, ${QT_CORE_DEP}, ${QT_GUI_DEP}, ${QT_WIDGETS_DEP}, ${QT_DBUS_DEP}, ${QT_NET_DEP}, libstdc++6, libxcb1, libxcb-shm0, libxcb-keysyms1
Description: Complete Linux screenshot application with region capture & annotation
 LinuxShot is a lightweight, high-performance desktop screenshot tool
 inspired by Lightshot. Features include region capture, full-screen
 capture, annotation tools (pen, line, arrow, rectangle, circle, marker,
 text, blur), clipboard copy, and configurable image saving.
EOF

DEB_FILE="${OUTPUT_DIR}/linuxshot_${VERSION}_${ARCH}.deb"
dpkg-deb --build --root-owner-group "${PKG_DIR}" "${DEB_FILE}"

echo "Debian package created: ${DEB_FILE}"

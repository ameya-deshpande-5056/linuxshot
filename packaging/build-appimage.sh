#!/usr/bin/env bash
set -euo pipefail

# Build AppImage for LinuxShot
VERSION="${1:-0.1.0}"
ARCH="x86_64"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
APPDIR="${BUILD_DIR}/AppDir"
OUTPUT_DIR="${ROOT_DIR}/packaging/appimage"
APPIMAGE_FILE="${OUTPUT_DIR}/LinuxShot-${VERSION}-${ARCH}.AppImage"
RUNTIME_FILE="${BUILD_DIR}/appimage_runtime"

echo "=== Building LinuxShot AppImage (v${VERSION}) ==="

mkdir -p "${OUTPUT_DIR}"
mkdir -p "${BUILD_DIR}"
rm -rf "${APPDIR}"

# Ensure binary is built
if [ ! -f "${BUILD_DIR}/linuxshot" ]; then
    echo "Error: ${BUILD_DIR}/linuxshot not found. Build the project first."
    exit 1
fi

mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/lib"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/scalable/apps"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"

cp "${BUILD_DIR}/linuxshot" "${APPDIR}/usr/bin/"
chmod 755 "${APPDIR}/usr/bin/linuxshot"

cp "${ROOT_DIR}/packaging/linuxshot.desktop" "${APPDIR}/linuxshot.desktop"
cp "${ROOT_DIR}/packaging/linuxshot.desktop" "${APPDIR}/usr/share/applications/"
cp "${ROOT_DIR}/resources/icons/linuxshot.png" "${APPDIR}/linuxshot.png"
cp "${ROOT_DIR}/resources/icons/linuxshot.png" "${APPDIR}/usr/share/icons/hicolor/256x256/apps/"
cp "${ROOT_DIR}/resources/icons/linuxshot.svg" "${APPDIR}/usr/share/icons/hicolor/scalable/apps/"

# Create standard AppRun script
cat <<'EOF' > "${APPDIR}/AppRun"
#!/bin/sh
SELF=$(readlink -f "$0")
HERE=${SELF%/*}
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export XDG_DATA_DIRS="${HERE}/usr/share:${XDG_DATA_DIRS}"
exec "${HERE}/usr/bin/linuxshot" "$@"
EOF
chmod 755 "${APPDIR}/AppRun"

# Acquire AppImage runtime if not cached
if [ ! -f "${RUNTIME_FILE}" ]; then
    if [ -f "/tmp/appimage_runtime" ]; then
        cp "/tmp/appimage_runtime" "${RUNTIME_FILE}"
    else
        echo "Downloading standard Type-2 AppImage runtime..."
        curl -fsSL -o "${RUNTIME_FILE}" "https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64" || \
        curl -fsSL -o "${RUNTIME_FILE}" "https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64"
    fi
fi

SQUASHFS_TMP="${BUILD_DIR}/linuxshot.squashfs"
rm -f "${SQUASHFS_TMP}" "${APPIMAGE_FILE}"

echo "Creating compressed SquashFS image..."
mksquashfs "${APPDIR}" "${SQUASHFS_TMP}" -root-owned -noappend -comp gzip -quiet

echo "Assembling AppImage binary..."
cat "${RUNTIME_FILE}" "${SQUASHFS_TMP}" > "${APPIMAGE_FILE}"
chmod +x "${APPIMAGE_FILE}"
rm -f "${SQUASHFS_TMP}"

echo "AppImage created: ${APPIMAGE_FILE}"

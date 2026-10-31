#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# LinuxShot — Native Linux Screenshot & Annotation Application
# uninstall.sh: Removes LinuxShot binary, desktop entries, and icons
# -----------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
MANIFEST="${BUILD_DIR}/install_manifest.txt"

INSTALL_PREFIX="/usr/local"
PURGE_CONFIG="false"

show_help() {
    cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Options:
  -p, --prefix <path>  Prefix directory where LinuxShot was installed (default: /usr/local)
  --purge              Also delete user configuration (~/.config/linuxshot)
  -h, --help           Show this help message and exit

Examples:
  sudo ./scripts/uninstall.sh
  sudo ./scripts/uninstall.sh --prefix /usr
  ./scripts/uninstall.sh --prefix ~/.local --purge
EOF
}

# Parse options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --purge)
            PURGE_CONFIG="true"
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Error: Unknown option '$1'" >&2
            show_help
            exit 1
            ;;
    esac
done

echo "======================================================"
echo " Uninstalling LinuxShot from: ${INSTALL_PREFIX}"
echo "======================================================"

# Determine if sudo is needed
SUDO=""
if [[ $EUID -ne 0 ]]; then
    if [[ -d "${INSTALL_PREFIX}" && ! -w "${INSTALL_PREFIX}" ]]; then
        SUDO="sudo"
    elif [[ -f "${INSTALL_PREFIX}/bin/linuxshot" && ! -w "${INSTALL_PREFIX}/bin/linuxshot" ]]; then
        SUDO="sudo"
    fi
fi

if [[ -n "${SUDO}" ]]; then
    echo "--> Elevated permissions required for '${INSTALL_PREFIX}'. Prompting for sudo..."
fi

# 1. Use install_manifest.txt if available
REMOVED_COUNT=0
if [[ -f "${MANIFEST}" ]]; then
    echo "--> Removing files listed in install manifest (${MANIFEST})..."
    while IFS= read -r file; do
        if [[ -f "${file}" || -L "${file}" ]]; then
            echo "    Removing: ${file}"
            ${SUDO} rm -f "${file}"
            ((REMOVED_COUNT++)) || true
        fi
    done < "${MANIFEST}"
fi

# 2. Explicitly remove standard installed artifacts in case manifest is absent or prefix differs
STANDARD_FILES=(
    "${INSTALL_PREFIX}/bin/linuxshot"
    "${INSTALL_PREFIX}/share/applications/linuxshot.desktop"
    "${INSTALL_PREFIX}/share/icons/hicolor/256x256/apps/linuxshot.png"
    "${INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/linuxshot.svg"
)

for file in "${STANDARD_FILES[@]}"; do
    if [[ -f "${file}" || -L "${file}" ]]; then
        echo "    Removing: ${file}"
        ${SUDO} rm -f "${file}"
        ((REMOVED_COUNT++)) || true
    fi
done

# 3. Update system caches
if command -v update-desktop-database >/dev/null 2>&1; then
    DESKTOP_DIR="${INSTALL_PREFIX}/share/applications"
    if [[ -d "${DESKTOP_DIR}" ]]; then
        echo "--> Updating desktop application database..."
        ${SUDO} update-desktop-database "${DESKTOP_DIR}" 2>/dev/null || true
    fi
fi

if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    ICON_DIR="${INSTALL_PREFIX}/share/icons/hicolor"
    if [[ -d "${ICON_DIR}" ]]; then
        echo "--> Updating GTK icon cache..."
        ${SUDO} gtk-update-icon-cache -f -q "${ICON_DIR}" 2>/dev/null || true
    fi
fi

# 4. Optional purge of configuration
if [[ "${PURGE_CONFIG}" == "true" ]]; then
    USER_CONFIG_DIR="${HOME}/.config/linuxshot"
    if [[ -d "${USER_CONFIG_DIR}" ]]; then
        echo "--> Purging configuration: ${USER_CONFIG_DIR}..."
        rm -rf "${USER_CONFIG_DIR}"
    fi
fi

echo ""
echo "======================================================"
if [[ ${REMOVED_COUNT} -gt 0 ]]; then
    echo " LinuxShot uninstalled successfully (${REMOVED_COUNT} files removed)."
else
    echo " No installed LinuxShot files were found in '${INSTALL_PREFIX}'."
fi
echo "======================================================"

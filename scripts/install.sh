#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# LinuxShot — Native Linux Screenshot & Annotation Application
# install.sh: Installs LinuxShot binary, desktop entries, and icons
# -----------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

INSTALL_PREFIX="/usr/local"
BUILD_IF_MISSING="true"

show_help() {
    cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Options:
  -p, --prefix <path>  Set installation prefix directory (default: /usr/local)
  --no-build           Fail if application is not compiled instead of building
  -h, --help           Show this help message and exit

Examples:
  sudo ./scripts/install.sh
  sudo ./scripts/install.sh --prefix /usr
  ./scripts/install.sh --prefix ~/.local
EOF
}

# Parse options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --no-build)
            BUILD_IF_MISSING="false"
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
echo " Installing LinuxShot to: ${INSTALL_PREFIX}"
echo "======================================================"

# Check if binary exists; compile if missing
if [[ ! -f "${BUILD_DIR}/linuxshot" ]]; then
    if [[ "${BUILD_IF_MISSING}" == "true" ]]; then
        echo "--> Binary not found; invoking ./scripts/build.sh..."
        "${ROOT_DIR}/scripts/build.sh" --prefix "${INSTALL_PREFIX}"
    else
        echo "Error: ${BUILD_DIR}/linuxshot not found. Run ./scripts/build.sh first." >&2
        exit 1
    fi
fi

# Determine if sudo is needed for destination
SUDO=""
if [[ $EUID -ne 0 ]]; then
    # Test writability by checking or creating prefix directory
    if [[ -d "${INSTALL_PREFIX}" ]]; then
        if [[ ! -w "${INSTALL_PREFIX}" ]]; then
            SUDO="sudo"
        fi
    else
        PARENT_DIR="$(dirname "${INSTALL_PREFIX}")"
        if [[ ! -w "${PARENT_DIR}" ]]; then
            SUDO="sudo"
        fi
    fi
fi

if [[ -n "${SUDO}" ]]; then
    echo "--> Elevated permissions required for '${INSTALL_PREFIX}'. Prompting for sudo..."
fi

# Run cmake --install
echo "--> Installing files via CMake..."
${SUDO} cmake --install "${BUILD_DIR}" --prefix "${INSTALL_PREFIX}"

# Update desktop database if tool is installed
if command -v update-desktop-database >/dev/null 2>&1; then
    DESKTOP_DIR="${INSTALL_PREFIX}/share/applications"
    if [[ -d "${DESKTOP_DIR}" ]]; then
        echo "--> Updating desktop application database..."
        ${SUDO} update-desktop-database "${DESKTOP_DIR}" 2>/dev/null || true
    fi
fi

# Update icon cache if tool is installed
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    ICON_DIR="${INSTALL_PREFIX}/share/icons/hicolor"
    if [[ -d "${ICON_DIR}" ]]; then
        echo "--> Updating GTK icon cache..."
        ${SUDO} gtk-update-icon-cache -f -q "${ICON_DIR}" 2>/dev/null || true
    fi
fi

echo ""
echo "======================================================"
echo " LinuxShot installed successfully!"
echo " Location: ${INSTALL_PREFIX}/bin/linuxshot"
echo ""
echo " You can launch LinuxShot now by running:"
echo "   linuxshot"
echo " or by pressing 'Print Screen' once launched."
echo ""
echo " To uninstall at any time, run:"
echo "   sudo ./scripts/uninstall.sh --prefix ${INSTALL_PREFIX}"
echo "======================================================"

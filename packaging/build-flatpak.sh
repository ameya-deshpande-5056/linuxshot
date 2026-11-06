#!/usr/bin/env bash
set -euo pipefail

# Build Flatpak bundle (.flatpak) for LinuxShot
VERSION="${1:-0.1.4}"
ARCH="x86_64"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/flatpak"
REPO_DIR="${BUILD_DIR}/repo"
MANIFEST="${ROOT_DIR}/packaging/flatpak/org.linuxshot.LinuxShot.yaml"
OUTPUT_DIR="${ROOT_DIR}/packaging/flatpak"
BUNDLE_FILE="${OUTPUT_DIR}/LinuxShot-${VERSION}-${ARCH}.flatpak"

echo "=== Building LinuxShot Flatpak Bundle (v${VERSION}) ==="

if ! command -v flatpak-builder &>/dev/null; then
    echo "Error: flatpak-builder not found. Please install flatpak-builder."
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"
mkdir -p "${BUILD_DIR}"

# Build Flatpak and export to local ostree repo
echo "--> Compiling Flatpak with flatpak-builder..."
flatpak-builder --force-clean \
    --user \
    --install-deps-from=flathub \
    --repo="${REPO_DIR}" \
    "${BUILD_DIR}/app" \
    "${MANIFEST}"

# Create standalone single-file Flatpak bundle
echo "--> Exporting standalone Flatpak bundle..."
flatpak build-bundle "${REPO_DIR}" "${BUNDLE_FILE}" org.linuxshot.LinuxShot stable

echo "=== Flatpak bundle successfully created ==="
echo "Artifact: ${BUNDLE_FILE}"

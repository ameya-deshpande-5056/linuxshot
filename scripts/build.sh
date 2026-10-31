#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# LinuxShot — Native Linux Screenshot & Annotation Application
# build.sh: Configures, builds, and tests LinuxShot from source
# -----------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

BUILD_TYPE="Release"
BUILD_TESTS="ON"
RUN_TESTS="true"
CLEAN_BUILD="false"
JOBS="$(nproc 2>/dev/null || echo 4)"
INSTALL_PREFIX="/usr/local"

show_help() {
    cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Options:
  -r, --release        Build in Release mode (default)
  -d, --debug          Build in Debug mode with sanitizers
  -c, --clean          Remove build directory before compiling
  -p, --prefix <path>  Set CMake install prefix (default: /usr/local)
  -j, --jobs <num>     Number of parallel compilation jobs (default: ${JOBS})
  --no-tests           Skip running unit tests after compilation
  -h, --help           Show this help message and exit

Examples:
  ./scripts/build.sh
  ./scripts/build.sh --clean --release
  ./scripts/build.sh --prefix /usr
EOF
}

# Parse command line options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD="true"
            shift
            ;;
        -p|--prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        --no-tests)
            BUILD_TESTS="OFF"
            RUN_TESTS="false"
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
echo " Building LinuxShot (${BUILD_TYPE})"
echo "======================================================"
echo "Source Directory : ${ROOT_DIR}"
echo "Build Directory  : ${BUILD_DIR}"
echo "Install Prefix   : ${INSTALL_PREFIX}"
echo "Parallel Jobs    : ${JOBS}"
echo "======================================================"

# Clean build directory if requested
if [[ "${CLEAN_BUILD}" == "true" && -d "${BUILD_DIR}" ]]; then
    echo "--> Cleaning existing build directory..."
    rm -rf "${BUILD_DIR}"
fi

# Configure with CMake
echo "--> Configuring project with CMake..."
cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
    -DBUILD_TESTS="${BUILD_TESTS}"

# Compile
echo "--> Compiling with ${JOBS} jobs..."
cmake --build "${BUILD_DIR}" -j"${JOBS}"

# Execute tests if enabled
if [[ "${RUN_TESTS}" == "true" ]]; then
    echo "--> Running automated test suite..."
    QT_QPA_PLATFORM=offscreen ctest --test-dir "${BUILD_DIR}" --output-on-failure
fi

echo ""
echo "======================================================"
echo " Build completed successfully!"
echo " Binary created at: ${BUILD_DIR}/linuxshot"
echo ""
echo " To install on this system, run:"
echo "   sudo ./scripts/install.sh"
echo "======================================================"

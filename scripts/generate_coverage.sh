#!/usr/bin/env bash
set -euo pipefail

# ----------------------------
# Config
# ----------------------------
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/cmake-build-coverage}"
COVERAGE_DIR="${COVERAGE_DIR:-$ROOT_DIR/coverage}"

# You can override jobs: JOBS=8 ./scripts/generate_coverage.sh
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

echo "==> Root:        $ROOT_DIR"
echo "==> Build dir:    $BUILD_DIR"
echo "==> Coverage dir: $COVERAGE_DIR"
echo "==> Jobs:         $JOBS"

# ----------------------------
# Preconditions
# ----------------------------
command -v lcov >/dev/null 2>&1 || {
  echo "ERROR: lcov not found. Install it first."
  echo "  Ubuntu: sudo apt-get install -y lcov"
  echo "  macOS:  brew install lcov"
  exit 1
}
command -v genhtml >/dev/null 2>&1 || {
  echo "ERROR: genhtml not found (comes with lcov)."
  exit 1
}

mkdir -p "$BUILD_DIR" "$COVERAGE_DIR"

# ----------------------------
# Clean previous counters
# ----------------------------
echo "==> Zeroing previous coverage counters"
# This clears runtime data (.gcda). Keep .gcno.
find "$BUILD_DIR" -name "*.gcda" -delete || true
find "$BUILD_DIR" -name "*.gcov" -delete || true

# ----------------------------
# Configure + build with coverage
# ----------------------------
echo "==> Configuring CMake (coverage)"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_COVERAGE=ON

echo "==> Building"
cmake --build "$BUILD_DIR" -j "$JOBS"

# ----------------------------
# Run tests (must execute to generate .gcda)
# ----------------------------
echo "==> Running tests"
ctest --test-dir "$BUILD_DIR" --output-on-failure

# ----------------------------
# Capture coverage
# ----------------------------
RAW_INFO="$COVERAGE_DIR/coverage.raw.info"
FILTERED_INFO="$COVERAGE_DIR/coverage.info"

echo "==> Capturing coverage"
# --directory must point to where gcda/gcno are (build dir)
lcov --capture \
  --directory "$BUILD_DIR" \
  --output-file "$RAW_INFO" \
  --ignore-errors \
    inconsistent,unsupported,format,source,mismatch

echo "==> Filtering coverage"
# Filter noise aggressively.
# Adjust patterns if your repo layout differs.
lcov --remove "$RAW_INFO" \
  "/usr/*" \
  "/Applications/*" \
  "$ROOT_DIR/build/*" \
  "$BUILD_DIR/*" \
  "$ROOT_DIR/tests/*" \
  "$ROOT_DIR/server/main.cpp" \
  "$ROOT_DIR/**/main.cpp" \
  "$HOME/.conan2/*" \
  "*/_deps/*" \
  "*/CMakeFiles/*" \
  "*/cmake-build-*/*" \
  "*/third_party/*" \
  --output-file "$FILTERED_INFO" \
  --ignore-errors unused,source,inconsistent,format

echo "==> Generating HTML report"
rm -rf "$COVERAGE_DIR/index.html" "$COVERAGE_DIR/gcov" "$COVERAGE_DIR/*.css" "$COVERAGE_DIR/*.png" "$COVERAGE_DIR/*.js" 2>/dev/null || true

genhtml "$FILTERED_INFO" \
  --output-directory "$COVERAGE_DIR" \
  --title "SignalStream Coverage" \
  --legend \
  --show-details \
  --ignore-errors source,corrupt,inconsistent,category

echo "✅ Done. Open:"
echo "   $COVERAGE_DIR/index.html"
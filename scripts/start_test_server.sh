#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/cmake-build-debug}"
CONFIG_FILE="$ROOT_DIR/config/config.test.json"
SERVER_BIN="$BUILD_DIR/server/signalstream_server"

echo "==> Root Directory: $ROOT_DIR"
echo "==> Build Directory: $BUILD_DIR"
echo "==> Server Binary: $SERVER_BIN"
echo "==> Config File: $CONFIG_FILE"

PORT=8080

if [[ ! -f "$SERVER_BIN" ]]; then
  echo "❌ Server binary not found: $SERVER_BIN"
  exit 1
fi

if [[ ! -f "$CONFIG_FILE" ]]; then
  echo "❌ Test config not found: $CONFIG_FILE"
  exit 1
fi

echo "🚀 Starting test server on :$PORT"

"$SERVER_BIN" \
  --config="$CONFIG_FILE" \
  --port="$PORT" &

SERVER_PID=$!
echo "$SERVER_PID" > /tmp/signalstream_test.pid

echo "🆔 Server PID: $SERVER_PID"

# --- wait until server responds ---
echo "⏳ Waiting for server to be ready..."

for i in {1..30}; do
  if nc -z 127.0.0.1 "$PORT"; then
    echo "✅ Server port $PORT is open"
    exit 0
  fi
  sleep 1
done

echo "❌ Server failed to start"
kill "$SERVER_PID"
exit 1
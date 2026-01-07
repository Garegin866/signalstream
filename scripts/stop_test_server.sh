#!/usr/bin/env bash
set -euo pipefail

PID_FILE="/tmp/signalstream_test.pid"

if [[ ! -f "$PID_FILE" ]]; then
  echo "⚠️ No test server PID file found"
  exit 0
fi

PID=$(cat "$PID_FILE")

echo "🛑 Stopping test server (PID $PID)"
kill "$PID" || true
rm -f "$PID_FILE"
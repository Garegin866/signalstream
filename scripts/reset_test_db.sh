#!/usr/bin/env bash
set -euo pipefail

DB_HOST="${DB_HOST:-127.0.0.1}"
DB_PORT="${DB_PORT:-5432}"
DB_NAME="${DB_NAME:-signaldb_test}"
DB_USER="${DB_USER:-signaluser}"
DB_PASSWORD="${DB_PASSWORD:-signalpass}"

export PGPASSWORD="$DB_PASSWORD"

psql -h "$DB_HOST" -U "$DB_USER" -d "$DB_NAME" -f db/init.sql

echo "🔄 Resetting test database..."

psql \
  -h "$DB_HOST" \
  -p "$DB_PORT" \
  -U "$DB_USER" \
  -d "$DB_NAME" <<'SQL'
BEGIN;

TRUNCATE TABLE
  users,
  sessions,
  reset_tokens,
  items,
  tags,
  item_tags,
  user_tags,
  notifications
RESTART IDENTITY CASCADE;

COMMIT;
SQL

echo "✅ Test database reset complete"
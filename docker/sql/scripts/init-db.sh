#!/usr/bin/env bash
# Import WarZ + BreezeNet schemas into warz-mssql (idempotent-ish: skips if Accounts exists)
set -euo pipefail

# Prevent Git Bash / MSYS from rewriting /opt/... paths passed to docker
export MSYS_NO_PATHCONV=1
export MSYS2_ARG_CONV_EXCL='*'

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

# Load .env
if [[ -f .env ]]; then
  set -a
  # shellcheck disable=SC1091
  source .env
  set +a
fi

PASS="${MSSQL_SA_PASSWORD:-WarZ_Local_2024!}"
CONTAINER="${MSSQL_CONTAINER:-warz-mssql}"
SQLCMD_BIN="//opt/mssql-tools18/bin/sqlcmd"

run_sqlcmd() {
  docker exec -i "$CONTAINER" "$SQLCMD_BIN" -S localhost -U sa -P "$PASS" -C "$@"
}

echo "==> Waiting for $CONTAINER ..."
for i in $(seq 1 60); do
  if run_sqlcmd -Q "SELECT 1" &>/dev/null; then
    echo "    SQL Server is up"
    break
  fi
  if [[ "$i" -eq 60 ]]; then
    echo "ERROR: SQL Server did not become ready" >&2
    exit 1
  fi
  sleep 2
done

echo "==> Creating databases"
run_sqlcmd -i //init/00-create-dbs.sql

ALREADY="$(run_sqlcmd -h -1 -W -Q "SET NOCOUNT ON; SELECT CASE WHEN OBJECT_ID(N'WarZ.dbo.Accounts') IS NOT NULL THEN 1 ELSE 0 END" | tr -d '[:space:]')"
if [[ "$ALREADY" == "1" ]]; then
  echo "==> WarZ schema already present — skip schema import"
else
  echo "==> Importing WarZ-dbo.sql (this can take a few minutes)..."
  run_sqlcmd -d WarZ -i //seed/WarZ-dbo.sql
  echo "==> Importing BreezeNet-dbo.sql..."
  run_sqlcmd -d BreezeNet -i //seed/BreezeNet-dbo.sql
fi

echo "==> Ensuring local@warz.local account"
run_sqlcmd -i //init/99-local-account.sql

echo "==> Done"
echo "    Host:   localhost,${MSSQL_PORT:-1433}"
echo "    User:   sa"
echo "    DBs:    WarZ, BreezeNet"
echo "    Login:  local@warz.local / test123"

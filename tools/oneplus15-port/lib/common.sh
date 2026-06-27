# SPDX-License-Identifier: GPL-2.0-only
set -Eeuo pipefail

PORT_TOOL_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=../config.env
source "${PORT_TOOL_DIR}/config.env"

log() { printf '[oneplus15-port] %s\n' "$*" >&2; }
warn() { printf '[oneplus15-port] WARNING: %s\n' "$*" >&2; }
die() { printf '[oneplus15-port] ERROR: %s\n' "$*" >&2; exit 1; }

have() { command -v "$1" >/dev/null 2>&1; }

kernel_root() {
  git -C "${PORT_TOOL_DIR}" rev-parse --show-toplevel 2>/dev/null || \
    die "This tool must live inside the generated Linux kernel repository."
}

jobs() {
  if have nproc; then nproc; else getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4; fi
}

ensure_dir() { mkdir -p "$@"; }

run_logged() {
  local logfile=$1
  shift
  ensure_dir "$(dirname -- "$logfile")"
  log "Running: $*"
  "$@" 2>&1 | tee "$logfile"
}

require_clean_tree() {
  local root=$1
  [[ -z "$(git -C "$root" status --porcelain)" ]] || \
    die "Git tree is not clean. Commit or stash changes first."
}

remote_add_or_set() {
  local root=$1 name=$2 url=$3
  if git -C "$root" remote get-url "$name" >/dev/null 2>&1; then
    git -C "$root" remote set-url "$name" "$url"
  else
    git -C "$root" remote add "$name" "$url"
  fi
}

clone_or_update() {
  local url=$1 branch=$2 dir=$3
  if [[ -d "$dir/.git" ]]; then
    log "Updating $(basename "$dir")"
    git -C "$dir" remote set-url origin "$url"
    git -C "$dir" fetch --filter=blob:none --depth=1 origin "$branch"
    git -C "$dir" checkout -B source FETCH_HEAD
  else
    log "Cloning $(basename "$dir")"
    git clone --filter=blob:none --depth=1 --single-branch --branch "$branch" "$url" "$dir"
    git -C "$dir" branch -M source
  fi
}

sha256_file() {
  if have sha256sum; then sha256sum "$@"; else shasum -a 256 "$@"; fi
}

#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
set -Eeuo pipefail

install=${1:-}
if [[ "$install" != "--install" ]]; then
  cat <<'MSG'
This script can install kernel-build dependencies using the detected package manager.
Review the command, then run:
  tools/oneplus15-port/install-deps.sh --install
MSG
  exit 0
fi

if command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update
  sudo apt-get install -y \
    git make clang lld llvm flex bison bc python3 rsync cpio gzip xz-utils zstd \
    libssl-dev libelf-dev libncurses-dev dwarves device-tree-compiler kmod ccache \
    android-sdk-platform-tools-common adb fastboot gh curl ca-certificates
elif command -v dnf >/dev/null 2>&1; then
  sudo dnf install -y \
    git make clang lld llvm flex bison bc python3 rsync cpio gzip xz zstd \
    openssl-devel elfutils-libelf-devel ncurses-devel dwarves dtc kmod ccache \
    android-tools gh curl ca-certificates
elif command -v pacman >/dev/null 2>&1; then
  sudo pacman -Syu --needed \
    git make clang lld llvm flex bison bc python rsync cpio gzip xz zstd \
    openssl libelf ncurses pahole dtc kmod ccache android-tools github-cli curl ca-certificates
else
  echo "Unsupported package manager. Install the tools listed by 'portctl doctor'." >&2
  exit 2
fi

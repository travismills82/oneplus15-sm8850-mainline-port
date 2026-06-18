# Building the OnePlus 15 mainline port

This guide covers compile-only development. It does not describe flashing or replacing any device partition.

## Host requirements

The bring-up helper checks for these required tools:

```text
git make clang ld.lld llvm-ar llvm-nm llvm-objcopy
flex bison bc python3 rsync cpio gzip xz zstd
openssl pahole
```

Optional tools used by some workflows:

```text
adb fastboot gh ccache dtc
```

The OEM source mirrors and build output can consume more than 60 GiB. Use a filesystem with enough free space before running `sync` or `all`.

Check the host:

```bash
./tools/oneplus15-port/portctl doctor
```

When dependencies are missing, the repository helper can install the expected packages on supported hosts:

```bash
./tools/oneplus15-port/install-deps.sh --install
```

Review an installation script before running it with elevated privileges.

## Source synchronization

Fetch the Linux stable baseline, Android Common Kernel reference, and the three public OnePlus source trees:

```bash
./tools/oneplus15-port/portctl sync
```

The command stores reference trees under:

```text
.port-work/
```

It also writes a reproducibility lock file:

```text
.port-work/refs.lock
```

Do not commit `.port-work/`.

## Configure

Generate the mainline configuration:

```bash
./tools/oneplus15-port/portctl configure
```

Default output directory:

```text
out/oneplus15-mainline/
```

The configuration process starts with an upstream ARM64 configuration and merges only symbols that exist in the target mainline tree. Toolchain-generated values, signing-key paths, `WERROR`, KMI trimming, and other unsafe stock-config entries are filtered.

## Build

Build the ARM64 kernel, modules, and available DTBs with LLVM:

```bash
./tools/oneplus15-port/portctl build
```

Run the complete host-check, sync, configure, and build sequence with:

```bash
./tools/oneplus15-port/portctl all
```

The project has previously completed a non-Bazel Linux `v6.12.23` ARM64 build using LLVM/Kbuild. That milestone validates the build environment only; it does not validate hardware boot.

## Useful environment overrides

```text
ONEPLUS15_WORK_DIR
ONEPLUS15_OUT_DIR
ONEPLUS15_ARTIFACT_DIR
ONEPLUS15_SNAPSHOT_DIR
CONFIG_SOURCE
MERGE_STOCK_CONFIG=1
JOBS
LLVM_PREFIX
```

Example:

```bash
JOBS=16 \
ONEPLUS15_OUT_DIR="$PWD/out/canoe-test" \
./tools/oneplus15-port/portctl build
```

## Reports

Generate a comparison with the OEM common kernel:

```bash
./tools/oneplus15-port/portctl oem-report
```

Generate the historical `sun` symbol/file gap report:

```bash
./tools/oneplus15-port/portctl sun-gap-report
```

The command name is retained for the original analysis workflow. The OnePlus 15 product DT target is Canoe, so new platform work must be validated against `qcom,canoe-*` compatibles.

Reports and build artifacts are written under:

```text
artifacts/oneplus15/
```

## Device snapshots

The following command captures read-only facts from an authorized ADB device. Root access is required for the complete snapshot:

```bash
./tools/oneplus15-port/portctl snapshot
```

The stock-image helper reads boot-related partitions into local files and does not write them:

```bash
./tools/oneplus15-port/portctl stock-images
```

Treat all captured device data as private. Review files before publishing them.

## Manual object probes

During subsystem porting, compile one object before adding permanent Kconfig integration:

```bash
make \
  O=out/oneplus15-mainline \
  ARCH=arm64 \
  LLVM=1 \
  LLVM_IAS=1 \
  V=1 \
  -j1 \
  drivers/pinctrl/qcom/pinctrl-canoe.o
```

This keeps the first failure focused on the imported driver rather than unrelated modules.

## Cleaning

Remove generated output without touching source:

```bash
rm -rf out/oneplus15-mainline
rm -rf artifacts/oneplus15
```

Removing `.port-work/` also removes all reference source mirrors and requires a complete re-sync.

## Build success criteria

A useful compile milestone requires all of the following:

1. The command exits with status zero.
2. `git diff --check` passes.
3. The target object or image exists and is non-empty.
4. The complete build log is retained.
5. No downstream core header was copied over a mainline header.
6. The result is clearly labeled compile-only until tested on hardware.

## Flashing

There is intentionally no flashing workflow in this documentation. Do not flash the current artifacts. Hardware testing should begin only after a minimal Canoe DTS, early-console configuration, boot-image packaging procedure, rollback plan, and device-specific recovery process have all been independently verified.
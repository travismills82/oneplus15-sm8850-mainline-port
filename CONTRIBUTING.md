# Contributing

Contributions are welcome when they keep the bring-up incremental, reviewable, and honest about validation status.

## Before starting

Read:

- [README.md](README.md)
- [docs/STATUS.md](docs/STATUS.md)
- [docs/ROADMAP.md](docs/ROADMAP.md)
- [docs/BUILDING.md](docs/BUILDING.md)
- [docs/GIT_WORKFLOW.md](docs/GIT_WORKFLOW.md)

Check whether the subsystem is already assigned to a bring-up branch or blocked by an earlier dependency.

## Development principles

1. Use upstream Linux interfaces whenever possible.
2. Do not replace mainline framework headers with downstream Qualcomm versions.
3. Port one hardware block at a time.
4. Compile imported source before adding permanent Kconfig integration.
5. Keep generated output and OEM source mirrors outside commits.
6. Clearly separate compile success from hardware validation.
7. Do not add flashing instructions until a reviewed recovery and rollback process exists.

## Branches

Start from a compact published branch, not a local full-history backup:

```bash
git fetch port-origin --prune
git switch -c bringup/<subsystem> port-origin/main
```

See [docs/GIT_WORKFLOW.md](docs/GIT_WORKFLOW.md) before pushing. A branch that starts enumerating millions of objects is based on the wrong history.

## Importing downstream code

When using the public OnePlus/Qualcomm sources as a reference:

- retain SPDX identifiers and copyright notices;
- preserve register values and tables only when their provenance is clear;
- remove unused vendor-only features;
- adapt structures to mainline APIs;
- avoid importing debug, minidump, CRM, Android vendor hooks, or proprietary framework dependencies unless independently justified;
- document substantial semantic changes in the commit message.

A downstream file compiling after downstream core headers are copied into mainline is not considered a successful port.

## Coding style

Follow the Linux kernel coding style and the conventions of the target subsystem.

Before submitting:

```bash
git diff --check
./scripts/checkpatch.pl --strict --codespell <your-patch-file>
```

Use existing nearby upstream drivers as the primary style reference.

## Commit messages

Use the kernel subsystem prefix:

```text
pinctrl: qcom: add Canoe TLMM support
clk: qcom: add Canoe GCC support
interconnect: qcom: add Canoe NoC support
phy: qcom: add Canoe UFS PHY configuration
arm64: dts: qcom: add OnePlus 15 initial support
```

The body should state:

- the hardware block being added;
- the compatible string;
- the main downstream-to-upstream API differences;
- the exact build or validation performed;
- remaining limitations.

Do not claim hardware support based only on object compilation.

## Minimum testing

### Driver-only change

```bash
make \
  O=out/oneplus15-mainline \
  ARCH=arm64 \
  LLVM=1 \
  LLVM_IAS=1 \
  V=1 \
  -j1 \
  path/to/driver.o
```

Also run:

```bash
git diff --check
```

### Kconfig or Makefile integration

Enable the symbol, run `olddefconfig`, and verify the final value:

```bash
./scripts/config \
  --file out/oneplus15-mainline/.config \
  --enable YOUR_CONFIG_SYMBOL

make O=out/oneplus15-mainline ARCH=arm64 LLVM=1 LLVM_IAS=1 olddefconfig
grep '^CONFIG_YOUR_CONFIG_SYMBOL=' out/oneplus15-mainline/.config
```

Then build the integrated object or subsystem.

### Device-tree change

Build the target DTB and run schema validation where possible:

```bash
make O=out/oneplus15-mainline ARCH=arm64 LLVM=1 LLVM_IAS=1 dtbs
make O=out/oneplus15-mainline ARCH=arm64 LLVM=1 LLVM_IAS=1 dtbs_check
```

Document existing unrelated warnings separately from warnings introduced by the change.

### Full build

```bash
./tools/oneplus15-port/portctl build
```

Retain the complete log, final `.config`, compiler version, and artifact hashes.

## Pull request checklist

- [ ] Branch is based on compact repository history.
- [ ] Change is limited to one logical subsystem or dependency series.
- [ ] No generated build output is committed.
- [ ] No downstream core header replaces a mainline header.
- [ ] `git diff --check` passes.
- [ ] Relevant object, DTB, or full image builds.
- [ ] Compiler and commands are documented.
- [ ] Compile-only and hardware-tested claims are clearly distinguished.
- [ ] `docs/STATUS.md` is updated when status changes.
- [ ] New bindings include schema documentation.
- [ ] No private device snapshots, keys, serial numbers, or proprietary blobs are included.

## Hardware testing

Hardware tests require extra care:

- use a noncritical test device;
- preserve stock boot-related partitions first;
- verify slot and boot-chain behavior;
- have a tested recovery path;
- begin with temporary boot methods when supported;
- capture serial, ramoops, or bootloader logs;
- never present an untested image as safe to flash.

Until the repository documents a reviewed boot-image and recovery process, contributions should remain compile and source-integration focused.

## Reporting problems

A useful issue or PR report includes:

- branch and commit SHA;
- host distribution;
- compiler and linker versions;
- exact command;
- complete first failure;
- relevant `.config` symbols;
- whether the result was compile-only or hardware-tested;
- logs as attachments rather than screenshots when possible.

Remove device serial numbers, account information, private keys, tokens, and proprietary binaries before sharing logs.
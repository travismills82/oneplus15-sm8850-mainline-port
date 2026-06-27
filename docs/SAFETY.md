# Device safety and hardware testing

This repository is a development tree. Compilation, DTB generation, and even a successful temporary boot do not prove that the kernel is safe for normal device use.

The included project tools are designed not to write phone partitions. Keep that property until a known-good, device-specific recovery path has been independently verified.

## Current policy

- Do not flash repository artifacts.
- Do not replace the active stock boot chain.
- Do not test on a device containing irreplaceable data.
- Do not distribute an image as bootable until packaging, rollback, and recovery procedures have been reviewed and exercised.
- Prefer source, object, DT schema, and compile testing until early-console support is ready.

## Risks

An incomplete platform port can cause:

- a nonbooting device or boot loop;
- inaccessible recovery modes;
- storage corruption or data loss;
- incorrect regulator, clock, or interconnect programming;
- overheating because thermal controls are absent;
- battery drain or charging faults;
- radio and peripheral malfunction.

The absence of visible errors does not mean power, thermal, or storage behavior is correct.

## Before testing any kernel

1. Record the exact OxygenOS or ColorOS build, active slot, bootloader state, and anti-rollback state.
2. Back up `boot`, `init_boot`, `vendor_boot`, `vendor_kernel_boot`, `dtbo`, available `vbmeta*` partitions, and `recovery` from the running slot with `portctl stock-images`.
3. Save hashes and a second offline copy of those images.
4. Confirm that the device can enter bootloader or fastboot mode and that the host detects it before changing anything.
5. Verify the exact boot-image format and device-specific packaging process.
6. Prefer a temporary boot only when the bootloader and image format explicitly support it. Do not assume `fastboot boot` is available or safe.
7. Establish serial, ramoops, pstore, or another retained logging method.
8. Use a minimal device tree with nonessential hardware disabled.
9. Have a reviewed rollback procedure before running any command that writes a partition.

Treat rollback and cross-version flashing as potentially irreversible until verified for the exact device and firmware build. Do not disable AVB, alter vbmeta, change slots, downgrade firmware, or flash a generated image as part of an automated bring-up script.

## Read-only data collection

The project helper provides read-only capture workflows:

```bash
./tools/oneplus15-port/portctl snapshot
./tools/oneplus15-port/portctl stock-images
```

The stock-image command reads boot-related partitions into local files. Review the implementation and output paths before use.

Device snapshots may contain:

- serial numbers;
- build fingerprints;
- partition information;
- kernel command lines;
- kernel symbols and module lists;
- hardware identifiers.

Remove private information before publishing a report.

## Debugging sequence

Use the least invasive evidence first:

1. Compile and link logs.
2. `dtc` warnings and DT schema validation.
3. Individual driver-object probes.
4. QEMU or generic ARM64 build sanity where applicable.
5. Temporary boot, only when explicitly supported.
6. Earlycon or serial output.
7. Pstore or ramoops after a failed boot.
8. Stock-kernel module, symbol, and device-tree comparisons.

## Build artifacts

Artifacts under `out/` and `artifacts/` are development output. Label retained artifacts with:

- source commit;
- configuration hash;
- compiler version;
- build date;
- compile-only or hardware-tested status.

Never infer flash safety from a valid `Image`, DTB, checksum, or successful module build.

## Early boot strategy

The first hardware-oriented target should be early serial output with as little hardware enabled as possible:

- CPU and interrupt controller;
- timer and PSCI;
- RPMh and command-database dependencies;
- Canoe GCC clocks required by serial;
- TLMM state required by the console;
- one GENI serial node;
- no display, camera, modem, audio, charging policy, or advanced power management.

Storage should be enabled only after serial output is stable and UFS dependencies are validated.

## Storage precautions

The first UFS validation should be read-only:

- confirm PHY and host probe;
- identify the controller and logical units;
- retain logs across repeated cold boots;
- avoid mounting writable filesystems;
- avoid filesystem repair tools;
- avoid writing partition tables or metadata.

Write testing belongs after the driver is repeatable and the recovery path has been exercised.

## Thermal precautions

A kernel without complete thermal support must not be used for sustained workloads. Keep early tests brief and stop immediately if the device becomes unexpectedly warm.

Do not enable high-performance CPU, GPU, display, charging, or camera paths before thermal sensors and conservative controls are available.

## Reporting hardware results

A hardware-test report should state:

- exact commit and branch;
- boot method;
- whether any partition was written;
- device slot;
- image packaging method;
- kernel command line;
- serial or retained logs;
- observed temperature or power anomalies;
- recovery result;
- whether the device returned to stock successfully.

Use the terms `boot tested` or `working` only when supported by repeatable hardware evidence.

## Emergency rule

When a command unexpectedly begins writing, targets an uncertain block device, or differs from the reviewed procedure, stop rather than guessing. Preserve logs and re-verify the partition map and recovery process before continuing.

Never use the only daily-use phone as the sole recovery device or credential holder during bring-up.
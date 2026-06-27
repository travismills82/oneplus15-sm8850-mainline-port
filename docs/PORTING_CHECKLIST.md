# Porting checklist

## Baseline

- [ ] `portctl doctor` passes.
- [ ] `portctl build` produces `artifacts/oneplus15/build/Image`.
- [ ] GitHub Actions passes on the clean branch.
- [ ] `portctl sync` records all source SHAs.

## Device evidence

- [ ] Rooted `portctl snapshot` completed.
- [ ] Stock boot-related images saved and hashed.
- [ ] Running `.config`, bootconfig, cmdline, modules, Kallsyms, DT, and slot saved.
- [ ] Exact firmware build and rollback constraints documented.

## Early platform

- [ ] SM8850 machine/SoC DT skeleton builds with `dtbs_check`.
- [ ] GIC, timer, PSCI, reserved memory, and early console are correct.
- [ ] RPMh/regulators, clocks, resets, pinctrl, and interconnect are represented.
- [ ] UFS and USB can initialize without vendor-only hacks.
- [ ] ramoops/pstore survives a failed test boot.

## Android integration

- [ ] Binder/binderfs, namespaces, cgroups, SELinux, dm-verity, F2FS, and required
      crypto options are enabled.
- [ ] Boot image header, page size, ramdisk arrangement, and DTB/DTBO placement
      match the exact firmware.
- [ ] Module signing, compression, load order, and partition placement are known.
- [ ] GKI KMI-dependent modules are either ported, replaced, or intentionally omitted.

## Hardware phases

- [ ] storage and filesystems
- [ ] charging, battery, thermal, and suspend
- [ ] display and touch
- [ ] GPU
- [ ] audio
- [ ] WLAN/Bluetooth
- [ ] sensors and haptics
- [ ] camera
- [ ] modem/data path

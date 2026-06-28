# OnePlus 15 SM8850 Mainline Linux Port

Experimental Linux mainline bring-up for the OnePlus 15 based on Qualcomm SM8850 / Canoe.

> **Warning**
>
> This is a development tree. The current kernel and device-tree work are not ready for general flashing or daily use. A successful compile or schema check does not prove that an image is safe to boot. Keep a known-good recovery path and treat every hardware test as a separately reviewed operation.

## Active baseline

| Item | Value |
|---|---|
| Device | OnePlus 15 |
| Qualcomm platform | SM8850 |
| Device-tree family | Canoe |
| OnePlus overlay family | Infiniti |
| Active mainline baseline | Linux `v7.1.1` |
| Active development branch | `bringup/canoe-v7.1` |
| Primary architecture | ARM64 |
| Toolchain | LLVM / Clang with integrated assembler |
| OEM source generation | Android 16 / Linux `6.12.23` |

The OEM 6.12.23 source is used only as a hardware and implementation reference. Active development, validation, and future boot artifacts are based on Linux 7.1.1.

## Project goals

- Bring the OnePlus 15 hardware platform up on a clean Linux 7.1.1 baseline.
- Replace downstream-only Qualcomm and Android interfaces with upstream Linux APIs.
- Port one hardware block at a time in focused, independently reviewable commits.
- Validate bindings and changed translation units before enabling them in the device tree.
- Reach a reproducible minimal boot before enabling display, camera, audio, wireless, modem, or other complex subsystems.
- Keep physical-device operations separate from source and compile validation.

## Current status

### Integrated and compile-validated on Linux 7.1.1

- Clean compact Linux 7.1.1 repository baseline.
- Canoe TLMM/pinctrl driver and binding.
- Canoe global clock controller.
- Canoe TCSR reference clock controller.
- Canoe HLOS-voted RPMh interconnect fabrics.
- Canoe UFS Gear 5 QMP PHY data and binding.
- Canoe UFS host binding with ICE and MCQ register resources.
- Linux 7.1 Qualcomm UFS host MCQ resource handling verified without downstream host-driver changes.

The merged subsystem work has been validated with targeted DT schema checks where applicable and ARM64 LLVM builds using `LLVM=1`, `LLVM_IAS=1`, and `W=1`. These results are compile-only unless explicitly stated otherwise.

### In progress

- Minimal Canoe RPMh clock descriptor.
- PMH0110 RPMh regulator rail table and compatible match.
- Minimal Canoe SoC device tree.
- OnePlus 15 board device tree.
- Targeted `dtbs_check` cleanup.
- Built-in bring-up kernel configuration and complete kernel/DTB build.

### Not yet validated on Linux 7.1.1 hardware

- Successful physical boot.
- Early console output.
- UFS storage enumeration or access.
- USB, display, GPU, camera, audio, modem, Wi-Fi, Bluetooth, sensors, charging, suspend, or thermal management.
- Android userspace compatibility.

Earlier Linux 6.12 hardware experiments are historical evidence only and are not Linux 7.1.1 boot validation.

## Branch model

| Branch | Purpose |
|---|---|
| `main` | Current documented Linux 7.1.1 project entry point |
| `bringup/canoe-v7.1` | Active Linux 7.1.1 integration branch |
| `port/*-v7.1` | Temporary subsystem validation branches; remove after merge |
| `codex/*` | Optional local or remote automation branches based on Linux 7.1.1 |

Legacy Linux 6.12.23 development branches are not part of the active branch model. Historical source provenance is identified by immutable commit IDs rather than long-lived development branches.

Because this repository uses compact snapshot history, new development branches must be based on the compact Linux 7.1.1 project history. Do not publish branches containing the full upstream Linux history.

## Repository layout

```text
arch/arm64/boot/dts/qcom/       Canoe and OnePlus 15 device-tree work
drivers/clk/qcom/               Qualcomm clock-controller support
drivers/interconnect/qcom/      Qualcomm NoC/interconnect support
drivers/pinctrl/qcom/           Canoe TLMM support
drivers/phy/qualcomm/           Canoe UFS and future PHY support
drivers/regulator/              RPMh regulator support
tools/oneplus15-port/           Bring-up, validation, and reporting helpers
artifacts/oneplus15/             Generated local reports and build artifacts
.port-work/                      Local reference-source mirrors; never commit
```

Generated output directories are intentionally excluded from source commits.

## Validation requirements

For every source change:

```bash
git diff --check
```

Changed C translation units must be built for ARM64 with LLVM:

```bash
make O=out/oneplus15 \
  ARCH=arm64 \
  LLVM=1 \
  LLVM_IAS=1 \
  W=1 \
  <object-target>
```

Treat compiler warnings as failures.

For binding changes, run a targeted `dt_binding_check`. For DTS changes, build the exact DTB and run targeted `dtbs_check`. Record the commands, warnings, errors, output hashes, and whether the result is compile-only or hardware-tested.

## Hardware safety policy

- Do not flash compile-only artifacts.
- Slot B is the stock recovery path and must not be modified.
- Slot A is test-only.
- Sparse Fastboot writes require `-S 16M`.
- Do not repeat a slot-A boot without a new evidence channel or changed diagnostic.
- Do not use direct watchdog MMIO.
- ADB, Fastboot, flashing, slot changes, and reboots are manual operations outside automated source-porting workflows.

Read [docs/SAFETY.md](docs/SAFETY.md) before considering a device test.

## Documentation

- [Linux 7.1 migration notes](PORTING-7.1.md)
- [Build and report workflow](docs/BUILDING.md)
- [Detailed subsystem status](docs/STATUS.md)
- [Staged bring-up roadmap](docs/ROADMAP.md)
- [Compact-history Git workflow](docs/GIT_WORKFLOW.md)
- [Device safety and hardware-testing policy](docs/SAFETY.md)
- [Source references and provenance](docs/SOURCE_REFERENCES.md)
- [Contribution and validation guidelines](CONTRIBUTING.md)

## Source provenance

The bring-up compares Linux 7.1.1 with public Android Common Kernel and OnePlus SM8850 sources. Those downstream repositories are reference material only; this project does not aim to reproduce the Android vendor kernel wholesale.

The final Linux 6.12.23 development snapshot used during migration is identified by commit:

```text
5555043e466f5a12dbc029254883f3f8ed7061cd
```

Do not base new development on that historical snapshot.

## License

The Linux kernel source remains licensed under GPL-2.0-only unless an individual file states otherwise. Imported or adapted files retain their original SPDX identifiers and copyright notices.

## Disclaimer

This is an independent development project and is not affiliated with or endorsed by OnePlus, Qualcomm, Google, or the Linux kernel project.

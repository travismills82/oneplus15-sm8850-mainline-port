# OnePlus 15 SM8850 Mainline Linux Port

> **Linux 7.1 migration**
>
> This branch is restarting the hardware port on the clean Linux `v7.1.1` baseline. The earlier Linux 6.12 work is preserved and is being replayed subsystem by subsystem. See [PORTING-7.1.md](PORTING-7.1.md).


Experimental Linux mainline bring-up for the OnePlus 15 platform based on Qualcomm SM8850 / Canoe.

> **Warning**
>
> This repository is a development tree. The current kernel and device-tree work are **not ready to flash**. A successful compile does not prove that an image is safe to boot on hardware. Keep a known-good recovery path and do not use generated artifacts on a daily-use device.

## Project goals

- Bring the OnePlus 15 hardware platform up on a clean Linux `v7.1.1` baseline.
- Replace downstream-only Qualcomm and Android interfaces with upstream Linux APIs.
- Keep each hardware block in a reviewable, independently testable commit.
- Reach early serial output before enabling storage, display, camera, audio, wireless, or other complex subsystems.
- Preserve reproducible reports for every major compatibility and build milestone.

## Platform identity

| Item | Value |
|---|---|
| Device | OnePlus 15 |
| Qualcomm platform | SM8850 |
| Device-tree family | Canoe |
| OnePlus device-tree overlays | Infiniti family |
| Mainline baseline | Linux `v7.1.1` |
| OEM kernel generation | Android 16 / Linux `7.1.1` |
| Primary architecture | ARM64 |
| Toolchain | LLVM / Clang with integrated assembler |

The OEM source contains both `sun` and `canoe` targets. The OnePlus product mapping selects the Canoe DTB family, so this port treats `qcom,canoe-*` compatibles as the authoritative hardware descriptions for the device.

## Current status

### Completed or validated

- Clean Linux `v7.1.1` source baseline established.
- Non-Bazel ARM64 kernel build completed with LLVM/Kbuild.
- Build configuration and OEM symbol-gap reporting automated.
- Full downstream `qcom/canoe.dts` include closure analyzed.
- Downstream Canoe DTS preprocesses and compiles with `dtc`; this is a dependency milestone, not a mainline-ready DTB.
- Canoe TLMM/pinctrl driver adapted to the Linux 6.12 Qualcomm pinctrl interface and compiled without warnings.
- Canoe UFS PHY-specific source compiled in isolation after supplying its downstream private headers; the shared framework still requires separate validation.

### In progress

- Permanent Canoe pinctrl integration and publication.
- Canoe GCC adaptation from downstream voltage-voting structures to mainline clock APIs.
- Canoe interconnect conversion from downstream QoS structures to mainline RPMh interconnect data.
- UFS PHY migration to the mainline QMP UFS framework.
- Minimal mainline Canoe DTS for early console and core SoC resources.

### Not yet validated

- Booting on physical hardware.
- Early console output from a mainline-built image.
- UFS storage access.
- USB, display, GPU, camera, audio, modem, Wi-Fi, Bluetooth, sensors, charging, suspend, or thermal management.
- Android userspace compatibility.

## Documentation

- [Build and report workflow](docs/BUILDING.md)
- [Detailed subsystem status](docs/STATUS.md)
- [Staged bring-up roadmap](docs/ROADMAP.md)
- [Compact-history Git workflow](docs/GIT_WORKFLOW.md)
- [Device safety and hardware-testing policy](docs/SAFETY.md)
- [Source references and provenance](docs/SOURCE_REFERENCES.md)
- [Contribution and validation guidelines](CONTRIBUTING.md)

## Repository layout

```text
arch/arm64/boot/dts/qcom/       Mainline device-tree work
drivers/clk/qcom/               Qualcomm clock controller work
drivers/interconnect/qcom/      Qualcomm NoC/interconnect work
drivers/pinctrl/qcom/           Canoe TLMM support
drivers/phy/qualcomm/           Canoe UFS/USB PHY work
tools/oneplus15-port/           Bring-up and reporting helpers
artifacts/oneplus15/             Generated local reports and build artifacts
.port-work/                      Local OEM source mirrors; never commit this directory
```

Generated output directories are intentionally excluded from source commits.

## Branch model

| Branch | Purpose |
|---|---|
| `main` | Compact, documented source snapshot and stable project entry point |
| `bringup/canoe-core-probe` | Experimental core-driver compatibility work |
| `bringup/canoe-ufs` | Planned UFS framework and PHY validation |
| `backup/*-full-history` | Local-only safety branches retaining the original Linux history |

Because this repository was created from a compact source snapshot, development branches should remain based on the repository's compact `main` history. Pushing a branch based on the full upstream Linux history may attempt to enumerate and upload millions of objects. Read [docs/GIT_WORKFLOW.md](docs/GIT_WORKFLOW.md) before publishing a development branch.

## Getting started

Run the host check first:

```bash
./tools/oneplus15-port/portctl doctor
```

Typical setup from the repository root:

```bash
./tools/oneplus15-port/portctl sync
./tools/oneplus15-port/portctl configure
./tools/oneplus15-port/portctl build
```

The exact helper commands available may change as the bring-up tooling evolves. Run the following to inspect the current interface:

```bash
./tools/oneplus15-port/portctl help
```

Read [docs/BUILDING.md](docs/BUILDING.md) for prerequisites, environment overrides, reports, and build-success criteria.

## Development rules

1. Do not flash compile-only artifacts.
2. Keep imported OEM code uncommitted until it compiles against the mainline tree.
3. Do not replace mainline core headers with downstream versions.
4. Port one subsystem at a time.
5. Preserve the first complete compiler log for every failed probe.
6. Keep generated reports out of source commits unless they document a reproducible milestone.
7. Prefer mainline abstractions over carrying downstream support frameworks.
8. Update `docs/STATUS.md` whenever a subsystem milestone changes.

See [CONTRIBUTING.md](CONTRIBUTING.md) for commit and testing expectations and [docs/SAFETY.md](docs/SAFETY.md) before considering any device test.

## Source references

The bring-up work compares this tree with public Android Common Kernel and OnePlus SM8850 sources. Those repositories are references only; this project does not aim to reproduce the downstream kernel wholesale. Exact source roles, branch names, revision locking, and import rules are documented in [docs/SOURCE_REFERENCES.md](docs/SOURCE_REFERENCES.md).

## License

The Linux kernel source remains licensed under GPL-2.0-only unless an individual file states otherwise. Imported or adapted files retain their original SPDX identifiers and copyright notices.

## Disclaimer

This is an independent development project and is not affiliated with or endorsed by OnePlus, Qualcomm, Google, or the Linux kernel project.
# Project status

Last documentation review: June 2026.

This document distinguishes source analysis, successful compilation, and hardware validation. These are separate milestones.

## Milestone definitions

| State | Meaning |
|---|---|
| Not started | No focused porting work has been completed. |
| Analyzed | OEM source, DT, or configuration dependencies have been identified. |
| Compiles | The relevant source or object compiles against this Linux baseline. |
| Integrated | Kconfig/Makefile and required bindings are included in a reviewable commit. |
| DT described | A reduced mainline-style device-tree description exists. |
| Boot tested | The code has been exercised on physical hardware with retained logs. |
| Working | The subsystem is usable and repeatably tested. |

## Core project milestones

| Milestone | State | Notes |
|---|---|---|
| Linux baseline | Integrated | Linux `v6.12.23` snapshot. |
| ARM64 LLVM build | Compiles | Non-Bazel Kbuild baseline completed. |
| OEM source synchronization | Integrated | ACK plus public OnePlus common, SoC, and modules/DT trees. |
| OEM reference lock | Integrated | `.port-work/refs.lock` records source revisions locally. |
| Canoe DT dependency closure | Compiles | Downstream `qcom/canoe.dts` preprocesses and compiles with warnings. |
| Minimal mainline Canoe DTB | Analyzed | Must be reduced from the downstream hierarchy. |
| Early serial console | Not started | OEM build data identifies GENI earlycon at `0x00a9c000`; not validated with mainline. |
| Physical boot | Not started | No generated image is approved for flashing. |

## Subsystem matrix

| Subsystem | State | Current finding | Next requirement |
|---|---|---|---|
| TLMM / pinctrl | Compiles | Canoe driver adapted to mainline wake IRQ and PM interfaces; object compiles without warnings. | Permanent integration, binding review, and DT node. |
| Global clock controller | Analyzed | OEM `gcc-canoe.c` imports downstream voltage-level and clock-regulator plumbing absent from mainline. | Remove or translate downstream VDD voting while preserving functional clock data. |
| RPMh interconnect | Analyzed | OEM Canoe NoC data uses downstream QoS and multi-voter structures that conflict with mainline `icc-rpmh`. | Convert descriptors to mainline node, QoS, BCM, and voter structures. |
| UFS PHY | Compiles in isolation | Canoe-specific object compiles after adding its downstream private interface headers. | Port calibration tables into mainline QMP UFS framework; avoid carrying the downstream framework unchanged. |
| UFS host | Analyzed | Generic mainline Qualcomm UFS support exists. | Add Canoe clocks, power domains, PHY, interconnect, and DT wiring. |
| Device tree | Analyzed | Full downstream include closure contains 44 resolved files and no missing includes. | Create a minimal upstream-style SoC and board DTS rather than importing all downstream files. |
| Interrupt controller | Mainline available | ARM GIC support is present. | Verify Canoe interrupt IDs in minimal DTS. |
| PDC / wake IRQs | Analyzed | Canoe PDC map is present in OEM pinctrl data. | Validate wake-parent and interrupt routing after boot. |
| RPMh / command DB | Mainline available | Generic Qualcomm framework exists. | Add Canoe-compatible resource descriptions. |
| Regulators / PMIC | Analyzed | Downstream DT contains extensive PMIC dependencies. | Defer nonessential rails; add only early-boot supplies first. |
| CPU topology | Analyzed | Downstream DT data available. | Reduce to boot-safe CPU, timer, PSCI, and idle definitions. |
| CPU frequency | Not started | Not needed for first serial boot. | Add only after stable boot. |
| SMMU / IOMMU | Mainline available | Generic Qualcomm SMMU support exists. | Add stream mappings incrementally as clients are enabled. |
| USB | Not started | Downstream nodes and PHY references identified. | Defer until clocks, pinctrl, and power domains are stable. |
| PCIe | Not started | Downstream nodes identified. | Defer until core NoC/clock support works. |
| Display / GPU | Not started | Requires additional clock, power, IOMMU, and firmware work. | Defer until storage and serial are stable. |
| Camera | Not started | Strong dependency on downstream-only frameworks. | Long-term task; do not include in initial boot series. |
| Audio | Not started | Requires LPASS, codecs, clocks, and board routing. | Defer. |
| Modem / remoteproc | Not started | Firmware and protection-domain dependencies unresolved. | Defer. |
| Wi-Fi / Bluetooth | Not started | Firmware and bus dependencies unresolved. | Defer. |
| Sensors / haptics | Not started | Board-specific peripherals. | Defer. |
| Charging / battery | Not started | PMIC and vendor policy dependencies. | Defer. |
| Thermal | Not started | Must be addressed before sustained hardware use. | Add after basic boot, before performance testing. |
| Suspend / resume | Not started | Requires stable clocks, PDC, RPMh, and PM domains. | Late bring-up milestone. |

## Known incompatibility classes

### Pinctrl

The downstream driver stores wake register offsets directly in each pin group and includes downstream QUP/I3C mode-register metadata. Linux 6.12 uses the existing Qualcomm PDC wake IRQ map and does not expose those downstream fields. The initial adaptation therefore:

- retains `canoe_pdc_map`;
- removes `wake_reg` and `wake_bit` initializers;
- omits downstream-only QUP mode-register metadata for the first GPIO/IRQ milestone;
- uses the mainline Qualcomm pinctrl PM operations.

### GCC

The downstream driver assumes:

- `vdd-level.h` and downstream VDD classes;
- `clk_regmap.vdd_data`;
- clock-regulator arrays in `qcom_cc_desc`;
- additional PLL and branch operations not guaranteed in Linux 6.12.

Do not overwrite mainline Qualcomm clock headers to satisfy these dependencies. Adapt the Canoe clock data to existing mainline APIs.

### Interconnect

The downstream driver uses:

- `qnoc-qos.h` structures that redefine names already present in mainline;
- multiple voters and CRM-specific fields;
- node fields such as local clocks, disabled flags, initial bandwidth, and downstream QoS operations.

The driver must be converted, not built by replacing mainline `icc-rpmh.h`.

### UFS PHY

The downstream Canoe driver is based on a separate Qualcomm UFS PHY framework. Linux 6.12 uses the unified QMP UFS implementation. The preferred port is to add Canoe calibration/configuration data and compatible matching to the mainline QMP driver.

## Current boot target

The next meaningful hardware-oriented artifact is a minimal Canoe DTB containing only:

- model and compatible strings;
- chosen node with early console;
- memory description supplied by firmware or boot environment;
- CPUs, PSCI, timer, GIC;
- RPMh, command DB, AOSS, PDC, TLMM;
- GCC and only clocks needed for serial and storage;
- serial engine at the OEM earlycon address;
- UFS host and PHY after those drivers compile and integrate.

Everything else should remain disabled until early serial output is repeatable.

## Evidence retention

For each milestone, retain:

- exact source revisions;
- `.config`;
- compiler version;
- complete build log;
- DT compiler warnings;
- artifact hashes;
- boot command line;
- serial or ramoops logs when hardware testing begins.

Never mark a subsystem as working solely because its source compiles.
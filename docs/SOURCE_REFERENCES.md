# Source references and provenance

This project uses public source repositories as technical references. Exact revisions are recorded locally by `portctl sync` in `.port-work/refs.lock`.

## Mainline baseline

- Linux stable tag: `v6.12.23`

The baseline defines the upstream APIs that new code must target. Downstream framework headers must not replace their mainline counterparts merely to make OEM code compile.

## Android Common Kernel reference

- Repository: Android Common Kernel
- Branch: `android16-6.12`

This source helps distinguish generic Android Common Kernel changes from Qualcomm and OnePlus platform changes.

## OnePlus public sources

All three public OnePlus repositories use this project branch:

```text
oneplus/sm8850_b_16.0.0_oneplus_15
```

Reference repositories:

- `OnePlusOSS/android_kernel_common_oneplus_sm8850`
- `OnePlusOSS/android_kernel_oneplus_sm8850`
- `OnePlusOSS/android_kernel_modules_and_devicetree_oneplus_sm8850`

## Platform identification

The public source set contains multiple Qualcomm targets, including `sun` and `canoe`. The OnePlus-specific platform map selects the Canoe DTB family and Infiniti overlays for the relevant device configuration.

For this project:

```text
Kernel target: canoe
DT compatibles: qcom,canoe-*
OnePlus overlay family: infiniti
```

The historical `sun-gap-report` helper remains useful for earlier analysis, but new device-specific work should use Canoe sources and compatibles.

## Local reference-tree locations

After synchronization:

```text
.port-work/ack-common/
.port-work/oneplus-common/
.port-work/oneplus-soc/
.port-work/oneplus-modules/
.port-work/refs.lock
```

These directories are local inputs and must not be added to source commits.

## Recording exact revisions

Run:

```bash
./tools/oneplus15-port/portctl sync
cat .port-work/refs.lock
```

The lock file records the generation time and the exact Linux, ACK, and OnePlus source commits used for the work.

Quote these revisions in a build report or pull request whenever behavior depends on a specific OEM source revision.

## Code-import policy

When adapting public OEM code:

1. Keep the original SPDX identifier.
2. Keep relevant copyright notices.
3. Copy only the files or data required by the hardware block.
4. Compare with a recent upstream driver for the same subsystem.
5. Remove Android or vendor-only functionality not needed for mainline operation.
6. Convert downstream structures to mainline APIs.
7. Document altered register tables or semantics.
8. Do not add proprietary firmware, keys, or binary-only modules.

## Subsystem provenance guidance

### Pinctrl

The downstream Canoe driver is a reference for pin names, mux functions, GPIO count, and the PDC wake map. Downstream-only wake-register and QUP mode metadata must be adapted to mainline pinctrl structures.

### Clock controller

The downstream GCC source is authoritative for Canoe clock/reset IDs and register programming, but its VDD voting and clock-regulator framework is not a drop-in match for Linux 6.12.

### Interconnect

The downstream Canoe NoC source contains topology and QoS data. Its multi-voter and CRM structures must be translated to the mainline RPMh interconnect framework.

### UFS PHY

The downstream source provides Canoe calibration tables and register values. The target architecture is the mainline QMP UFS PHY framework, not the separate downstream UFS PHY framework.

### Device tree

The downstream Canoe DTS hierarchy is a hardware reference. Mainline device trees should be reduced, organized by SoC and board, and validated against upstream schemas rather than imported wholesale.

## Refreshing references

The OEM branch may move while retaining the same branch name. Refresh deliberately:

```bash
./tools/oneplus15-port/portctl refresh-oem
```

Review the updated `refs.lock` and source differences before changing port code.

## Attribution in commits

Commit messages should identify the source repository and reference revision when a patch contains substantial imported tables or data. This makes later review and rebasing possible without turning the working tree into undocumented vendor code.
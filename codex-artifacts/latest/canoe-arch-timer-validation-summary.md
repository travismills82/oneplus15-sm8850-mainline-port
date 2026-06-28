# Canoe ARM architectural timer validation

## Result

passed

## Source identity

- Locked base: `0f7617a2bf86c27e3388b9273d22400855e117ca`
- Source branch: `port/canoe-arch-timer-v7.1`
- Branch mode: `resume-existing-remote-branch`
- Source commit: `4337f3e21b45fcbcfeb4a2debf406d71aa10ee93`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `none`
- Exit status: `0`

## Provenance

- Official repository: `OnePlusOSS/android_kernel_modules_and_devicetree_oneplus_sm8850`
- Official reference: `307d23721d20e21b8160b9ba9c3cb50911ff8c00`
- Official file: `kernel_platform/qcom/opensource/devicetree/qcom/canoe.dtsi`

The official Canoe source and the historical port agree on:

- secure physical timer: PPI 13, level-low
- non-secure physical timer: PPI 14, level-low
- virtual timer: PPI 11, level-low
- hypervisor timer: PPI 10, level-low
- CPU mask: eight CPUs
- counter frequency workaround: 19.2 MHz

## Scope

Included:

- one root-level `arm,armv8-timer` node
- four timer PPIs
- the Canoe 19.2 MHz firmware-frequency workaround

Explicitly excluded:

- memory-mapped timer at `0x17810000`
- ITS/MSI
- CPU and PMU nodes
- memory
- PDC
- UART
- unrelated peripherals

## Validation

- exact main-base lock
- exact-file semantic lock
- one-file source-delta verification
- signed-off single-commit verification
- Codex implementation or read-only review
- targeted `arm,arch_timer.yaml` binding check
- ARM64 LLVM defconfig
- exact OnePlus 15 DTB build with `W=1`
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- compiled-DTB timer inspection
- strict checkpatch
- final remote-head verification

## DTB SHA-256

`b2fcd71b0290b1b80d1635acf2cc057e6828bb2dc61925a84f7ceb24398e2e59`

## Hardware status

Compile and schema validation only. No device operation or hardware-validation
claim was made.

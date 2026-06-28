# Canoe PSCI validation

## Result

Validation passed.

## Source target

- Branch: `port/canoe-psci-v7.1`
- Base commit: `e5bb08e549b8251818b8a5f65029d996b40af8da`
- Source commit: `e1238a633627700f46b76d170da32e63b44595bd`
- Changed files: exactly one
- dtschema: `2026.6`

## Source change

Added a resource-free PSCI node to `arch/arm64/boot/dts/qcom/canoe.dtsi`:

- compatible: `arm,psci-1.0`
- conduit: `smc`

No CPU nodes, MPIDRs, power domains, idle states, MMIO ranges, interrupts,
clocks, regulators, or other unverified data were added.

## Validation completed

- exact base and branch verification
- exact one-file source-delta verification
- targeted PSCI `dt_binding_check`
- ARM64 LLVM defconfig
- exact `qcom/canoe-oneplus-15.dtb` build with `W=1`
- exact per-DTB kernel validation using `CHECK_DTBS=y`
- colon-separated ARM board, PSCI, and SCM schema filters
- direct `dt-validate` with empty stdout and stderr
- compiled-DTB inspection with DTC
- strict checkpatch review
- source branch push and remote commit verification

## DTB SHA-256

`dfefbb7d6b4341f05523505e5547bbe2a1af065ca1399400feb4f72e36449f3f`

## Hardware status

Compile and schema validation only. No hardware operation or claim was made.

## Next action

Create and merge a focused pull request using a signed-off squash commit after
independently verifying these published logs.

# Canoe SCM validation

## Result

Validation passed.

## Pull request target

- Pull request: `#12`
- Branch: `port/canoe-scm-v7.1-clean`
- Commit: `81530faeabe64ba233c3ede97cc1457374f31df2`
- Base: `origin/main`
- Changed files: exactly two

## Source changes

1. Added `qcom,scm-canoe` to:
   `Documentation/devicetree/bindings/firmware/qcom,scm.yaml`
2. Added a resource-free SCM firmware node to:
   `arch/arm64/boot/dts/qcom/canoe.dtsi`

No register ranges, interrupts, clocks, interconnects, memory regions,
download-mode properties, or other unverified resources were added.

## Validation completed

- exact remote branch and commit verification
- `git diff --check origin/main...HEAD`
- exact two-file source-delta verification
- targeted SCM `dt_binding_check`
- ARM64 LLVM defconfig
- exact `qcom/canoe-oneplus-15.dtb` build with `W=1`
- targeted ARM board and SCM `dtbs_check`
- compiled-DTB inspection with DTC
- separate strict checkpatch reviews
- no actionable checkpatch findings

## DTB SHA-256

`8de535b9361facaff13db0f48600a9563f0146a2b484adcbad43620212e6e68b`

## Hardware status

Compile and schema validation only. No hardware operation or claim was made.

## Next action

Mark PR #12 ready for review and merge it after independently verifying this
published report.

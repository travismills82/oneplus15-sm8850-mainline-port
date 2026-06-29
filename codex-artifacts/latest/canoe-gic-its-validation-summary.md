# Canoe GIC ITS validation and publication recovery

## Result

Validation passed. The original publication stage failed after all validation
steps had completed; this recovery republishes the existing evidence without
rebuilding or changing kernel source.

## Source identity

- Locked base: `7c7f69c50c7ecd2c49b3c39e341ffb5d71055ae6`
- Source branch: `port/canoe-gic-its-v7.1`
- Source commit: `6840ea2042571180da305e038a53fcc6b4c1075d`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Commit count: `1`
- Signed-off commit: `yes`

## Provenance

- Official repository: `OnePlusOSS/android_kernel_modules_and_devicetree_oneplus_sm8850`
- Official reference: `307d23721d20e21b8160b9ba9c3cb50911ff8c00`
- Official file: `kernel_platform/qcom/opensource/devicetree/qcom/canoe.dtsi`
- ITS base: `0x17040000`
- ITS size: `0x20000`

Codex reported that its restricted network/cache environment could not
independently fetch the official provenance file. That limitation did not
affect the local source-delta review, binding checks, DTB build, schema
validation, compiled-DTB inspection, or checkpatch result.

## Verified validation evidence

- Codex read-only verdict: `CODEX_REVIEW_PASS`
- Targeted GICv3 binding check: passed with no warning/error findings
- Exact ARM64 LLVM `W=1` DTB build: passed with no warning/error findings
- Exact per-DTB schema validation: passed with no warning/error findings
- Direct `dt-validate` stdout: empty
- Direct `dt-validate` stderr: empty
- Compiled-DTB semantic inspection: `compiled ITS checks passed`
- Strict checkpatch: `0 errors, 0 warnings`
- Remote source head reverified: `6840ea2042571180da305e038a53fcc6b4c1075d`

## DTB SHA-256

`4abc7fa1bbe0d9acca18401d56d8dd0a7887ee439dbfcad885b10b8e0cc57e8e`

## Scope

Included:

- GIC child address and size cells
- empty GIC child `ranges`
- ITS MSI controller at `0x17040000`
- ITS window size `0x20000`
- one MSI cell

Excluded:

- CPU and PMU nodes
- memory
- PDC
- UART
- memory-mapped timer
- unrelated peripherals

## Hardware status

No physical-device operation or hardware validation was performed or inferred.

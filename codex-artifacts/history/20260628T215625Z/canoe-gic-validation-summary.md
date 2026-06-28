# Canoe GICv3 core-controller validation

## Result

failed

## Source

- Base commit: `7da2b8056503ca8d5435f909016af3a5ebe0e26b`
- Source branch: `port/canoe-gic-v7.1`
- Source commit: `not created`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `local source branch already exists: port/canoe-gic-v7.1`
- Exit status: `1`

## Provenance

The values were independently confirmed in the official OnePlus SM8850
devicetree source:

- Repository: `OnePlusOSS/android_kernel_modules_and_devicetree_oneplus_sm8850`
- Reference: `307d23721d20e21b8160b9ba9c3cb50911ff8c00`
- Path: `kernel_platform/qcom/opensource/devicetree/qcom/canoe.dtsi`

Confirmed values:

- GICD: `0x17000000`, size `0x10000`
- GICR: `0x17080000`, size `0x200000`
- redistributor regions: `1`
- redistributor stride: `0x40000`
- maintenance interrupt: PPI 9, level-high
- root interrupt parent: `&intc`

## Scope

Included:

- ARM GIC binding include
- root `interrupt-parent`
- minimal `soc@0` simple bus
- GICv3 core controller

Explicitly excluded:

- ITS/MSI controller
- architectural timer
- CPU and PMU nodes
- memory
- PDC
- UART
- all unrelated peripherals

## Validation

- exact-file semantic lock
- one-file source-delta verification
- `git diff --check`
- targeted GICv3 `dt_binding_check`
- ARM64 LLVM defconfig
- exact OnePlus 15 DTB build with `W=1`
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- compiled-DTB DTC inspection
- strict checkpatch
- signed-off commit verification
- remote branch head verification

## DTB SHA-256

`not produced`

## Hardware status

Compile and schema validation only. No device operation or hardware-validation
claim was made.

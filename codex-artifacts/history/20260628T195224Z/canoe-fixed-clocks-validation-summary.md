# Canoe fixed input clocks validation

## Result

Validation passed.

## Source target

- Branch: `port/canoe-fixed-clocks-v7.1`
- Base commit: `84ff2a4c6971911f8835972bd70cb8195e4ef4d0`
- Source commit: `2e36dd81f1d8f1d3b5ba47cc54047c571711695e`
- Changed files: exactly one
- dtschema: `2026.6`

## Codex involvement

The validated source commit was produced by the earlier Codex-assisted workflow.
A later rerun stopped because that local branch already contained this commit
and overwrote the latest report with a safety failure. This recovery run did
not regenerate or alter the source; it independently validated the exact
remote commit.

## Source change

Added a root-level `clocks` container to `arch/arm64/boot/dts/qcom/canoe.dtsi` containing exactly:

- `bi_tcxo: clock-19200000`
  - `compatible = "fixed-clock"`
  - `#clock-cells = <0>`
  - `clock-frequency = <19200000>`
- `sleep_clk: clock-32768`
  - `compatible = "fixed-clock"`
  - `#clock-cells = <0>`
  - `clock-frequency = <32768>`

No PCIe, USB, UFS, MMIO, interrupt, regulator, reset, power-domain, CPU,
memory, GIC, timer, or other unverified data was added.

## Validation completed

- exact remote branch, base, and source commit verification
- exact one-file source-delta verification
- Signed-off-by trailer verification
- semantic checks for exactly two approved fixed-clock providers
- `git diff --check`
- targeted fixed-clock `dt_binding_check`
- ARM64 LLVM defconfig
- exact `qcom/canoe-oneplus-15.dtb` build with `W=1`
- exact per-DTB validation using `CHECK_DTBS=y`
- colon-separated schema filters
- direct `dt-validate` with empty stdout and stderr
- compiled-DTB inspection with DTC
- strict checkpatch review

## DTB SHA-256

`20954a3be4e4f27d991af43fd0b6dbcb0ae8954344e52a832f1145d3cd07e243`

## Hardware status

Compile and schema validation only. No hardware operation or claim was made.

## Next action

Independently verify the published logs, open a focused pull request, and merge
only if the remote branch still points to the validated source commit.

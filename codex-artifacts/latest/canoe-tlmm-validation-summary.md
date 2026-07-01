# Canoe TLMM DTS validation

- Result: `passed`
- Locked base: `91660d9e94c05d860bd4ccab35e838db794c073d`
- Source branch: `port/canoe-tlmm-v7.1`
- Source commit: `b11324b8e4637c94299c10709f98aa4a64d2fc7a`
- DTSI blob: `323eb918b7127e13a0dc1077c7fefbe3bdf20543`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `none`
- Exit status: `0`
- DTB SHA-256: `4ab8359e3cac0666be018fec2e70781bd2aa09658edd37a5e0d1fe619ca20e00`

## Scope

Included only:

- `tlmm: pinctrl@f000000`
- dedicated compatible `qcom,canoe-tlmm`
- register window `0x0f000000`, size `0x202000`
- GIC SPI 208, level-high
- GPIO and interrupt-controller provider cells
- `gpio-ranges` exposing 218 GPIOs
- PDC wakeup parent
- reserved GPIO ranges `36-43`, `74`, `119-120`, and `144-147`

No pin states, UART pins, UFS reset state, GPIO consumers, peripheral
enablement, or unrelated nodes are included.

## Commit message policy

The source commit must include an imperative subject, explanatory body,
purpose, scope, and Signed-off-by trailer.

## Validation

- exact main/source SHA, parent, changed-file, and DTSI blob locks
- one signed-off commit with a descriptive body
- exact one-file, 13-line source delta
- Codex read-only review
- pinned dtschema `2026.6`
- targeted Canoe TLMM binding check
- exact LLVM `W=1` board DTB build
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- numeric compiled-DTB TLMM semantic inspection
- strict checkpatch
- final remote source-head verification

No hardware validation or physical-device operation was performed.

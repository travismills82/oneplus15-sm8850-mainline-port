# Canoe AOSS QMP DTS validation

- Result: `passed`
- Locked base: `0f4d29f65ac4c5cd48a44e764f3df4d0b1ea4c05`
- Source branch: `port/canoe-aoss-qmp-v7.1-clean`
- Source commit: `110110b12c40466e42480c223235bbe6078b270e`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `none`
- Exit status: `0`
- DTB SHA-256: `6348dd163b90b0a8613274c512577c3e472a45725701bed271fa471a1dd6828e`

## Scope

Included only:

- `aoss_qmp: qmp@c300000`
- compatible `qcom,canoe-aoss-qmp`, `qcom,aoss-qmp`
- register window `0x0c300000`, size `0x400`
- AOP GLINK-QMP incoming interrupt through IPCC
- AOP GLINK-QMP outgoing mailbox through IPCC
- zero-cell clock provider

No AOSS consumers, cooling devices, power-domain cells, remote processors,
SMP2P nodes, CPUs, PMU, memory, UART, or unrelated devices are included.

## Validation

- exact source and parent lock
- one signed-off commit and one changed file
- Codex read-only review
- targeted AOSS QMP binding check
- exact LLVM `W=1` DTB build
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- numeric compiled-DTB semantic inspection
- strict checkpatch
- final remote source-head verification

No hardware validation or physical-device operation was performed.

# Canoe IPCC DTS validation

- Result: `passed`
- Base: `559b77665695727a5602650d36cf4b99bfb0b2ee`
- Branch: `port/canoe-ipcc-v7.1`
- Source commit: `4db4ef22947c65d3acf79987d05574f471c59033`
- DTSI blob: `73ba070adc2d48415fa863613633d175ed18fdda`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Final step: `none`
- Exit status: `0`
- DTB SHA-256: `ff7b6946aa99f66b4aa16a53a9ade905d9ac8be123df35560617d8c7f1f3f51f`

Included only the IPCC controller at 0x01106000, GIC SPI 246, three interrupt
cells, and two mailbox cells. No consumers or unrelated nodes are included.

Validation covers exact Git identity, Codex read-only review, targeted binding
validation, LLVM W=1 DTB build, exact per-DTB schema validation, empty direct
dt-validate output, numeric compiled-DTB inspection, strict checkpatch, and a
final remote-head lock. No hardware operation was performed.

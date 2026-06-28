# Canoe GICv3 existing-branch validation

- Result: passed
- Base commit: `7da2b8056503ca8d5435f909016af3a5ebe0e26b`
- Source branch: `port/canoe-gic-v7.1`
- Source commit: `3c7b32c8add9244d767fbd073d61f1d113233f22`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Codex review: `CODEX_REVIEW_PASS`
- dtschema: `2026.6`
- DTB SHA-256: `be00089a8145a22730d454f5aee8da98bca58a8027a18bbc091b7746630e97e5`
- Source branch rewritten: `no`
- Pull request opened: `no`
- Physical-device operation: `no`

Validation included exact-file locking, one-file scope verification, signed-off
commit verification, GICv3 binding check, LLVM W=1 exact DTB build, per-DTB
schema validation, empty direct dt-validate output, compiled-DTB inspection,
strict checkpatch, and final remote-head verification.

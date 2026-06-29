# Canoe PDC DTS validation recovery

## Result

Validation passed. The original workflow completed Codex review, targeted
binding validation, the exact LLVM `W=1` DTB build, exact per-DTB schema
validation, and direct `dt-validate` before exiting at its overly strict
compiled-DTB register regex.

This recovery verified the prior evidence, rebuilt only the exact DTB, replaced
the regex with cell-by-cell semantic validation, ran strict checkpatch, and
reverified the immutable source branch.

## Source identity

- Base: `9e8d5b2fa1b8f412534e74fe72b2804c44e6ff36`
- Branch: `port/canoe-pdc-v7.1`
- Commit: `ee6c5f7564d87bb7ec1041799a6a00bec9fd36eb`
- DTSI blob: `bdabe985d44884d468aaa6e18d9919a27b1499fa`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Signed-off commit: `yes`

## Validation evidence

- Codex read-only review: `CODEX_REVIEW_PASS`
- Targeted PDC binding check: passed
- Original exact LLVM `W=1` DTB build: passed
- Original exact per-DTB schema validation: passed
- Direct `dt-validate` stdout: empty
- Direct `dt-validate` stderr: empty
- Recovery exact LLVM `W=1` DTB build: passed
- Robust compiled-DTB PDC semantic check: passed
- Strict checkpatch: `0 errors, 0 warnings`
- Source branch reverified: `ee6c5f7564d87bb7ec1041799a6a00bec9fd36eb`

## DTB SHA-256

`b8b5896488e0744b456d3a3f47a9708339be975e4e9e1d1c6be26a7b78627163`

## Hardware status

No physical-device operation or hardware validation was performed or inferred.

# Canoe IPCC binding validation

- Result: `passed`
- Locked base: `f8bcff6104eaec20e8b9ccacf614814e9ffb6ac5`
- Source branch: `port/canoe-ipcc-binding-v7.1`
- Source commit: `d517223ce0bb1da248db2b7bc2d27b08a20b969c`
- Changed file: `Documentation/devicetree/bindings/mailbox/qcom-ipcc.yaml`
- Added compatible: `qcom,canoe-ipcc`
- Failed step: `none`
- Exit status: `0`

## Scope

The source delta adds only `qcom,canoe-ipcc` to the compatible enum in
`qcom-ipcc.yaml`.

No DTS node, driver change, runtime claim, source-branch rewrite, or hardware
operation is included.

## Validation

- exact main/source SHA lock
- exactly one signed-off commit
- exactly one changed file and one added source line
- exact enum-content and alphabetical-order verification
- Codex read-only review
- pinned dtschema `2026.6`
- targeted `dt_binding_check`
- strict checkpatch
- final remote source-head verification

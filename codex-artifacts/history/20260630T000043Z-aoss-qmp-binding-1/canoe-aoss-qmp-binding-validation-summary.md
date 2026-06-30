# Canoe AOSS QMP binding validation

- Result: `passed`
- Locked base: `48497ee326cae5fa9715cb9bfea1d565ecd1b50b`
- Source branch: `port/canoe-aoss-qmp-binding-v7.1-clean`
- Source commit: `0f4d29f65ac4c5cd48a44e764f3df4d0b1ea4c05`
- Binding blob: `23ff501328c476414fdcfb637e4dd78dcef58301`
- Changed file: `Documentation/devicetree/bindings/soc/qcom/qcom,aoss-qmp.yaml`
- Added compatible: `qcom,canoe-aoss-qmp`
- Failed step: `none`
- Exit status: `0`

## Scope

The source delta adds only `qcom,canoe-aoss-qmp` to the compatible enum in
`qcom,aoss-qmp.yaml`.

The earlier staging branch that temporarily removed description text is not
used by this validation. This workflow validates only the clean one-commit
branch named above.

No DTS node, driver change, runtime claim, source-branch rewrite, or hardware
operation is included.

## Validation

- exact main, source, parent, changed-file, and blob locks
- exactly one signed-off commit
- exactly one changed file and one added line
- exact enum content and alphabetical-order verification
- Codex read-only review
- pinned dtschema `2026.6`
- targeted `dt_binding_check`
- strict checkpatch
- final remote source-head verification

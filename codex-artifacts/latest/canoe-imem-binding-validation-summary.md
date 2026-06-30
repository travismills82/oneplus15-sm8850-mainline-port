# Canoe IMEM binding validation

- Result: `passed`
- Locked base: `fbb569da8c72373db3d428de722fa91eecd83774`
- Source branch: `port/canoe-imem-binding-v7.1-clean`
- Source commit: `7b2a5848ded6cfae8d94373190c5febfb9a16f7e`
- Binding blob: `e33b562192cd3470629a9172ff0f39d606869702`
- Changed file: `Documentation/devicetree/bindings/sram/qcom,imem.yaml`
- Added compatible: `qcom,canoe-imem`
- Failed step: `none`
- Exit status: `0`

## Scope

The source delta adds only `qcom,canoe-imem` to the compatible enum in
`qcom,imem.yaml`.

The earlier branch that also added an unnecessary YAML end marker is excluded.
This workflow validates only the clean one-commit branch named above.

No DTS node, driver change, runtime claim, source-branch rewrite, or hardware
operation is included.

## Validation

- exact main, source, parent, file, and blob locks
- exactly one signed-off commit
- exactly one changed file and one added source line
- exact enum content and alphabetical order
- Codex read-only review
- pinned dtschema `2026.6`
- targeted `dt_binding_check`
- strict checkpatch
- final remote source-head verification

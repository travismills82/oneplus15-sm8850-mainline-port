# Canoe PDC binding validation

- Result: `passed`
- Locked base: `41b74fb97e7e135a8f14d1510704a4a80d1fe3df`
- Source branch: `port/canoe-pdc-binding-v7.1`
- Source commit: `beb12cb6b78a6b08da64a3f104bb74f87f1bd4ec`
- Expected source commit: `beb12cb6b78a6b08da64a3f104bb74f87f1bd4ec`
- Changed file: `Documentation/devicetree/bindings/interrupt-controller/qcom,pdc.yaml`
- Failed step: `none`
- Exit status: `0`

## Scope

The source delta adds only:

`qcom,canoe-pdc`

to the compatible enum in `qcom,pdc.yaml`.

No DTS node, driver change, hardware operation, or runtime claim is included.

## Validation

- exact main and source SHA lock
- exactly one signed-off commit
- exactly one changed file and one added line
- exact source-delta verification
- Codex read-only review
- pinned dtschema `2026.6`
- targeted `dt_binding_check`
- strict checkpatch
- final remote-source-head verification

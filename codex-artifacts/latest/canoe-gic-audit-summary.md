# Canoe GIC provenance audit handoff

## Result

Codex audit completed and the report passed structural and repository-safety
checks.

## Target

- Main commit: `7da2b8056503ca8d5435f909016af3a5ebe0e26b`
- Historical reference commit: `5555043e466f5a12dbc029254883f3f8ed7061cd`
- Historical file: `arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts`
- Current file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- GIC schema: `Documentation/devicetree/bindings/interrupt-controller/arm,gic-v3.yaml`
- Codex CLI: `codex-cli 0.142.3`

## Safety verification

- Codex ran with the read-only sandbox.
- The detached audit worktree remained clean.
- No source branch, commit, pull request, or merge was created.
- No physical-device operation was performed.

## Published report

`codex-artifacts/latest/canoe-gic-provenance-audit.md`

## Next action

Review the Codex verdict and evidence matrix. Implement a GIC node only when
every required property has sufficient provenance and no unresolved blocker.

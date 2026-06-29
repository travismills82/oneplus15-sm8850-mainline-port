No findings.

Verified commit `ee6c5f7`:

- Direct parent is specified base `9e8d5b2`.
- Only `canoe.dtsi` changed: 17 additions.
- PDC address, compatibles, register windows, 24 tuples, two interrupt cells, GIC parent, and `interrupt-controller` are correct.
- No PDC consumers or unrelated TLMM, SPMI, CPU, PMU, memory, UART, or timer nodes were added.
- `git diff --check` passes.

CODEX_REVIEW_PASS
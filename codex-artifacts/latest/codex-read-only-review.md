Review passed.

- Detached HEAD matches source commit `3c7b32c8...`.
- Source branch resolves to that commit.
- Delta from locked base changes only `canoe.dtsi`: 21 insertions, 0 deletions.
- Added content is limited to:
  - ARM GIC binding include
  - root `interrupt-parent`
  - minimal `soc@0` simple bus
  - GICv3 controller with the specified GICD/GICR ranges, one redistributor region, `0x40000` stride, and PPI 9 level-high
- ITS, MSI, timer, CPUs, PMU, memory, PDC, UART, serial, and unrelated devices are absent from the delta.
- `git diff --check` passed.
- Working file exactly matches the source commit.
- No modifications or other prohibited actions occurred.

Commands run:

```text
git status --short --branch
git rev-parse HEAD
git cat-file -t <base/source>
git diff --name-status/--stat/--numstat/--check <base> <source>
git diff ... -- canoe.dtsi
git show --no-patch --format=fuller <source>
git branch --all --contains <source>
git rev-parse port/canoe-gic-v7.1
rg ... (for prohibited additions and provenance/value checks)
git diff --exit-code <source> -- canoe.dtsi
git status --porcelain=v1
```

Remaining risk: the official GitHub reference could not be independently fetched because network access returned a cache miss. The committed values match the provenance details supplied in the request.

CODEX_REVIEW_PASS
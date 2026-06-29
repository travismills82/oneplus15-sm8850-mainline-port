Review result: no issues found.

- Commit `6840ea2` is exactly one commit after locked base `7c7f69c`.
- Remote branch `origin/port/canoe-gic-its-v7.1` resolves to the source commit.
- Only `canoe.dtsi` changed: 10 insertions, all inside the GIC node.
- Every required GIC ITS property matches exactly.
- No CPU, PMU, memory, PDC, UART, memory-timer, or unrelated device changes.
- `git diff --check` passed.
- Worktree remained unchanged.

Commands run:

```text
git status --short
git rev-parse --verify <base/source>
git diff --name-status/--stat/--check <base> <source>
git show --format=fuller --stat <source>
git diff --unified=80 <base> <source> -- canoe.dtsi
git merge-base --is-ancestor <base> <source>
git rev-list --count <base>..<source>
git branch -a --contains <source>
git rev-parse <source>^
git rev-parse refs/remotes/origin/port/canoe-gic-its-v7.1
git diff-tree --name-status <source>
git show <source>:arch/arm64/boot/dts/qcom/canoe.dtsi
git remote -v
git status --porcelain=v1 --untracked-files=all
```

Remaining risks: the stated OnePlus provenance file could not be independently fetched due restricted network/cache access. No DT build or hardware validation was performed.

CODEX_REVIEW_PASS
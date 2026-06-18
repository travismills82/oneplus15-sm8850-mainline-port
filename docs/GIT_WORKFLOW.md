# Git workflow

This repository uses a compact source-snapshot history. That design keeps GitHub pushes practical while the local development tree may still contain the complete upstream Linux history.

## Why this matters

A normal push from a branch based on the full Linux history can attempt to enumerate more than ten million Git objects. Symptoms include a push appearing to stop at:

```text
Enumerating objects: 10xxxxxx, done.
```

That is not the intended publishing workflow for this repository.

## Remotes

Recommended remote naming:

```text
stable          Linux stable reference
oneplus-common  OnePlus common-kernel reference
port-origin     This compact GitHub repository
```

Verify before pushing:

```bash
git remote -v
```

Set the project remote safely:

```bash
git remote set-url port-origin \
  https://github.com/travismills82/oneplus15-sm8850-mainline-port.git
```

Use `git remote add` only when the remote does not already exist.

## Branch types

### Compact published branches

These branches must descend from the compact `main` commit in this repository:

```text
main
bringup/canoe-core-probe
bringup/canoe-ufs
bringup/canoe-gcc
bringup/canoe-interconnect
bringup/canoe-dts
```

Normal pushes from these branches should enumerate only the objects changed by the port.

### Local full-history branches

Branches such as:

```text
backup/canoe-core-probe-full-history
```

exist only as local safety references. Do not push them to `port-origin`.

## Starting new work

Fetch the compact repository and start from its published base:

```bash
git fetch port-origin --prune
git switch main
git reset --hard port-origin/main
git switch -c bringup/canoe-ufs
```

When work depends on another compact bring-up branch, branch from that published branch instead:

```bash
git fetch port-origin --prune
git switch -c bringup/canoe-dts \
  port-origin/bringup/canoe-core-probe
```

## Before committing

```bash
git status --short
git diff --check
git diff --stat
git diff
```

For imported OEM code, confirm that no upstream core framework header was replaced:

```bash
git status --short | grep -E \
  '(^|/)common\.h$|icc-rpmh\.h$|clk-regmap\.h$|pinctrl-msm\.h$' \
  && echo 'Review core-header changes carefully'
```

## Commit structure

Prefer one subsystem per commit. Suggested order:

```text
dt-bindings: ...
clk: qcom: ...
pinctrl: qcom: ...
interconnect: qcom: ...
phy: qcom: ...
arm64: dts: qcom: ...
docs: ...
```

A commit message should explain:

- what hardware block is added;
- which downstream API difference was adapted;
- what was compiled or tested;
- what remains unvalidated.

## Publishing a compact branch

First confirm its ancestry:

```bash
git merge-base --is-ancestor port-origin/main HEAD
```

Exit status zero means `HEAD` descends from compact `main`.

Then push:

```bash
git push -u port-origin HEAD
```

If Git begins enumerating millions of objects, press `Ctrl+C` and inspect the branch ancestry. Do not leave the push running.

## Converting a full-history branch into a compact branch

Preserve the original branch first:

```bash
git branch backup/my-branch-full-history HEAD
```

Fetch the compact base:

```bash
git fetch port-origin --prune
```

Create a commit with the current source tree on top of compact `main`:

```bash
TREE="$(git rev-parse HEAD^{tree})"
BASE="$(git rev-parse port-origin/main)"

COMPACT_COMMIT="$({
  printf '%s\n' 'describe the current port milestone'
} | git commit-tree "$TREE" -p "$BASE")"

git reset --hard "$COMPACT_COMMIT"
```

Review the resulting commit before pushing:

```bash
git log --oneline --decorate -3
git diff --stat port-origin/main..HEAD
git push -u port-origin HEAD
```

This preserves the complete source tree but does not publish the full upstream object history.

## Syncing OEM reference trees

Use the project helper:

```bash
./tools/oneplus15-port/portctl sync
```

The OEM mirrors live under `.port-work/` and have their own Git repositories. Do not add them to the main kernel repository.

## Generated files

Do not commit these directories:

```text
.port-work/
out/
artifacts/oneplus15/build/
```

Reports may be committed only when they document a stable, reproducible milestone and contain no private device information.

## Rewriting published history

Avoid force-pushing shared bring-up branches. If a compact branch must be repaired:

1. create a backup branch;
2. coordinate any open work based on it;
3. use `--force-with-lease`, never plain `--force`;
4. document why the rewrite was necessary.

## Pull requests

For larger changes:

- push a compact topic branch;
- open a PR against `main` or the appropriate dependency branch;
- include build commands and results;
- call out compile-only status;
- update `docs/STATUS.md` when a subsystem milestone changes.
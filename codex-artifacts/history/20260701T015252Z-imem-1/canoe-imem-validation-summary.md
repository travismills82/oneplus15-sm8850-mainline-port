# Canoe IMEM DTS post-merge validation

- Result: `passed`
- Locked base: `ef35e20c16d4d02c5274ebce6fb860cf2c78fb62`
- Source branch: `main`
- Source commit: `91660d9e94c05d860bd4ccab35e838db794c073d`
- DTSI blob: `64c571396c015a554cbeef65c3c982d539d00fed`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `none`
- Exit status: `0`
- DTB SHA-256: `1cd38081e3769e655dab51cb39acb6309f569e337c358e7a566e3655f2280f91`

## Scope

Included only:

- `imem: sram@14680000`
- compatible `qcom,canoe-imem`, `syscon`, `simple-mfd`
- 4 KiB window at `0x14680000`
- one-cell child address and size format
- PIL relocation child at offset `0x94c`, size `0xc8`

Downstream-only IMEM debug, restart-reason, dump-table, timeout, and diagnostic
children are excluded.

## Commit message policy

The source commit must include:

- an imperative subject line;
- a non-empty explanatory body describing what changed and why;
- scope information;
- `Signed-off-by: Travis Mills <travismills82@gmail.com>`.

## Validation

- exact main/source SHA, parent, changed-file, and blob locks
- one signed-off commit with a descriptive body
- Codex read-only review
- targeted IMEM and PIL-info binding checks
- exact LLVM `W=1` board DTB build
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- numeric compiled-DTB semantic inspection
- strict checkpatch
- final remote source-head verification

No hardware validation or physical-device operation was performed.

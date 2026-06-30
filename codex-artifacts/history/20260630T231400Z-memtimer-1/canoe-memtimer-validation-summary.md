# Canoe memory-mapped timer validation

- Result: `passed`
- Locked base: `37e9f48bb6f3598c4648474ef88e39d6a882b217`
- Source branch: `port/canoe-memtimer-v7.1`
- Source commit: `c76ab9bdd40f6fb5a08f7eb7b190faa9effcf432`
- DTSI blob: `e9f8b219ff11140b7fe749b45db16fe42df7188a`
- Changed file: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Failed step: `none`
- Exit status: `0`
- DTB SHA-256: `049af240db6a5316119d9c7a1e93778c1fbe05e67767a2e97c03b984c9456e17`

## Scope

Included only:

- `memtimer: timer@17810000`
- generic compatible `arm,armv7-timer-mem`
- control frame at `0x17810000`, size `0x1000`
- 19.2 MHz clock frequency
- enabled frame 0
- frame views at `0x17811000` and `0x17812000`
- GIC SPI 8 physical and SPI 6 virtual timer interrupts

The six disabled downstream frames, watchdog, CPUs, PMU, TLMM, reserved
memory, UART, and unrelated devices are excluded.

## Validation

- exact main/source SHA, parent, file, and blob locks
- one signed-off commit and one changed file
- Codex read-only review
- pinned dtschema `2026.6`
- targeted memory-timer binding check
- exact LLVM `W=1` board DTB build
- exact per-DTB schema validation
- direct `dt-validate` with empty stdout and stderr
- numeric compiled-DTB semantic inspection
- strict checkpatch
- final remote source-head verification

No hardware validation or physical-device operation was performed.

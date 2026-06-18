# Bring-up roadmap

The project follows a dependency-first sequence. Each phase should end with a reviewable commit, a focused build result, and retained logs.

## Phase 0 — Reproducible baseline

Status: completed.

- Establish Linux `v6.12.23` as the source baseline.
- Build ARM64 `Image`, modules, and available DTBs with LLVM/Kbuild.
- Synchronize public ACK and OnePlus reference trees.
- Record exact source revisions.
- Keep output, OEM mirrors, and device snapshots outside source commits.

Exit criteria:

- Clean build succeeds.
- Build configuration and compiler versions are retained.
- Repository history is compact enough for normal GitHub pushes.

## Phase 1 — Platform identity and DT analysis

Status: completed for analysis; implementation in progress.

- Confirm the OnePlus platform mapping uses Canoe DTBs and Infiniti overlays.
- Analyze the recursive include closure for `qcom/canoe.dts`.
- Inventory binding headers, clocks, resets, interconnect IDs, power domains, and board overlays.
- Identify the early serial address and OEM kernel command line.

Exit criteria:

- No ambiguity remains between the `sun` and `canoe` platform families.
- A reduced list of early-boot dependencies is documented.

## Phase 2 — Canoe TLMM / pinctrl

Status: object compilation validated.

- Adapt the OEM Canoe TLMM data to Linux 6.12 pinctrl structures.
- Preserve the PDC wake IRQ map.
- Exclude downstream-only QUP/I3C mode-register plumbing from the first patch.
- Add Kconfig and Makefile integration.
- Add or clean up the DT binding.

Exit criteria:

- `pinctrl-canoe.o` compiles without warnings.
- `CONFIG_PINCTRL_CANOE=y` survives `olddefconfig`.
- DT schema for the minimal TLMM node passes.
- No mainline core header is replaced.

## Phase 3 — Global clock controller

Status: analyzed.

Work items:

- Import Canoe clock and reset IDs.
- Compare `gcc-canoe.c` against recent upstream Qualcomm GCC drivers.
- Remove downstream VDD-class and regulator-array assumptions.
- Translate PLL types and operations to existing mainline equivalents.
- Preserve only clocks required for serial, UFS, USB, and essential buses initially.
- Add GDSCs only where the mainline framework supports the required semantics.

Exit criteria:

- `gcc-canoe.o` compiles without downstream core clock headers.
- Kconfig/Makefile integration is isolated in one commit.
- Serial and UFS clock IDs used by the minimal DTS resolve.

## Phase 4 — Minimal SoC and board device tree

Status: analyzed.

Create a mainline-style split, rather than copying the full downstream hierarchy:

```text
arch/arm64/boot/dts/qcom/sm8850-canoe.dtsi
arch/arm64/boot/dts/qcom/oneplus-oneplus15.dts
```

The exact upstream naming may change after confirming public product identifiers.

Initial nodes:

- CPUs, PSCI, timer, GIC;
- reserved memory only when proven necessary;
- RPMh, command DB, AOSS, PDC;
- TLMM;
- GCC;
- one GENI serial engine;
- UFS host and PHY after their driver work is ready.

Keep all nonessential peripherals disabled.

Exit criteria:

- DTB builds through the kernel build system.
- `dtbs_check` is clean for newly added bindings, or warnings are explicitly tracked.
- The source contains no unresolved downstream labels or phandles.

## Phase 5 — Early serial boot

Status: not started.

- Configure the GENI console used by the OEM consolidate build.
- Use the known earlycon MMIO address `0x00a9c000` only after verifying the matching serial node and register range.
- Build a test image without enabling storage-dependent root filesystems.
- Establish a recovery and rollback procedure before any hardware test.
- Capture serial output from the first instruction visible to the kernel.

Exit criteria:

- Repeatable kernel output is captured from the mainline image.
- Failures can be categorized before storage or userspace starts.
- The device can always return to the known-good boot path.

## Phase 6 — Interconnect

Status: analyzed.

- Convert Canoe NoC nodes and BCMs to the mainline RPMh interconnect model.
- Replace downstream QoS operations with mainline `qcom_icc_qosbox` data.
- Start with paths required by serial and UFS.
- Add additional fabrics only as clients are enabled.

Exit criteria:

- Canoe interconnect driver compiles without replacing `icc-rpmh.h`.
- Required UFS and system NoC paths probe successfully.
- No unexplained RPMh vote failures appear in boot logs.

## Phase 7 — UFS storage

Status: Canoe-specific source compiles in isolation; framework port not complete.

Preferred approach:

- Port Canoe register tables and calibration data into the mainline QMP UFS PHY driver.
- Avoid importing the complete downstream `phy-qcom-ufs` framework.
- Add compatible matching, clocks, resets, supplies, and GDSC references.
- Enable the UFS host only after GCC and interconnect dependencies work.

Exit criteria:

- PHY and host probe without errors.
- The controller identifies storage reliably across cold boots.
- Read-only filesystem or block-device tests pass before any write test.

## Phase 8 — Core platform stability

Status: not started.

- SMMU mappings for enabled clients.
- Thermal sensors and conservative thermal policy.
- Watchdog.
- CPU frequency and idle states.
- Restart and power-off behavior.
- Ramoops or another persistent crash-log path.

Exit criteria:

- Repeated cold and warm boots succeed.
- No thermal or power-domain regressions appear during sustained idle testing.
- Crash logs survive unexpected resets.

## Phase 9 — Peripheral enablement

Order peripherals by dependency and recovery risk:

1. USB device mode.
2. USB host mode.
3. PCIe.
4. Display and GPU.
5. Audio.
6. Wi-Fi and Bluetooth.
7. Modem and remote processors.
8. Sensors and haptics.
9. Camera.
10. Charging and advanced power policy.

Each subsystem should have its own branch and status entry.

## Phase 10 — Upstream-quality cleanup

- Split bindings, drivers, and DTS into logical patch series.
- Run `checkpatch.pl`, kernel-doc checks, sparse where useful, and DT schema validation.
- Remove downstream naming and unused data.
- Document provenance for imported tables.
- Rebase onto a supported upstream kernel after the 6.12 bring-up is stable.

## Non-goals for early phases

- Carrying the entire Android vendor kernel.
- Making all OEM Android modules build against mainline.
- Enabling camera, display, or modem before early serial and storage work.
- Hiding compiler errors by replacing upstream framework headers.
- Treating a downstream DTB that compiles as an upstream-ready device tree.

## Milestone discipline

A phase is complete only when:

- the source change is committed;
- the relevant object, DTB, or image builds;
- logs and configuration are retained;
- the status document is updated;
- the next phase starts from the compact repository history.
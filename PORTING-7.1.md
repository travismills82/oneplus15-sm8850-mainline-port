# OnePlus 15 Canoe migration to Linux v7.1.1

## Baselines

- Historical Linux 6.12.23 development snapshot: `5555043e466f5a12dbc029254883f3f8ed7061cd`
- Active kernel baseline: `v7.1.1`
- Upstream Linux 7.1.1 commit: `c9acdc466e9aa96352f658b9276aa8a45b8e817d`
- Compact Linux 7.1.1 snapshot: `680393dbcf5c2db5177bae8852f6bac86b6080a3`
- Active integration branch: `bringup/canoe-v7.1`

The Linux 6.12.23 snapshot is historical reference material only. New work must be based on the compact Linux 7.1.1 history.

## Integrated on Linux 7.1.1

The following subsystems have been independently adapted and compile-validated:

1. Canoe TLMM/pinctrl
2. Canoe global clock controller
3. Canoe TCSR reference clock controller
4. Canoe HLOS-voted RPMh interconnect fabrics
5. Canoe UFS Gear 5 QMP PHY
6. Canoe UFS host binding and MCQ resources

Linux 7.1 already provided several facilities that the earlier tree carried privately, including Taycan EKO-T PLL support, UFS v7 QMP register definitions, generalized UFS QMP table overlays, Qualcomm UFS MCQ handling, and PMIC530 regulator hardware types.

## Current migration order

1. Finish the minimal Canoe RPMh clock descriptor.
2. Add the missing PMH0110 14-rail RPMh regulator table and OF match.
3. Audit all minimal DTS prerequisites.
4. Port the minimal Canoe SoC DTS.
5. Port the OnePlus 15 board DTS.
6. Resolve targeted `dtbs_check` failures.
7. Build a built-in ARM64 kernel and exact OnePlus 15 DTB.
8. Audit boot and vendor-boot image packaging.
9. Perform a separately reviewed guarded hardware test.

## Validation policy

Each subsystem must be reviewed, adapted, built, and committed independently. Shared framework changes from the historical tree must not be copied unless Linux 7.1 compilation or binding validation proves they are still required.

A successful compile, schema check, kernel build, or image build is not hardware validation. Do not flash Linux 7.1.1 artifacts until the kernel, DTB, image audit, inactive-slot stage, rollback path, and observer have all passed review.

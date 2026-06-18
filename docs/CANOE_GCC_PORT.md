# Canoe global clock controller port

This document records the first compile-and-link milestone for the Qualcomm
Canoe global clock controller used by the OnePlus 15 SM8850 platform.

## Validation status

The driver is integrated into the Linux 6.12 Qualcomm common-clock framework
and was built into a complete ARM64 kernel on branch
`bringup/canoe-gcc`.

Validated:

- `CONFIG_COMMON_CLK_QCOM=y`
- `CONFIG_QCOM_GDSC=y`
- `CONFIG_SM_GCC_CANOE=y`
- `drivers/clk/qcom/clk-alpha-pll.o` compiles
- `drivers/clk/qcom/gcc-canoe.o` compiles
- complete `Image modules` build succeeds
- `gcc_canoe_probe` and `gcc_canoe_driver` are present in `System.map`
- no Canoe GCC or alpha-PLL warnings were emitted

Build artifacts from the final validation:

| Artifact | Size | SHA-256 |
|---|---:|---|
| `Image` | 38795776 bytes | `91f034cd0e8de5da08a46f1b054a5892ec3dc639b293b425cd764b9399c3a261` |
| `vmlinux` | 401834392 bytes | `7a6afe01a9ea7a417a2aa4f9f115c2bc423edf0ae8f4957946b09bae98c96929` |

This is compile-and-link validation only. It is not hardware validation.

## Source adaptation

The Canoe data originated from the public OnePlus SM8850 Android 16 source and
was adapted to the Linux 6.12 Qualcomm clock interfaces.

### Taycan/EKO-T PLL

Linux 6.12 did not yet contain the Taycan/EKO-T PLL register layout needed by
Canoe. The port adds the newer upstream register layout and aliases its clock
operations to the compatible Lucid EVO implementations.

### Removed downstream voltage voting

The downstream driver contained:

- one GCC regulator-definition block;
- 51 per-clock VDD voting blocks;
- descriptor-level clock-regulator arrays.

Linux 6.12 does not expose the same downstream `clk_regmap.vdd_data` and
clock-regulator model. Those fields were removed for this milestone.

A mainline power/corner-management replacement has not yet been implemented.
Runtime testing must verify that firmware and existing boot-time configuration
leave the required PLLs and root clocks at usable voltage corners.

### Hardware-controlled root clocks

Forty-five downstream `HW_CLK_CTRL_MODE` flags were translated to the
Linux 6.12 `struct clk_rcg2.hw_clk_ctrl` Boolean.

### Removed downstream-only metadata

The adaptation removes:

- 45 `enable_safe_config` initializers;
- one Qualcomm clock-minidump flag;
- eight `CLK_DONT_HOLD_STATE` flags;
- the downstream `qcom_cc_sync_state()` callback.

These interfaces do not exist in the Linux 6.12 Qualcomm clock framework.

### Hardware-control branch clocks

Five users of the downstream-only `clk_branch2_hw_ctl_ops` were mapped to
the standard Linux 6.12 `clk_branch2_ops` implementation to complete the
compile-and-link milestone.

This is the most important remaining runtime risk in the driver. The mapping
must be checked against register behavior during early serial and UFS testing.
It must not be described as hardware-validated support.

## Remaining work

Before calling the GCC hardware support complete:

1. Add and validate a DT schema for `qcom,canoe-gcc`.
2. Review clock parent input indices against the reduced Canoe DTS.
3. Decide whether the five hardware-control branch clocks need a dedicated
   mainline operation or should be omitted from consumers.
4. Validate PLL, RCG, branch, reset, and GDSC register behavior through early
   serial logs.
5. Validate UFS clock rates and parent switching.
6. Replace or justify the removed downstream voltage/corner voting.
7. Confirm suspend/resume behavior after PDC and RPMh support are operational.

## Safety

The generated `Image` is not ready to flash. Successful linking proves only
that the Canoe GCC code is internally compatible with this kernel tree.

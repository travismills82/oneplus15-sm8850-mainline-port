# Canoe GICv3 provenance audit

## Verdict

HOLD_FOR_MORE_EVIDENCE

The historical Canoe DTS is the only Canoe-specific source for the register layout. Kaanapali matches most values exactly, but the repository provides no evidence that Kaanapali and Canoe/SM8850 are register-compatible. Nearby Qualcomm designs also demonstrate material variation in GIC base addresses, redistributor sizes, interrupt-cell counts, and maintenance polarity.

## Audited baseline

- Current commit: `7da2b8056503ca8d5435f909016af3a5ebe0e26b`
- Current DTSI: `arch/arm64/boot/dts/qcom/canoe.dtsi`
- Current board DTS: `arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts`
- Historical commit: `5555043e466f5a12dbc029254883f3f8ed7061cd`
- Historical DTS: `arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts`
- Current schema: `Documentation/devicetree/bindings/interrupt-controller/arm,gic-v3.yaml`
- Relevant headers:
  - `include/dt-bindings/interrupt-controller/arm-gic.h`
  - `include/dt-bindings/interrupt-controller/irq.h`

The worktree was clean and detached at the stated current commit. The current Canoe DTSI contains clocks, SCM, PSCI, and `chosen`; it has no GIC or `soc` node. The board DTS only supplies the model and compatible strings.

## Historical candidate

```dts
/ {
	interrupt-parent = <&intc>;

	soc@0 {
		compatible = "simple-bus";
		#address-cells = <2>;
		#size-cells = <2>;
		ranges = <0x0 0x0 0x0 0x0 0x10 0x0>;

		intc: interrupt-controller@17000000 {
			compatible = "arm,gic-v3";
			reg = <0x0 0x17000000 0x0 0x10000>,
			      <0x0 0x17080000 0x0 0x200000>;
			#interrupt-cells = <3>;
			interrupt-controller;
			#redistributor-regions = <1>;
			redistributor-stride = <0x0 0x40000>;
			interrupts = <GIC_PPI 9 IRQ_TYPE_LEVEL_HIGH>;
		};
	};
};
```

## Binding requirements

The current GICv3 schema directly requires:

- `compatible`
- `reg`, with at least two regions:
  1. GIC distributor
  2. One or more redistributor regions

The inherited generic interrupt-controller schema requires the interrupt-controller declaration and interrupt-cell encoding:

- `interrupt-controller`
- `#interrupt-cells`

Relevant constraints and optional properties:

- `compatible` may be `"arm,gic-v3"`.
- `#interrupt-cells` must be `3` or `4`.
- `interrupts` is optional and describes one VGIC maintenance interrupt.
- `#redistributor-regions` is required only when more than one independent redistributor region exists.
- `redistributor-stride` is optional, nonzero, and a multiple of `0x10000`.
- `#address-cells`, `#size-cells`, and `ranges` are relevant only when the GIC has child nodes such as an ITS.
- ITS child nodes are optional.
- `dma-noncoherent`, MSI/MBI properties, clocks, power domains, and resets are optional hardware-specific properties and lack Canoe evidence.

The schema does not require CPU or timer nodes. A standalone GIC node can therefore be structurally schema-valid before those nodes exist.

The GIC maintenance `interrupts` property still needs an interrupt parent. With the historical representation, root `interrupt-parent = <&intc>` supplies it. Root `interrupt-parent` is not itself mandated by `arm,gic-v3.yaml`, but is required for resolving that interrupt specifier unless an equivalent explicit parent is supplied.

## Evidence matrix

| Property | Candidate value | Canoe-specific source | Independent corroboration | Binding status | Confidence | Decision |
|---|---|---|---|---|---|---|
| compatible | `"arm,gic-v3"` | Historical Canoe DTS | All compared Qualcomm GICv3 nodes | Allowed and mandatory | HIGH | ACCEPT |
| distributor base | `0x17000000` | Historical Canoe DTS only | Kaanapali, Hamoa/X1E80100 and Glymur use same base; SM8750/SM8650 differ | Hardware-defined `reg` value | LOW | BLOCK |
| distributor size | `0x10000` | Historical Canoe DTS only | Consistent across all credible comparisons | Hardware-defined `reg` value | MEDIUM | BLOCK |
| redistributor base | `0x17080000` | Historical Canoe DTS only | Kaanapali, Hamoa and Glymur match; SM8750/SM8650 differ | Hardware-defined `reg` value | LOW | BLOCK |
| redistributor size | `0x200000` | Historical Canoe DTS only | Kaanapali matches; Hamoa uses `0x300000`, Glymur `0x480000` | Hardware-defined `reg` value | LOW | BLOCK |
| #interrupt-cells | `<3>` | Historical Canoe DTS | Kaanapali, Hamoa, SM8750 and Glymur use 3; SM8650 uses 4 | Mandatory; 3 or 4 allowed | MEDIUM | ACCEPT |
| interrupt-controller | present | Historical Canoe DTS | Universal in compared nodes | Mandatory through generic schema | HIGH | ACCEPT |
| #redistributor-regions | `<1>` | Historical Canoe DTS only | Kaanapali, Hamoa, SM8750 and SM8650 use 1 | Optional for one region | MEDIUM | OMIT |
| redistributor-stride | `<0x0 0x40000>` | Historical Canoe DTS only | Kaanapali, Hamoa, SM8750, SM8650 and SM8450 match; older platforms use `0x20000` | Optional; value satisfies schema | LOW | BLOCK |
| maintenance PPI number | `GIC_PPI 9` | Historical Canoe DTS only | Universal in compared Qualcomm examples | Optional hardware interrupt | MEDIUM | BLOCK |
| maintenance PPI trigger | `IRQ_TYPE_LEVEL_HIGH` | Historical Canoe DTS only | Kaanapali, Hamoa, SM8750 and SM8450 match; SM8650/SM8550 use level-low | Optional hardware interrupt | LOW | BLOCK |
| root interrupt-parent | `<&intc>` | Historical Canoe DTS | Kaanapali, Hamoa and other Qualcomm top-level DTSIs use it | Not GIC-schema-mandated; needed to resolve historical maintenance interrupt | HIGH | ACCEPT |
| parent bus and ranges | `soc@0`, 64-bit cells, bounded identity range | Historical Canoe DTS | Qualcomm DTSIs conventionally use an SoC simple-bus; current Canoe root already has matching cells | GIC may legally be directly under root; historical bus is not mandatory | HIGH | OMIT |

`BLOCK` means the property needs independent Canoe-specific evidence before implementation, not that the syntax violates the binding.

## Current-tree comparisons

- Kaanapali has the strongest syntactic match: distributor `0x17000000/0x10000`, redistributor `0x17080000/0x200000`, 3 cells, one region, `0x40000` stride, and PPI 9 level-high. However, no inspected binding, compatible declaration, or history identifies Kaanapali as Canoe/SM8850 or establishes register compatibility. It is supporting convention only.

- Hamoa/X1E80100 uses the same distributor and redistributor bases, three interrupt cells, `0x40000` stride, and PPI 9 level-high. Its redistributor range is `0x300000`, reflecting twelve redistributors rather than the candidate’s eight-sized window.

- Glymur uses the same bases and maintenance interrupt but a `0x480000` redistributor range and omits explicit region-count and stride properties. This demonstrates that matching bases do not prove the redistributor window.

- SM8750 uses the same sizes, stride, region count, and interrupt encoding, but bases them at `0x16000000` and `0x16080000`. Its layout pattern supports plausibility, not Canoe addresses.

- SM8650 uses `0x17100000`/`0x17180000`, four interrupt cells with PPI partitions, and a level-low maintenance interrupt. It directly demonstrates that nearby generation is insufficient evidence.

- SM8550 likewise uses four cells and a level-low maintenance PPI.

- SM8450 uses three cells, PPI 9 level-high, one region, and `0x40000` stride, but different bases. Older SM8350/SM8250 generations use a `0x20000` stride and smaller redistributor range.

None of these comparisons independently proves Canoe’s hardware layout.

## Minimal proposed scope

If the missing evidence is obtained, an isolated implementation should change exactly:

- `arch/arm64/boot/dts/qcom/canoe.dtsi`

No board DTS, binding, header, Makefile, CPU, timer, memory, RSC, Command DB, UART, or other node should be changed.

The node may be placed directly under `/`; creating a new `soc@0` solely for the GIC is not required by the binding. The only necessary include is:

```dts
#include <dt-bindings/interrupt-controller/arm-gic.h>
```

That header already includes `irq.h`, so a separate `irq.h` include is unnecessary.

Optional ITS, MSI/MBI, clocks, power domains, resets, noncoherency, address-cell, size-cell, ranges, and redistributor-region properties should be omitted unless independently substantiated or needed by children.

## Candidate implementation

Not provided.

## Validation plan

Use a separate output directory after implementation:

```sh
make O=out ARCH=arm64 defconfig
make O=out ARCH=arm64 qcom/canoe-oneplus-15.dtb
```

Validate the binding and selected DT schemas using colon-separated filters:

```sh
make O=out ARCH=arm64 dt_binding_check \
  DT_SCHEMA_FILES=interrupt-controller/arm,gic-v3.yaml:arm/qcom.yaml

make O=out ARCH=arm64 dtbs_check \
  DT_SCHEMA_FILES=interrupt-controller/arm,gic-v3.yaml:arm/qcom.yaml \
  qcom/canoe-oneplus-15.dtb
```

Directly confirm the generated DTB with the processed schema:

```sh
dt-validate \
  -s out/Documentation/devicetree/bindings/processed-schema.json \
  out/arch/arm64/boot/dts/qcom/canoe-oneplus-15.dtb
```

Run source and patch checks:

```sh
git diff --check
scripts/checkpatch.pl --strict --no-tree \
  arch/arm64/boot/dts/qcom/canoe.dtsi
```

For a commit-based review, prefer:

```sh
scripts/checkpatch.pl --strict -g HEAD
```

`dt-validate` was not installed in the audited environment, `.config` was absent, and no builds were run because this was a read-only audit.

## Unresolved risks

- No independent Canoe/SM8850 source confirms either GIC base.
- No independent Canoe source confirms either register-region size.
- No independent Canoe source confirms eight redistributor frames or a `0x200000` window.
- No independent Canoe source confirms the `0x40000` redistributor stride.
- No independent Canoe source confirms one contiguous redistributor region.
- No independent Canoe source confirms PPI 9 as the maintenance interrupt.
- No independent Canoe source confirms level-high polarity.
- No evidence establishes Kaanapali-to-Canoe register compatibility.
- The candidate contains no ITS description; this is schema-valid but says nothing about eventual MSI support.
- No hardware validation was performed or inferred.

## Recommended next step

Identify independent Canoe/SM8850 evidence for the GICD/GICR register ranges, redistributor topology/stride, and maintenance PPI number and polarity, then stop until that evidence is available.

## Commands run

- `git status --short --branch`, `git rev-parse HEAD`, and `git show -s`: confirmed a clean detached worktree at `7da2b805...`.
- `sed` on current Canoe DTS/DTSI: confirmed no current GIC or SoC bus.
- Exact `git show 5555043e...:arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts`: recovered the historical candidate.
- `sed` on `arm,gic-v3.yaml`, `arm-gic.h`, and `irq.h`: established schema constraints and macro values.
- `rg` across Qualcomm DTS/DTSI files: found Kaanapali, Hamoa, Glymur, SM8750, SM8650, SM8550, SM8450, and older comparisons.
- `git log -S`, `git log -G`, and path history: traced the Canoe node to commit `01d9e5b98...`; no second Canoe-specific provenance source was found.
- `git show 01d9e5b98...`: confirmed the node entered as part of an early bring-up commit without cited hardware provenance.
- Inspection of schema Makefiles and `writing-schema.rst`: confirmed colon-separated `DT_SCHEMA_FILES`, `dt_binding_check`, `dtbs_check`, and direct `dt-validate` workflow.
- Final `git status --short --branch`: confirmed no files were changed.
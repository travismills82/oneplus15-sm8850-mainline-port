You are performing an evidence-first, read-only provenance audit for the next
step of the OnePlus 15 (Qualcomm Canoe / SM8850) Linux 7.1.1 mainline port.

Repository and safety rules:
- Work only in the current detached worktree.
- Do not modify, create, delete, stage, commit, or push any file.
- Do not create or switch branches or worktrees.
- Do not install packages or fetch new remotes.
- Do not use sudo.
- Do not access phones, USB devices, ADB, Fastboot, block devices, partitions,
  slots, reboot, flashing tools, or physical hardware.
- You may inspect the local Git history, tracked source files, and bindings.
- This is an audit only, not an implementation task.

Exact project state:
- Current merged main commit:
  7da2b8056503ca8d5435f909016af3a5ebe0e26b
- Current Canoe DTSI:
  arch/arm64/boot/dts/qcom/canoe.dtsi
- Historical reference commit:
  5555043e466f5a12dbc029254883f3f8ed7061cd
- Historical reference DTS:
  arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts
- Current GICv3 schema:
  Documentation/devicetree/bindings/interrupt-controller/arm,gic-v3.yaml

Historical candidate to investigate, not blindly accept:
- node: intc: interrupt-controller@17000000
- compatible: "arm,gic-v3"
- distributor: base 0x17000000, size 0x10000
- redistributor: base 0x17080000, size 0x200000
- #interrupt-cells = <3>
- interrupt-controller
- #redistributor-regions = <1>
- redistributor-stride = <0x0 0x40000>
- maintenance interrupt: GIC_PPI 9 IRQ_TYPE_LEVEL_HIGH
- historical root interrupt-parent = <&intc>
- historical node lived under soc@0 with a simple-bus range.

Audit tasks:
1. Inspect the current Canoe DTSI and board DTS.
2. Inspect the exact historical node with:
   git show 5555043e466f5a12dbc029254883f3f8ed7061cd:arch/arm64/boot/dts/qcom/canoe-oneplus-15.dts
3. Inspect the full current GICv3 binding and relevant dt-bindings headers.
4. Search the current tree and Git history for modern Qualcomm GICv3 examples,
   prioritizing nearby generations such as SM8750, SM8650, X1E80100, and other
   credible Qualcomm ARM64 platforms.
5. Search for independent corroboration of:
   - distributor base and size;
   - redistributor base and size;
   - redistributor stride;
   - redistributor region count;
   - maintenance PPI and trigger;
   - number of interrupt cells;
   - required root interrupt-parent;
   - required parent bus/ranges representation.
6. Determine whether adding only the GIC node is schema-valid before CPU and
   timer nodes exist.
7. Determine the minimal required include files and whether any optional GIC
   properties should be omitted.
8. Distinguish clearly between:
   - directly proven Canoe-specific facts;
   - binding-mandated facts;
   - convention-only supporting evidence;
   - unresolved or speculative values.
9. Do not assume SM8750 and Canoe are register-compatible merely because they
   are nearby generations.
10. Do not recommend timer, CPU, memory, RSC, Command DB, UART, or other nodes
    as part of the same patch.

Required report format:
# Canoe GICv3 provenance audit

## Verdict
Use exactly one of:
- READY_FOR_ISOLATED_IMPLEMENTATION
- HOLD_FOR_MORE_EVIDENCE

Give a concise reason.

## Audited baseline
List the exact current and historical commits/files.

## Historical candidate
Provide the candidate node as a fenced DTS snippet, preserving exact values.

## Binding requirements
List mandatory and relevant optional properties from the current schema.

## Evidence matrix
Use a Markdown table with these rows:
- compatible
- distributor base
- distributor size
- redistributor base
- redistributor size
- #interrupt-cells
- interrupt-controller
- #redistributor-regions
- redistributor-stride
- maintenance PPI number
- maintenance PPI trigger
- root interrupt-parent
- parent bus and ranges

Columns:
Property | Candidate value | Canoe-specific source | Independent corroboration |
Binding status | Confidence | Decision

Confidence must be one of HIGH, MEDIUM, LOW.
Decision must be one of ACCEPT, OMIT, BLOCK.

## Current-tree comparisons
Discuss each credible Qualcomm comparison and its limitations.

## Minimal proposed scope
State exactly which files an isolated implementation would change. Do not
implement them.

## Candidate implementation
Provide the minimal proposed Canoe DTS/DTSI snippet only if the verdict is
READY_FOR_ISOLATED_IMPLEMENTATION. Otherwise write "Not provided."

## Validation plan
Give exact build/schema/checkpatch commands suitable for this Linux tree,
including colon-separated DT_SCHEMA_FILES and direct dt-validate confirmation.

## Unresolved risks
List every remaining uncertainty.

## Recommended next step
State either:
- implement the isolated GIC node; or
- identify the exact missing evidence and stop.

## Commands run
List the inspection commands used and summarize their outcomes.

Be conservative. A historical DTS alone is not independent corroboration.
Do not claim hardware validation.

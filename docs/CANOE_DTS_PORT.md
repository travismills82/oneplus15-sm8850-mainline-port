# OnePlus 15 Canoe device-tree bring-up

This document records the first schema-clean, compile-only device-tree
milestone for the OnePlus 15 on the Qualcomm Canoe platform.

## Validation

The following checks pass on branch `bringup/canoe-dts`:

- all four modified or new DT bindings pass `dt_binding_check`;
- the ARM64 kernel Image builds;
- `qcom/canoe-oneplus-15.dtb` builds;
- `dtbs_check W=1` completes without diagnostics;
- `git diff --check` passes.

Artifacts from the final validation:

| Artifact | Size | SHA-256 |
|---|---:|---|
| `Image` | 38795776 bytes | `91f034cd0e8de5da08a46f1b054a5892ec3dc639b293b425cd764b9399c3a261` |
| `canoe-oneplus-15.dtb` | 4364 bytes | `e3b61398a125febde39b40198266a0adab643d41716107754e0c833e93696f35` |

## Included hardware

The reduced device tree currently describes only:

- eight CPUs and PSCI;
- the ARM generic timer;
- GICv3;
- Canoe GCC;
- Canoe TLMM;
- QUPv3 wrapper 1;
- the SE7 GENI debug UART;
- UART TX on GPIO 62 and RX on GPIO 63;
- fixed placeholders for external GCC clock inputs.

The early console is configured for:

```text
console=ttyMSM0,115200n8
earlycon=qcom_geni,0x00a9c000
```

## Deliberate limitations

This is not a complete board description.

Not yet included:

- RPMh and RPMh power domains;
- PDC wake routing;
- interconnect providers;
- production regulator topology;
- reserved-memory carveouts;
- UFS, USB and PCIe;
- remote processors;
- display, audio, camera and sensors.

The DRAM node follows the Qualcomm mobile convention:

```dts
memory@a0000000 {
	device_type = "memory";
	reg = <0x0 0xa0000000 0x0 0x0>;
};
```

The bootloader must replace the zero size with the actual DRAM size.

The external GCC inputs are temporary fixed-clock placeholders and require
runtime verification before later peripheral bring-up.

## Safety

This milestone is compile and schema validation only. The DTB has not been
booted or hardware-validated and must not be flashed.

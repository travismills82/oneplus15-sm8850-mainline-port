# OnePlus 15 Canoe migration to Linux v7.1.1

## Baselines

- Previous development commit: `5555043e466f5a12dbc029254883f3f8ed7061cd`
- Previous kernel baseline: `v6.12.23`
- New kernel baseline: `v7.1.1`
- New upstream commit: `c9acdc466e9aa96352f658b9276aa8a45b8e817d`
- Compact snapshot commit: `680393dbcf5c2db5177bae8852f6bac86b6080a3`

## Migration state

The Linux v7.1.1 baseline and project scaffold are established.

The previous kernel-source delta has been exported and dry-run against v7.1.1.
Kernel source has **not** yet been accepted into this branch. Each subsystem
must be reviewed, adapted, built, and committed independently.

Recommended replay order:

1. Device-tree bindings and identifiers
2. Canoe TLMM/pinctrl
3. GCC and TCSR clock controllers
4. RPMh interconnect providers
5. QMP UFS PHY and UFS host resources
6. Minimal Canoe SoC and OnePlus 15 device tree
7. Early console and observability configuration
8. Boot-image packaging and guarded hardware tests

The Linux 6.12 branch remains preserved at:

`backup/canoe-6.12-final-5555043`

Do not flash artifacts from this branch until a fresh v7.1.1 build, DT validation,
image audit, inactive-slot stage, and guarded observer have all passed.

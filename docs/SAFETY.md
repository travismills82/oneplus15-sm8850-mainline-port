# Device safety

The included tools never write a phone partition. Keep that property until a
known-good stock recovery path is verified.

## Before testing any kernel

1. Record the exact OxygenOS/ColorOS build, active slot, bootloader state, and
   anti-rollback state.
2. Back up `boot`, `init_boot`, `vendor_boot`, `vendor_kernel_boot`, `dtbo`,
   available `vbmeta*` partitions, and `recovery` from the currently running slot
   with `portctl stock-images`.
3. Save hashes and a second offline copy of those images.
4. Confirm that the device can enter bootloader/fastboot mode and that the host
   can see it before changing anything.
5. Prefer a temporary `fastboot boot` test only when the device and image format
   explicitly support it. Do not assume this is available.
6. Do not disable AVB, alter vbmeta, change slots, downgrade firmware, or flash a
   generated image as part of an automated script.

Recent OnePlus Android 16 releases have generated anti-rollback warnings in the
modding community. Treat rollback and cross-version flashing as potentially
irreversible until verified for the exact device build.

## Debugging sequence

Use the least invasive evidence first:

- compile and link logs
- `dtc` warnings and DT schema validation
- QEMU/virt arm64 build sanity where applicable
- temporary boot, if supported
- earlycon/serial output
- pstore/ramoops after a failed boot
- stock-kernel module and symbol comparison

Never use the only daily-use phone as the sole recovery device or credential
holder during bring-up.

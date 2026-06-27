#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

from __future__ import annotations

import os
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
OLD_REF = os.environ.get(
    "OLD_REF",
    "origin/backup/canoe-6.12-final-5555043",
)

DRIVER = Path("drivers/clk/qcom/gcc-canoe.c")
HEADER = Path("include/dt-bindings/clock/qcom,gcc-canoe.h")
KCONFIG = Path("drivers/clk/qcom/Kconfig")
MAKEFILE = Path("drivers/clk/qcom/Makefile")

KCONFIG_BLOCK = """config SM_GCC_CANOE
\ttristate \"Canoe Global Clock Controller\"
\tdepends on ARM64 || COMPILE_TEST
\tselect QCOM_GDSC
\thelp
\t  Support for the global clock controller on Qualcomm Canoe devices.
\t  Say Y if you want to use peripheral devices such as UART, SPI, I2C,
\t  USB, UFS, SD/eMMC or PCIe.

"""

MAKEFILE_LINE = "obj-$(CONFIG_SM_GCC_CANOE) += gcc-canoe.o\n"


def git_show(path: Path) -> bytes:
    return subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{OLD_REF}:{path.as_posix()}"]
    )


def import_file(path: Path) -> None:
    destination = ROOT / path
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(git_show(path))


def insert_after_config(text: str, symbol: str, block: str) -> str:
    lines = text.splitlines(keepends=True)
    starts = [
        index
        for index, line in enumerate(lines)
        if re.fullmatch(rf"config {re.escape(symbol)}\s*", line)
    ]
    if len(starts) != 1:
        raise RuntimeError(f"expected one {symbol} config block")

    start = starts[0]
    end = len(lines)
    for index in range(start + 1, len(lines)):
        if re.fullmatch(r"config [A-Z0-9_]+\s*", lines[index]):
            end = index
            break

    lines.insert(end, block)
    return "".join(lines)


def adapt_kconfig() -> None:
    path = ROOT / KCONFIG
    text = path.read_text()
    if "config SM_GCC_CANOE" in text:
        raise RuntimeError("SM_GCC_CANOE already exists")

    text = insert_after_config(text, "SM_GCC_8750", KCONFIG_BLOCK)
    if text.count("config SM_GCC_CANOE") != 1:
        raise RuntimeError("GCC Kconfig insertion count is not one")

    match = re.search(
        r"^config SM_GCC_CANOE\n(?P<body>.*?)(?=^config )",
        text,
        re.MULTILINE | re.DOTALL,
    )
    if not match:
        raise RuntimeError("could not parse inserted GCC block")

    body = match.group(0)
    for required in (
        'tristate "Canoe Global Clock Controller"',
        "depends on ARM64 || COMPILE_TEST",
        "select QCOM_GDSC",
    ):
        if required not in body:
            raise RuntimeError(f"missing Kconfig requirement: {required}")

    path.write_text(text)


def adapt_makefile() -> None:
    path = ROOT / MAKEFILE
    text = path.read_text()
    if "CONFIG_SM_GCC_CANOE" in text:
        raise RuntimeError("SM_GCC_CANOE already exists in Makefile")

    anchor = "obj-$(CONFIG_SM_GCC_8750) += gcc-sm8750.o\n"
    if text.count(anchor) != 1:
        raise RuntimeError("SM8750 GCC Makefile anchor is missing or ambiguous")

    text = text.replace(anchor, anchor + MAKEFILE_LINE, 1)
    if text.count(MAKEFILE_LINE) != 1:
        raise RuntimeError("Canoe GCC Makefile insertion count is not one")
    path.write_text(text)


def verify() -> None:
    driver = (ROOT / DRIVER).read_text()
    header = (ROOT / HEADER).read_text()
    kconfig = (ROOT / KCONFIG).read_text()
    makefile = (ROOT / MAKEFILE).read_text()

    checks = {
        "driver compatible": 'qcom,canoe-gcc' in driver,
        "probe function": "gcc_canoe_probe" in driver,
        "Taycan PLL type": "CLK_ALPHA_PLL_TYPE_TAYCAN_EKO_T" in driver,
        "Taycan PLL ops": "clk_alpha_pll_fixed_taycan_eko_t_ops" in driver,
        "clock binding content": "GCC_AGGRE_NOC_PCIE_AXI_CLK" in header,
        "Kconfig symbol": kconfig.count("config SM_GCC_CANOE") == 1,
        "Makefile object": makefile.count("CONFIG_SM_GCC_CANOE") == 1,
    }
    failed = [name for name, passed in checks.items() if not passed]
    if failed:
        raise RuntimeError("verification failed: " + ", ".join(failed))


def main() -> None:
    os.chdir(ROOT)
    import_file(DRIVER)
    import_file(HEADER)
    adapt_kconfig()
    adapt_makefile()
    verify()


if __name__ == "__main__":
    main()

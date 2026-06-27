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

DRIVER = Path("drivers/clk/qcom/tcsrcc-canoe.c")
HEADER = Path("include/dt-bindings/clock/qcom,canoe-tcsr.h")
KCONFIG = Path("drivers/clk/qcom/Kconfig")
MAKEFILE = Path("drivers/clk/qcom/Makefile")

KCONFIG_BLOCK = """config SM_TCSRCC_CANOE
\ttristate \"Canoe TCSR Clock Controller\"
\tdepends on ARM64 || COMPILE_TEST
\tselect QCOM_GDSC
\thelp
\t  Support for the TCSR clock controller on Qualcomm Canoe platforms.
\t  The controller provides reference-clock gates for PCIe, UFS and
\t  USB PHYs. These gates allow the corresponding PHY blocks to use
\t  the external reference clock. Say Y or M when they are required.

"""

MAKEFILE_LINE = "obj-$(CONFIG_SM_TCSRCC_CANOE) += tcsrcc-canoe.o\n"


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
    if "config SM_TCSRCC_CANOE" in text:
        raise RuntimeError("SM_TCSRCC_CANOE already exists")

    text = insert_after_config(text, "SM_TCSRCC_8750", KCONFIG_BLOCK)
    if text.count("config SM_TCSRCC_CANOE") != 1:
        raise RuntimeError("TCSRCC Kconfig insertion count is not one")

    match = re.search(
        r"^config SM_TCSRCC_CANOE\n(?P<body>.*?)(?=^config )",
        text,
        re.MULTILINE | re.DOTALL,
    )
    if not match:
        raise RuntimeError("could not parse inserted TCSRCC block")

    body = match.group(0)
    for required in (
        'tristate "Canoe TCSR Clock Controller"',
        "depends on ARM64 || COMPILE_TEST",
        "select QCOM_GDSC",
    ):
        if required not in body:
            raise RuntimeError(f"missing Kconfig requirement: {required}")

    path.write_text(text)


def adapt_makefile() -> None:
    path = ROOT / MAKEFILE
    text = path.read_text()
    if "CONFIG_SM_TCSRCC_CANOE" in text:
        raise RuntimeError("SM_TCSRCC_CANOE already exists in Makefile")

    anchor = "obj-$(CONFIG_SM_TCSRCC_8750) += tcsrcc-sm8750.o\n"
    if text.count(anchor) != 1:
        raise RuntimeError("SM8750 TCSRCC Makefile anchor is missing or ambiguous")

    text = text.replace(anchor, anchor + MAKEFILE_LINE, 1)
    if text.count(MAKEFILE_LINE) != 1:
        raise RuntimeError("Canoe TCSRCC Makefile insertion count is not one")
    path.write_text(text)


def verify() -> None:
    driver = (ROOT / DRIVER).read_text()
    header = (ROOT / HEADER).read_text()
    kconfig = (ROOT / KCONFIG).read_text()
    makefile = (ROOT / MAKEFILE).read_text()

    checks = {
        "driver compatible": 'qcom,canoe-tcsr' in driver,
        "probe function": "tcsr_cc_canoe_probe" in driver,
        "four clocks": "ARRAY_SIZE(tcsr_cc_canoe_clocks)" in driver,
        "header PCIe id": "TCSR_PCIE_0_CLKREF_EN" in header,
        "header UFS id": "TCSR_UFS_CLKREF_EN" in header,
        "header USB2 id": "TCSR_USB2_CLKREF_EN" in header,
        "header USB3 id": "TCSR_USB3_CLKREF_EN" in header,
        "Kconfig symbol": kconfig.count("config SM_TCSRCC_CANOE") == 1,
        "Makefile object": makefile.count("CONFIG_SM_TCSRCC_CANOE") == 1,
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

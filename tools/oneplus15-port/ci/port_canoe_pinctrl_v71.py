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

BINDING = Path(
    "Documentation/devicetree/bindings/pinctrl/qcom,canoe-tlmm.yaml"
)
DRIVER = Path("drivers/pinctrl/qcom/pinctrl-canoe.c")
KCONFIG = Path("drivers/pinctrl/qcom/Kconfig.msm")
MAKEFILE = Path("drivers/pinctrl/qcom/Makefile")

KCONFIG_BLOCK = """config PINCTRL_CANOE
\ttristate \"Qualcomm Canoe pin controller driver\"
\tdepends on ARM64 || COMPILE_TEST
\thelp
\t  This is the pinctrl, pinmux, pinconf and gpiolib driver for the
\t  Qualcomm TLMM block found on the Qualcomm Canoe platform.

"""

MAKEFILE_LINE = "obj-$(CONFIG_PINCTRL_CANOE)\t+= pinctrl-canoe.o\n"
REMOVE_LINE = "\t.remove_new = msm_pinctrl_remove,\n"


def git_show(ref: str, path: Path) -> bytes:
    return subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{ref}:{path.as_posix()}"]
    )


def import_file(path: Path) -> None:
    destination = ROOT / path
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(git_show(OLD_REF, path))


def adapt_driver() -> None:
    path = ROOT / DRIVER
    text = path.read_text()
    count = text.count(REMOVE_LINE)
    if count != 1:
        raise RuntimeError(
            f"expected one obsolete remove callback, found {count}"
        )
    text = text.replace(REMOVE_LINE, "", 1)
    if "msm_pinctrl_remove" in text or ".remove_new" in text:
        raise RuntimeError("obsolete pinctrl remove API remains in driver")
    path.write_text(text)


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
        if re.fullmatch(r"config PINCTRL_[A-Z0-9_]+\s*", lines[index]):
            end = index
            break

    lines.insert(end, block)
    return "".join(lines)


def adapt_kconfig() -> None:
    path = ROOT / KCONFIG
    text = path.read_text()
    if "config PINCTRL_CANOE" in text:
        raise RuntimeError("PINCTRL_CANOE already exists")

    text = insert_after_config(text, "PINCTRL_APQ8084", KCONFIG_BLOCK)

    if text.count("config PINCTRL_CANOE") != 1:
        raise RuntimeError("PINCTRL_CANOE insertion count is not one")

    match = re.search(
        r"^config PINCTRL_CANOE\n(?P<body>.*?)(?=^config PINCTRL_)",
        text,
        re.MULTILINE | re.DOTALL,
    )
    if not match:
        raise RuntimeError("could not parse inserted PINCTRL_CANOE block")

    body = match.group(0)
    if "select PINCTRL_MSM" in body:
        raise RuntimeError("PINCTRL_CANOE recursively selects PINCTRL_MSM")

    path.write_text(text)


def adapt_makefile() -> None:
    path = ROOT / MAKEFILE
    text = path.read_text()
    if "CONFIG_PINCTRL_CANOE" in text:
        raise RuntimeError("PINCTRL_CANOE already exists in Makefile")

    anchor = "obj-$(CONFIG_PINCTRL_APQ8084)\t+= pinctrl-apq8084.o\n"
    if text.count(anchor) != 1:
        raise RuntimeError("APQ8084 Makefile anchor is missing or ambiguous")

    text = text.replace(anchor, anchor + MAKEFILE_LINE, 1)
    if text.count(MAKEFILE_LINE) != 1:
        raise RuntimeError("Canoe Makefile insertion count is not one")
    path.write_text(text)


def verify() -> None:
    driver = (ROOT / DRIVER).read_text()
    kconfig = (ROOT / KCONFIG).read_text()
    makefile = (ROOT / MAKEFILE).read_text()
    binding = (ROOT / BINDING).read_text()

    checks = {
        "driver compatible": 'qcom,canoe-tlmm' in driver,
        "VM compatible": 'qcom,canoe-vm-tlmm' in driver,
        "probe function": "canoe_tlmm_probe" in driver,
        "obsolete remove callback absent": "msm_pinctrl_remove" not in driver,
        "obsolete remove field absent": ".remove_new" not in driver,
        "Kconfig symbol": kconfig.count("config PINCTRL_CANOE") == 1,
        "Makefile object": makefile.count("CONFIG_PINCTRL_CANOE") == 1,
        "binding compatible": "qcom,canoe-tlmm" in binding,
    }
    failed = [name for name, passed in checks.items() if not passed]
    if failed:
        raise RuntimeError("verification failed: " + ", ".join(failed))


def main() -> None:
    os.chdir(ROOT)
    import_file(BINDING)
    import_file(DRIVER)
    adapt_driver()
    adapt_kconfig()
    adapt_makefile()
    verify()


if __name__ == "__main__":
    main()

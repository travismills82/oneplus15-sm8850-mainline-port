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

DRIVER = Path("drivers/phy/qualcomm/phy-qcom-qmp-ufs.c")
BINDING = Path(
    "Documentation/devicetree/bindings/phy/"
    "qcom,sc8280xp-qmp-ufs-phy.yaml"
)
COM_V6 = Path("drivers/phy/qualcomm/phy-qcom-qmp-qserdes-com-v6.h")
TXRX_V7 = Path("drivers/phy/qualcomm/phy-qcom-qmp-qserdes-txrx-ufs-v7.h")

TABLE_NAMES = (
    "canoe_ufsphy_serdes",
    "canoe_ufsphy_tx",
    "canoe_ufsphy_rx",
    "canoe_ufsphy_pcs",
    "canoe_ufsphy_g4_pcs",
    "canoe_ufsphy_g5_pcs",
    "canoe_ufsphy_hs_b_pcs",
)


def git_show(path: Path) -> str:
    return subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{OLD_REF}:{path.as_posix()}"],
        text=True,
    )


def extract_block(text: str, pattern: str, name: str) -> str:
    match = re.search(pattern, text, re.MULTILINE | re.DOTALL)
    if not match:
        raise RuntimeError(f"could not extract {name} from preserved source")
    return match.group(0).rstrip() + "\n\n"


def adapt_driver() -> None:
    path = ROOT / DRIVER
    current = path.read_text()
    old = git_show(DRIVER)

    prerequisites = {
        "UFS v7 register header": '#include "phy-qcom-qmp-qserdes-txrx-ufs-v7.h"',
        "general table initializer": "static void qmp_ufs_init_all(",
        "adaptive analog register": "QSERDES_V6_COM_ADAPTIVE_ANALOG_CONFIG",
    }
    combined_headers = current + (ROOT / COM_V6).read_text()
    for description, token in prerequisites.items():
        haystack = current if description != "adaptive analog register" else combined_headers
        if token not in haystack:
            raise RuntimeError(f"Linux v7.1 prerequisite missing: {description}")

    for name in TABLE_NAMES:
        if name in current:
            raise RuntimeError(f"{name} already exists in current driver")

    table_blocks = "".join(
        extract_block(
            old,
            rf"static const struct qmp_phy_init_tbl {re.escape(name)}\[\] = \{{.*?^\}};",
            name,
        )
        for name in TABLE_NAMES
    )

    table_anchor = "static const struct qmp_phy_init_tbl sm8650_ufsphy_serdes[] = {"
    if current.count(table_anchor) != 1:
        raise RuntimeError("sm8650 UFS table anchor is missing or ambiguous")
    current = current.replace(table_anchor, table_blocks + table_anchor, 1)

    vreg_block = (
        "static const struct regulator_bulk_data canoe_ufsphy_vreg_l[] = {\n"
        '\t{ .supply = "vdda-phy" },\n'
        '\t{ .supply = "vdda-pll" },\n'
        "};\n\n"
    )
    vreg_anchor = "static const struct regulator_bulk_data milos_ufsphy_vreg_l[] = {"
    if current.count(vreg_anchor) != 1:
        raise RuntimeError("modern UFS regulator-list anchor is missing or ambiguous")
    current = current.replace(vreg_anchor, vreg_block + vreg_anchor, 1)

    cfg_block = extract_block(
        old,
        r"static const struct qmp_phy_cfg canoe_ufsphy_cfg = \{.*?^\};",
        "canoe_ufsphy_cfg",
    )
    if cfg_block.count("qmp_phy_vreg_l") != 2:
        raise RuntimeError("preserved Canoe config has unexpected regulator references")
    cfg_block = cfg_block.replace("qmp_phy_vreg_l", "canoe_ufsphy_vreg_l")

    cfg_anchor = "static const struct qmp_phy_cfg sm8650_ufsphy_cfg = {"
    if current.count(cfg_anchor) != 1:
        raise RuntimeError("sm8650 UFS config anchor is missing or ambiguous")
    current = current.replace(cfg_anchor, cfg_block + cfg_anchor, 1)

    match_anchor = "static const struct of_device_id qmp_ufs_of_match_table[] = {\n"
    if current.count(match_anchor) != 1:
        raise RuntimeError("UFS OF match table anchor is missing or ambiguous")

    match_entry = (
        '\t{\n'
        '\t\t.compatible = "qcom,canoe-qmp-ufs-phy",\n'
        '\t\t.data = &canoe_ufsphy_cfg,\n'
        '\t}, {\n'
    )
    current = current.replace(match_anchor, match_anchor + match_entry, 1)

    # The inserted entry opens the next initializer. Remove the original first
    # opener so the following existing compatible remains syntactically intact.
    duplicate = match_anchor + match_entry + "\t{\n"
    if duplicate not in current:
        raise RuntimeError("could not normalize inserted OF match entry")
    current = current.replace(duplicate, match_anchor + match_entry, 1)

    path.write_text(current)


def adapt_binding() -> None:
    path = ROOT / BINDING
    text = path.read_text()
    compatible = "qcom,canoe-qmp-ufs-phy"
    if compatible in text:
        raise RuntimeError("Canoe UFS PHY compatible already exists in binding")

    enum_anchor = "      - enum:\n          - qcom,milos-qmp-ufs-phy\n"
    if text.count(enum_anchor) != 1:
        raise RuntimeError("main compatible enum anchor is missing")
    text = text.replace(
        enum_anchor,
        "      - enum:\n"
        f"          - {compatible}\n"
        "          - qcom,milos-qmp-ufs-phy\n",
        1,
    )

    conditional_anchor = (
        "            enum:\n"
        "              - qcom,milos-qmp-ufs-phy\n"
    )
    if text.count(conditional_anchor) != 1:
        raise RuntimeError("three-clock compatible enum anchor is missing")
    text = text.replace(
        conditional_anchor,
        "            enum:\n"
        f"              - {compatible}\n"
        "              - qcom,milos-qmp-ufs-phy\n",
        1,
    )

    if text.count(compatible) != 2:
        raise RuntimeError("Canoe compatible was not inserted exactly twice")
    path.write_text(text)


def verify() -> None:
    driver = (ROOT / DRIVER).read_text()
    binding = (ROOT / BINDING).read_text()
    txrx = (ROOT / TXRX_V7).read_text()
    com = (ROOT / COM_V6).read_text()

    checks = {
        "Canoe config": driver.count("canoe_ufsphy_cfg") >= 2,
        "Canoe OF match": driver.count("qcom,canoe-qmp-ufs-phy") == 1,
        "Gear 5 support": ".max_supported_gear\t= UFS_HS_G5" in driver,
        "two lanes": ".lanes\t\t\t= 2" in driver,
        "Canoe regulator list": driver.count("canoe_ufsphy_vreg_l") == 3,
        "UFS v7 TX macro": "QSERDES_UFS_V7_TX_LANE_MODE_1" in txrx,
        "UFS v7 RX macro": "QSERDES_UFS_V7_RX_MODE_RATE4_SB_B7" in txrx,
        "adaptive PLL macro": "QSERDES_V6_COM_ADAPTIVE_ANALOG_CONFIG" in com,
        "binding compatible": binding.count("qcom,canoe-qmp-ufs-phy") == 2,
        "generalized overlays": "qmp_ufs_init_all" in driver,
    }
    failed = [name for name, passed in checks.items() if not passed]
    if failed:
        raise RuntimeError("verification failed: " + ", ".join(failed))


def main() -> None:
    os.chdir(ROOT)
    adapt_driver()
    adapt_binding()
    verify()


if __name__ == "__main__":
    main()

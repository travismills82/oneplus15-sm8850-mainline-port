#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

from __future__ import annotations

import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BINDING = Path("Documentation/devicetree/bindings/ufs/qcom,ufs.yaml")

CANOE_CONDITION = """  - if:
      properties:
        compatible:
          contains:
            const: qcom,canoe-ufshc
    then:
      properties:
        clocks:
          minItems: 9
          maxItems: 9
        clock-names:
          items:
            - const: core_clk
            - const: bus_aggr_clk
            - const: iface_clk
            - const: core_clk_unipro
            - const: core_clk_ice
            - const: ref_clk
            - const: tx_lane0_sync_clk
            - const: rx_lane0_sync_clk
            - const: rx_lane1_sync_clk
        reg:
          minItems: 4
          maxItems: 4
        reg-names:
          minItems: 4
          maxItems: 4
      required:
        - reg-names

"""

OLD_ICE_CONDITION = """  - if:
      required:
        - qcom,ice
    then:
      properties:
        reg:
          maxItems: 1
        clocks:
          minItems: 7
          maxItems: 8
    else:
      properties:
        reg:
          minItems: 1
          maxItems: 2
        clocks:
          minItems: 7
          maxItems: 9
"""

NEW_ICE_CONDITION = """  - if:
      properties:
        compatible:
          contains:
            const: qcom,canoe-ufshc
    then:
      properties:
        reg:
          minItems: 4
          maxItems: 4
        clocks:
          minItems: 9
          maxItems: 9
    else:
      if:
        required:
          - qcom,ice
      then:
        properties:
          reg:
            maxItems: 1
          clocks:
            minItems: 7
            maxItems: 8
      else:
        properties:
          reg:
            minItems: 1
            maxItems: 2
          clocks:
            minItems: 7
            maxItems: 9
"""


def replace_once(text: str, old: str, new: str, description: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{description}: expected one match, found {count}")
    return text.replace(old, new, 1)


def adapt_binding() -> None:
    path = ROOT / BINDING
    text = path.read_text()

    if "qcom,canoe-ufshc" in text:
        raise RuntimeError("Canoe UFS host compatible already exists")

    text = replace_once(
        text,
        "          - qcom,msm8994-ufshc\n",
        "          - qcom,canoe-ufshc\n          - qcom,msm8994-ufshc\n",
        "select compatible insertion",
    )

    # The same msm8994 line now occurs once more in the properties enum.
    text = replace_once(
        text,
        "          - qcom,msm8994-ufshc\n",
        "          - qcom,canoe-ufshc\n          - qcom,msm8994-ufshc\n",
        "properties compatible insertion",
    )

    text = replace_once(
        text,
        "  reg:\n    minItems: 1\n    maxItems: 2\n",
        "  reg:\n    minItems: 1\n    maxItems: 4\n",
        "top-level reg range",
    )

    text = replace_once(
        text,
        "  reg-names:\n    items:\n      - const: std\n      - const: ice\n",
        "  reg-names:\n    items:\n      - const: std\n      - const: ice\n      - const: mcq_sqd\n      - const: mcq_vs\n",
        "MCQ register names",
    )

    text = replace_once(
        text,
        "allOf:\n  - $ref: qcom,ufs-common.yaml\n\n",
        "allOf:\n  - $ref: qcom,ufs-common.yaml\n\n" + CANOE_CONDITION,
        "Canoe clock and register condition",
    )

    text = replace_once(
        text,
        OLD_ICE_CONDITION,
        NEW_ICE_CONDITION,
        "ICE compatibility condition",
    )

    if text.count("qcom,canoe-ufshc") != 4:
        raise RuntimeError("Canoe compatible insertion count is not four")
    if text.count("mcq_sqd") != 1 or text.count("mcq_vs") != 1:
        raise RuntimeError("MCQ register names were not inserted exactly once")

    path.write_text(text)


def verify_driver_support() -> None:
    driver = (ROOT / "drivers/ufs/host/ufs-qcom.c").read_text()
    header = (ROOT / "drivers/ufs/host/ufs-qcom.h").read_text()
    combined = driver + header

    required = (
        "mcq_sqd",
        "mcq_vs",
        "ufs_qcom_mcq_config_resource",
    )
    missing = [token for token in required if token not in combined]
    if missing:
        raise RuntimeError(
            "Linux v7.1 UFS host lacks MCQ support tokens: " + ", ".join(missing)
        )


def main() -> None:
    os.chdir(ROOT)
    adapt_binding()
    verify_driver_support()


if __name__ == "__main__":
    main()

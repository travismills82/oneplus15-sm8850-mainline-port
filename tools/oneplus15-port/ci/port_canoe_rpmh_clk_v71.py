#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DRIVER = ROOT / "drivers/clk/qcom/clk-rpmh.c"
BINDING = ROOT / "Documentation/devicetree/bindings/clock/qcom,rpmhcc.yaml"

CLOCK_BLOCK = """static struct clk_hw *canoe_rpmh_clocks[] = {
	[RPMH_CXO_CLK]		= &clk_rpmh_bi_tcxo_div4.hw,
	[RPMH_CXO_CLK_A]	= &clk_rpmh_bi_tcxo_div4_ao.hw,
	[RPMH_IPA_CLK]		= &clk_rpmh_ipa.hw,
};

static const struct clk_rpmh_desc clk_rpmh_canoe = {
	.clks = canoe_rpmh_clocks,
	.num_clks = ARRAY_SIZE(canoe_rpmh_clocks),
};

"""


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


def main() -> None:
    driver = DRIVER.read_text()
    if "clk_rpmh_canoe" in driver or "qcom,canoe-rpmh-clk" in driver:
        raise RuntimeError("Canoe RPMh clock support already exists")

    anchor = "static struct clk_hw *x1e80100_rpmh_clocks[] = {\n"
    driver = replace_once(driver, anchor, CLOCK_BLOCK + anchor,
                          "Canoe descriptor insertion")

    match_anchor = (
        'static const struct of_device_id clk_rpmh_match_table[] = {\n'
        '\t{ .compatible = "qcom,eliza-rpmh-clk", .data = &clk_rpmh_eliza},\n'
    )
    match_new = (
        'static const struct of_device_id clk_rpmh_match_table[] = {\n'
        '\t{ .compatible = "qcom,canoe-rpmh-clk", .data = &clk_rpmh_canoe},\n'
        '\t{ .compatible = "qcom,eliza-rpmh-clk", .data = &clk_rpmh_eliza},\n'
    )
    driver = replace_once(driver, match_anchor, match_new,
                          "OF match insertion")

    # Linux v7.1's existing /4 ARC clock is equivalent to the preserved
    # xo_pad /2 followed by a fixed-factor /2, without modifying probe logic.
    if "clk_fixed_factor_ops" in CLOCK_BLOCK or "RPMH_CXO_PAD_CLK" in CLOCK_BLOCK:
        raise RuntimeError("minimal Canoe descriptor unexpectedly uses old workaround")
    DRIVER.write_text(driver)

    binding = BINDING.read_text()
    if "qcom,canoe-rpmh-clk" in binding:
        raise RuntimeError("Canoe RPMh compatible already exists in binding")
    binding = replace_once(
        binding,
        "    enum:\n      - qcom,eliza-rpmh-clk\n",
        "    enum:\n      - qcom,canoe-rpmh-clk\n      - qcom,eliza-rpmh-clk\n",
        "binding compatible insertion",
    )
    BINDING.write_text(binding)

    final = DRIVER.read_text()
    checks = (
        "clk_rpmh_bi_tcxo_div4.hw",
        "clk_rpmh_bi_tcxo_div4_ao.hw",
        "clk_rpmh_ipa.hw",
        "qcom,canoe-rpmh-clk",
    )
    missing = [token for token in checks if token not in final + BINDING.read_text()]
    if missing:
        raise RuntimeError("missing expected tokens: " + ", ".join(missing))


if __name__ == "__main__":
    main()

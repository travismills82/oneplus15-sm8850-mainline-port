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

DRIVER = Path("drivers/interconnect/qcom/canoe.c")
HEADER = Path("include/dt-bindings/interconnect/qcom,canoe-rpmh.h")
KCONFIG = Path("drivers/interconnect/qcom/Kconfig")
MAKEFILE = Path("drivers/interconnect/qcom/Makefile")

KCONFIG_BLOCK = """config INTERCONNECT_QCOM_CANOE
\ttristate \"Qualcomm Canoe interconnect driver\"
\tdepends on INTERCONNECT_QCOM_RPMH_POSSIBLE
\tselect INTERCONNECT_QCOM_RPMH
\tselect INTERCONNECT_QCOM_BCM_VOTER
\thelp
\t  This is a driver for the HLOS-voted Qualcomm Network-on-Chip fabrics
\t  on Canoe-based platforms. Camera, display and PCIe CRM voter domains
\t  are not supported.

"""

QNOCOBJ_LINE = "qnoc-canoe-objs\t\t\t:= canoe.o\n"
OBJECT_LINE = "obj-$(CONFIG_INTERCONNECT_QCOM_CANOE) += qnoc-canoe.o\n"

NODE_RE = re.compile(
    r"^static struct qcom_icc_node (?P<name>[A-Za-z0-9_]+) = \{\n"
    r"(?P<body>.*?)"
    r"^\};",
    re.MULTILINE | re.DOTALL,
)


def git_show(path: Path) -> bytes:
    return subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{OLD_REF}:{path.as_posix()}"]
    )


def import_file(path: Path) -> None:
    destination = ROOT / path
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(git_show(path))


def convert_nodes() -> None:
    path = ROOT / DRIVER
    text = path.read_text()
    matches = list(NODE_RE.finditer(text))
    if not matches:
        raise RuntimeError("no qcom_icc_node definitions found")

    id_to_name: dict[str, str] = {}
    node_names: list[str] = []

    for match in matches:
        name = match.group("name")
        body = match.group("body")
        id_match = re.search(r"^\s*\.id = ([A-Z0-9_]+),\n", body, re.MULTILINE)
        if not id_match:
            raise RuntimeError(f"node {name} has no binding ID")
        node_id = id_match.group(1)
        if node_id in id_to_name:
            raise RuntimeError(f"duplicate binding ID {node_id}")
        id_to_name[node_id] = name
        node_names.append(name)

    converted_count = 0
    linked_count = 0

    def rewrite(match: re.Match[str]) -> str:
        nonlocal converted_count, linked_count
        name = match.group("name")
        body = match.group("body")

        body, id_count = re.subn(
            r"^\s*\.id = [A-Z0-9_]+,\n",
            "",
            body,
            count=1,
            flags=re.MULTILINE,
        )
        if id_count != 1:
            raise RuntimeError(f"failed to remove ID from {name}")

        num_match = re.search(r"^\s*\.num_links = (\d+),\n", body, re.MULTILINE)
        if not num_match:
            raise RuntimeError(f"node {name} has no num_links")
        expected_links = int(num_match.group(1))

        links_match = re.search(
            r"^(?P<indent>\s*)\.links = \{(?P<ids>.*?)\},\n",
            body,
            re.MULTILINE | re.DOTALL,
        )

        if expected_links == 0:
            if links_match:
                raise RuntimeError(f"zero-link node {name} unexpectedly has links")
        else:
            if not links_match:
                raise RuntimeError(f"linked node {name} has no links initializer")

            ids = re.findall(r"[A-Z][A-Z0-9_]+", links_match.group("ids"))
            if len(ids) != expected_links:
                raise RuntimeError(
                    f"node {name} declares {expected_links} links but has {len(ids)} IDs"
                )

            unknown = [node_id for node_id in ids if node_id not in id_to_name]
            if unknown:
                raise RuntimeError(
                    f"node {name} has unmapped links: {', '.join(unknown)}"
                )

            pointers = ", ".join(f"&{id_to_name[node_id]}" for node_id in ids)
            replacement = (
                f"{links_match.group('indent')}.link_nodes = {{ {pointers} }},\n"
            )
            body = body[:links_match.start()] + replacement + body[links_match.end():]
            linked_count += expected_links

        converted_count += 1
        return f"static struct qcom_icc_node {name} = {{\n{body}}};"

    text = NODE_RE.sub(rewrite, text)

    if ".id =" in text:
        raise RuntimeError("legacy qcom_icc_node ID initializer remains")
    if ".links =" in text:
        raise RuntimeError("legacy qcom_icc_node links initializer remains")
    if ".remove_new" not in text:
        raise RuntimeError("expected legacy platform remove field was not found")
    text = text.replace("\t.remove_new = qcom_icc_rpmh_remove,\n",
                        "\t.remove = qcom_icc_rpmh_remove,\n", 1)

    marker = "static const struct regmap_config icc_regmap_config = {\n"
    if text.count(marker) != 1:
        raise RuntimeError("could not locate forward-declaration insertion point")

    declarations = "".join(
        f"static struct qcom_icc_node {name};\n" for name in node_names
    ) + "\n"
    text = text.replace(marker, declarations + marker, 1)

    path.write_text(text)
    print(f"converted_nodes={converted_count}")
    print(f"converted_links={linked_count}")


def insert_after_config(text: str, symbol: str, block: str) -> str:
    lines = text.splitlines(keepends=True)
    starts = [
        index for index, line in enumerate(lines)
        if re.fullmatch(rf"config {re.escape(symbol)}\s*", line)
    ]
    if len(starts) != 1:
        raise RuntimeError(f"expected one {symbol} config block")

    end = len(lines)
    for index in range(starts[0] + 1, len(lines)):
        if re.fullmatch(r"config [A-Z0-9_]+\s*", lines[index]):
            end = index
            break
    lines.insert(end, block)
    return "".join(lines)


def adapt_kconfig() -> None:
    path = ROOT / KCONFIG
    text = path.read_text()
    if "config INTERCONNECT_QCOM_CANOE" in text:
        raise RuntimeError("Canoe interconnect Kconfig symbol already exists")
    text = insert_after_config(text, "INTERCONNECT_QCOM_SM8750", KCONFIG_BLOCK)
    if text.count("config INTERCONNECT_QCOM_CANOE") != 1:
        raise RuntimeError("Canoe interconnect Kconfig insertion count is not one")
    path.write_text(text)


def adapt_makefile() -> None:
    path = ROOT / MAKEFILE
    text = path.read_text()
    if "qnoc-canoe" in text or "INTERCONNECT_QCOM_CANOE" in text:
        raise RuntimeError("Canoe interconnect Makefile entries already exist")

    obj_anchor = "qnoc-sm8750-objs\t\t\t:= sm8750.o\n"
    config_anchor = "obj-$(CONFIG_INTERCONNECT_QCOM_SM8750) += qnoc-sm8750.o\n"
    if text.count(obj_anchor) != 1 or text.count(config_anchor) != 1:
        raise RuntimeError("SM8750 interconnect Makefile anchors are missing")

    text = text.replace(obj_anchor, obj_anchor + QNOCOBJ_LINE, 1)
    text = text.replace(config_anchor, config_anchor + OBJECT_LINE, 1)
    path.write_text(text)


def verify() -> None:
    driver = (ROOT / DRIVER).read_text()
    header = (ROOT / HEADER).read_text()
    kconfig = (ROOT / KCONFIG).read_text()
    makefile = (ROOT / MAKEFILE).read_text()

    checks = {
        "driver compatible": 'qcom,canoe-gem-noc' in driver,
        "pointer links": ".link_nodes =" in driver,
        "legacy IDs absent": ".id =" not in driver,
        "legacy links absent": ".links =" not in driver,
        "current remove field": ".remove = qcom_icc_rpmh_remove" in driver,
        "legacy remove field absent": ".remove_new" not in driver,
        "binding master ID": "CANOE_MASTER_UFS_MEM" in header,
        "binding slave ID": "CANOE_SLAVE_LLCC" in header,
        "Kconfig symbol": kconfig.count("config INTERCONNECT_QCOM_CANOE") == 1,
        "Makefile object": makefile.count("INTERCONNECT_QCOM_CANOE") == 1,
    }
    failed = [name for name, passed in checks.items() if not passed]
    if failed:
        raise RuntimeError("verification failed: " + ", ".join(failed))


def main() -> None:
    os.chdir(ROOT)
    import_file(DRIVER)
    import_file(HEADER)
    convert_nodes()
    adapt_kconfig()
    adapt_makefile()
    verify()


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
from __future__ import annotations

import json
import re
from pathlib import Path


TARGETS = {
    "CHANJOB": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 7,
    },
    "DUMMYJOB": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 6,
    },
    "ADVAN": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 8,
    },
    "NUDIRECT": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 4,
    },
    "ENDPRCHG": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 9,
        "exact_override": [
            "RELINT",
            "DXCH 0164",
            "EXTEND",
            "BZMF 2772",
            "DXCH Z",
            "CS A",
            "AD 4753",
            "TS 0164",
            "TCF 6050",
        ],
    },
    "NUCHANG2": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank01_full.dtest.txt",
        "length": 10,
        "exact_override": [
            "INHINT",
            "CCS 0067",
            "TCF 3016",
            "RELINT",
            "TCF 3216",
            "CA 4752",
            "EXTEND",
            "WOR 11",
            "DXCH 0164",
            "TCF 2712",
        ],
    },
    "INTRSM": {
        "source_file": "INTERPRETER.agc",
        "dtest_file": "luminary099_bank03_interp_window.dtest.txt",
        "length": 2,
    },
    "SUPDXCHZ": {
        "source_file": "EXECUTIVE.agc",
        "dtest_file": "luminary099_bank02_full.dtest.txt",
        "length": 5,
    },
}

OPERAND_ALIASES = {
    "ZERO": "4755",
    "TWO": "4752",
    "DSALMOUT": "11",
    "NEWJOB": "0067",
    "LOC": "0164",
    "BANKSET": "0165",
    "SELFRET": "1361",
    "SELFBANK": "3224",
    "NUCHANG2": "3011",
    "NUDIRECT": "3225",
    "SUPDXCHZ": "5165",
    "SUPDXCHZ +1": "5166",
    "BBANK": "BB",
    "INTPRET +3": "6042",
    "ONE": "4753",
}


def clean_source_line(line: str) -> str:
    return line.split("#", 1)[0].rstrip()


def normalize_source_opcode(opcode: str) -> str:
    if opcode == "CAE":
        return "CA"
    if opcode == "CAF":
        return "CA"
    return opcode


def normalize_source_rendered(rendered: str) -> str:
    rendered = re.sub(r"^[+-]\d+\s+", "", rendered).strip()
    if not rendered:
        return rendered
    parts = rendered.split(maxsplit=1)
    opcode = parts[0]
    operand = parts[1] if len(parts) > 1 else ""
    operand = OPERAND_ALIASES.get(operand, operand)
    return f"{opcode} {operand}".strip()


def load_source_sequences(source_dir: Path) -> dict[str, list[str]]:
    source_files = {spec["source_file"] for spec in TARGETS.values()}
    raw_lines_by_file: dict[str, list[str]] = {
        source_file: (source_dir / source_file).read_text(encoding="utf-8").splitlines()
        for source_file in source_files
    }
    index_by_file_and_label: dict[tuple[str, str], int] = {}
    for source_file, raw_lines in raw_lines_by_file.items():
        for index, raw in enumerate(raw_lines):
            stripped = clean_source_line(raw)
            if not stripped.strip():
                continue
            tokens = stripped.split()
            if not raw[:1].isspace() and tokens:
                index_by_file_and_label[(source_file, tokens[0])] = index

    sequences: dict[str, list[str]] = {}
    for label, spec in TARGETS.items():
        source_file = spec["source_file"]
        raw_lines = raw_lines_by_file[source_file]
        label_index = index_by_file_and_label[(source_file, label)]
        wanted = spec["length"]
        collected: list[str] = []
        i = label_index
        while i < len(raw_lines) and len(collected) < wanted:
            stripped = clean_source_line(raw_lines[i])
            i += 1
            if not stripped.strip():
                continue
            tokens = stripped.split()
            if not tokens:
                continue

            if raw_lines[i - 1][:1].isspace():
                opcode = normalize_source_opcode(tokens[0])
                operand = " ".join(tokens[1:])
            else:
                if len(tokens) < 2:
                    continue
                opcode = normalize_source_opcode(tokens[1])
                operand = " ".join(tokens[2:])

            if opcode in {"BLOCK", "BANK", "COUNT", "COUNT*", "EBANK="} or opcode.endswith("="):
                continue

            rendered = f"{opcode} {operand}".strip()
            collected.append(normalize_source_rendered(rendered))
        sequences[label] = collected
    return sequences


def load_dtest_rows(dtest_path: Path) -> list[dict[str, str]]:
    rows = []
    for raw in dtest_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        match = re.match(r"^(?P<addr>\d\d,\d{4})\s+\d+\s+(?P<opcode>\S+)(?:\s+(?P<operand>\S+))?", raw)
        if not match:
            continue
        opcode = match.group("opcode")
        operand = match.group("operand") or ""
        rows.append(
            {
                "addr": match.group("addr"),
                "exact": f"{opcode} {operand}".strip(),
                "opcode": opcode,
                "raw": raw,
            }
        )
    return rows


def find_hits(rows: list[dict[str, str]], sequence: list[str], mode: str) -> list[str]:
    hits: list[str] = []
    for i in range(len(rows) - len(sequence) + 1):
        if mode == "exact":
            candidate = [rows[i + j]["exact"] for j in range(len(sequence))]
        else:
            candidate = [rows[i + j]["opcode"] for j in range(len(sequence))]
        if candidate == sequence:
            hits.append(rows[i]["addr"])
    return hits


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    source_dir = repo_root / "third_party" / "apollo" / "apollo11" / "lm" / "luminary099"
    output_dir = repo_root / "third_party" / "_derived_tools"
    output_dir.mkdir(parents=True, exist_ok=True)

    source_sequences = load_source_sequences(source_dir)
    rows_by_dtest: dict[str, list[dict[str, str]]] = {}
    for spec in TARGETS.values():
        dtest_name = spec["dtest_file"]
        if dtest_name in rows_by_dtest:
            continue
        dtest_path = output_dir / dtest_name
        rows_by_dtest[dtest_name] = load_dtest_rows(dtest_path)

    report = {
        "source_dir": str(source_dir.relative_to(repo_root)).replace("\\", "/"),
        "dtest_files": {
            name: str((output_dir / name).relative_to(repo_root)).replace("\\", "/")
            for name in rows_by_dtest
        },
        "targets": {},
    }

    for label, spec in TARGETS.items():
        dtest_name = spec["dtest_file"]
        rows = rows_by_dtest[dtest_name]
        exact_sequence = spec.get("exact_override", source_sequences[label])
        exact_hits = find_hits(rows, exact_sequence, "exact")
        opcode_hits = find_hits(rows, [entry.split()[0] for entry in exact_sequence], "opcode")
        report["targets"][label] = {
            "source_file": str((source_dir / spec["source_file"]).relative_to(repo_root)).replace("\\", "/"),
            "dtest_file": str((output_dir / dtest_name).relative_to(repo_root)).replace("\\", "/"),
            "source_exact_sequence": exact_sequence,
            "exact_hits": exact_hits,
            "opcode_only_hits": opcode_hits,
        }

    json_path = output_dir / "luminary099_executive_alignment_check.json"
    txt_path = output_dir / "luminary099_executive_alignment_check.txt"
    json_path.write_text(json.dumps(report, indent=2), encoding="utf-8")

    lines = [
        "Luminary 099 Executive alignment check",
        f"Source dir: {report['source_dir']}",
        "",
    ]
    for label in ["CHANJOB", "DUMMYJOB", "ADVAN", "NUDIRECT", "ENDPRCHG", "NUCHANG2", "INTRSM", "SUPDXCHZ"]:
        item = report["targets"][label]
        lines.append(label)
        lines.append(f"  source: {item['source_file']}")
        lines.append(f"  disassembly: {item['dtest_file']}")
        lines.append(f"  source exact sequence: {' | '.join(item['source_exact_sequence'])}")
        lines.append(f"  exact hits: {', '.join(item['exact_hits']) if item['exact_hits'] else 'none'}")
        lines.append(
            f"  opcode-only hits: {', '.join(item['opcode_only_hits']) if item['opcode_only_hits'] else 'none'}"
        )
        lines.append("")
    txt_path.write_text("\n".join(lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

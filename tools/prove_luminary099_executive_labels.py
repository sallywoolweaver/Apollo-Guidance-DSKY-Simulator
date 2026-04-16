#!/usr/bin/env python3
from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


ZERO_WORD_OPCODES = {
    "BANK",
    "BLOCK",
    "COUNT",
    "COUNT*",
    "SETLOC",
}


@dataclass(frozen=True)
class EmittedLine:
    source_line: int
    label: str | None
    opcode: str
    text: str


def strip_comment(line: str) -> str:
    return line.split("#", 1)[0].rstrip()


def parse_emitted_lines(source_path: Path) -> list[EmittedLine]:
    emitted: list[EmittedLine] = []
    for line_number, raw_line in enumerate(source_path.read_text(encoding="utf-8").splitlines(), start=1):
        uncommented = strip_comment(raw_line)
        if not uncommented.strip():
            continue

        leading_stripped = uncommented.lstrip()
        if leading_stripped.startswith("#") or leading_stripped.startswith("##"):
            continue

        tokens = uncommented.split()
        if not tokens:
            continue

        label = None
        opcode = ""
        if raw_line[:1].isspace():
            opcode = tokens[0]
        else:
            label = tokens[0]
            if len(tokens) >= 2:
                opcode = tokens[1]

        if not opcode:
            continue
        if opcode in ZERO_WORD_OPCODES or opcode.endswith("="):
            continue

        emitted.append(
            EmittedLine(
                source_line=line_number,
                label=label,
                opcode=opcode,
                text=raw_line.rstrip(),
            )
        )
    return emitted


def offset_map_from_anchor(
    emitted: Iterable[EmittedLine],
    anchor_label: str,
    anchor_offset_octal: str,
) -> dict[str, dict[str, object]]:
    emitted_list = list(emitted)
    anchor_index = next(index for index, entry in enumerate(emitted_list) if entry.label == anchor_label)
    anchor_offset = int(anchor_offset_octal, 8)

    mapped: dict[str, dict[str, object]] = {}
    for index, entry in enumerate(emitted_list):
        if not entry.label or entry.label.startswith(("+", "-")):
            continue
        offset = anchor_offset + (index - anchor_index)
        mapped[entry.label] = {
            "offset_octal": f"{offset:04o}",
            "source_line": entry.source_line,
            "opcode": entry.opcode,
            "text": entry.text,
        }
    return mapped


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    source_path = repo_root / "third_party" / "apollo" / "apollo11" / "lm" / "luminary099" / "EXECUTIVE.agc"
    output_dir = repo_root / "third_party" / "_derived_tools"
    output_dir.mkdir(parents=True, exist_ok=True)

    emitted = parse_emitted_lines(source_path)
    mapped = offset_map_from_anchor(emitted, anchor_label="SUPDXCHZ", anchor_offset_octal="3165")

    validation_targets = {
        "NOVAC2": "1261",
        "NOVAC3": "1264",
        "CORFOUND": "1272",
        "SPECTEST": "1305",
        "SETLOC": "1315",
        "NEXTCORE": "1330",
        "SUPDXCHZ": "3165",
    }
    validation = {}
    for label, expected in validation_targets.items():
        actual = mapped.get(label, {}).get("offset_octal")
        validation[label] = {
            "expected_octal": expected,
            "actual_octal": actual,
            "matches": actual == expected,
        }

    scheduler_labels = [
        "CHANJOB",
        "ENDPRCHG",
        "JOBSLP1",
        "NUCHANG2",
        "DUMMYJOB",
        "ADVAN",
        "SELFBANK",
        "NUDIRECT",
        "SUPDXCHZ",
    ]

    payload = {
        "source_file": str(source_path.relative_to(repo_root)).replace("\\", "/"),
        "anchor": {
            "label": "SUPDXCHZ",
            "offset_octal": "3165",
        },
        "validation": validation,
        "scheduler_labels": {label: mapped[label] for label in scheduler_labels if label in mapped},
    }

    json_path = output_dir / "luminary099_executive_label_proof.json"
    text_path = output_dir / "luminary099_executive_label_proof.txt"
    json_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")

    lines = [
        "Luminary 099 Executive label proof",
        f"Source: {payload['source_file']}",
        "Anchor: SUPDXCHZ @ 02:3165",
        "",
        "Validation against already-proven exact labels:",
    ]
    for label in validation_targets:
        result = validation[label]
        status = "OK" if result["matches"] else "MISMATCH"
        lines.append(
            f"- {label}: expected 02:{result['expected_octal']} actual 02:{result['actual_octal']} [{status}]"
        )
    lines.append("")
    lines.append("Derived scheduler/job-switch labels:")
    for label in scheduler_labels:
        if label not in mapped:
            continue
        item = mapped[label]
        lines.append(
            f"- {label}: 02:{item['offset_octal']} (source line {item['source_line']}, opcode {item['opcode']})"
        )
    text_path.write_text("\n".join(lines) + "\n", encoding="utf-8")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

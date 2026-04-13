#!/usr/bin/env python3
"""
Simple Apollo AGC source annotation extractor.

This utility is intentionally conservative. It scans AGC source files,
captures file headers, section comments, and nearby comment blocks for labels,
and writes a JSON structure compatible with the Android asset loader.

It is designed for local copies of the Apollo-11 / Virtual AGC source trees.
It does not attempt to prove runtime execution mappings.
"""

from __future__ import annotations

import argparse
import json
import re
from collections import defaultdict
from pathlib import Path

COMMENT_RE = re.compile(r"^\s*#(.*)$")
LABEL_RE = re.compile(r"^(?P<label>[A-Z0-9][A-Z0-9/_-]*)\s+")

PRIORITY_FILES = {
    "ALARM_AND_ABORT.agc": ["alarm", "abort", "lm"],
    "BURN,_BABY,_BURN_--_MASTER_IGNITION_ROUTINE.agc": ["ignition", "burn", "lm"],
    "DISPLAY_INTERFACE_ROUTINES.agc": ["dsky", "display", "interface"],
    "LUNAR_LANDING_GUIDANCE_EQUATIONS.agc": ["guidance", "landing", "lm"],
    "PINBALL_GAME_BUTTONS_AND_LIGHTS.agc": ["dsky", "keyboard", "pinball"],
    "THE_LUNAR_LANDING.agc": ["landing", "p63", "lm"],
}


def parse_agc_file(path: Path) -> tuple[dict, dict]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    file_key = path.as_posix()
    title = path.stem.replace(".agc", "")

    header_comments: list[str] = []
    labels: dict[str, dict] = {}
    pending_comments: list[str] = []
    started_body = False

    for raw_line in lines:
        comment_match = COMMENT_RE.match(raw_line)
        if comment_match:
            comment = comment_match.group(1).rstrip()
            if not started_body:
                header_comments.append(comment)
            pending_comments.append(comment)
            continue

        stripped = raw_line.strip()
        if not stripped:
            if pending_comments:
                pending_comments.append("")
            continue

        started_body = True
        label_match = LABEL_RE.match(raw_line)
        if label_match:
            label = label_match.group("label")
            if label not in labels:
                block = "\n".join(line for line in pending_comments if line is not None).strip()
                labels[label] = {
                    "file": file_key,
                    "title": title,
                    "summary": "",
                    "commentExcerpt": next((line.strip() for line in pending_comments if line.strip()), ""),
                    "commentBlock": block,
                    "tags": PRIORITY_FILES.get(path.name, []),
                }
            pending_comments = []
            continue

        pending_comments = []

    file_info = {
        "title": title,
        "headerComment": "\n".join(line for line in header_comments if line.strip()).strip(),
        "labels": sorted(labels.keys()),
    }
    return file_info, labels


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source_root", type=Path, help="Path to Apollo source tree")
    parser.add_argument("--output", type=Path, required=True, help="JSON output path")
    parser.add_argument("--focus", default="Luminary099", help="Subdirectory to prioritize")
    args = parser.parse_args()

    source_root = args.source_root
    focus_root = source_root / args.focus
    if not focus_root.exists():
        raise SystemExit(f"Focus directory not found: {focus_root}")

    files: dict[str, dict] = {}
    labels: dict[str, dict] = {}
    tags: dict[str, list[str]] = defaultdict(list)

    for path in sorted(focus_root.rglob("*.agc")):
        file_info, file_labels = parse_agc_file(path.relative_to(source_root))
        files[path.relative_to(source_root).as_posix()] = file_info
        labels.update(file_labels)
        for label, info in file_labels.items():
            for tag in info["tags"]:
                tags[tag].append(label)

    payload = {
        "labels": labels,
        "files": files,
        "tags": {tag: sorted(set(values)) for tag, values in sorted(tags.items())},
    }
    args.output.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")


if __name__ == "__main__":
    main()

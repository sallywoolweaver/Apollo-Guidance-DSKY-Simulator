# Apollo Artifact Import Area

This directory is reserved for read-only Apollo source artifacts, listings, binaries, and provenance notes.

Rules:

- Imported Apollo artifacts are treated as historical source material, not project-owned code.
- Do not rewrite, normalize, patch, paraphrase, or "clean up" imported Apollo code here.
- Do not modify imported Apollo artifacts even if they appear messy, imperfect, or inconvenient for the emulator/app.
- If Apollo artifacts have quirks or difficult structure, adapt the emulator/interface/app around them instead.
- Custom bootstrap data, UI assets, and helper metadata do not belong in this directory.
- Executable program images used by the app belong under `app/src/main/assets/programs/`.
- Custom scenario/bootstrap overlays belong under `app/src/main/assets/bootstrap/`.
- Extracted annotation indexes belong under `app/src/main/assets/source_annotations/`.

Current state:

- The repo does not yet vendor full Apollo source listings or full assembled rope images in this directory.
- The immediate purpose of this directory is to make provenance and read-only boundaries explicit while the project transitions away from custom compatibility logic.

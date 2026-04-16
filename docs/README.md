# Apollo Guidance DSKY Simulator Docs

## Current state

- Android shell: Kotlin + Compose
- Native layer: C++ + JNI + NDK/CMake
- Stable JNI API:
  - `initCore()`
  - `loadProgramImage(...)`
  - `resetScenario()`
  - `pressKey(...)`
  - `stepSimulation(...)`
  - `getSnapshot()`

## Data layout

- `third_party/apollo/...`
  - untouched imported historical artifacts
- `third_party/apollo/upstream/...`
  - full upstream git clones used as the authoritative Apollo source base
- `app/src/main/assets/programs/...`
  - runtime program assets
  - Apollo 11 LM now uses `AP11ROPE.binsource`
  - Apollo 11 CM now includes `Comanche055.binsource`
- `app/src/main/assets/bootstrap/...`
  - custom bootstrap/scenario data only
- `app/src/main/assets/source_annotations/...`
  - annotation/index data only

## Runtime truth

- The app is not yet a strict AGC emulator.
- The Apollo 11 LM slice now loads a full Apollo-derived rope image from `AP11ROPE.binsource`.
- The active Luminary 099 key path now routes through exact Apollo labels:
  - `KEYRUPT1`
  - `LODSAMPT`
  - `KEYCOM`
  - `ACCEPTUP`
  - `NOVAC`
  - `NOVAC2`
  - `NOVAC3`
  - `CORFOUND`
  - `SETLOC`
  - `RESUME`
- Apollo relay-row output now owns visible:
  - `PROG`
  - `VERB`
  - `NOUN`
  - register digits/signs
  - `COMP ACTY`
  - `KEY REL`
  - `OPR ERR`
- The native core now includes:
  - bank-local rope/fixed/erasable addressing fixes
  - `TC Q`
  - corrected double-word pairing for `DCA`, `DAS`, and `DXCH`
  - real `WAITLIST` `RESUME` instruction handling on the routed key path
- The remaining blocker is Apollo-owned Executive/core-set/job-switch aftermath after `SETLOC` / `RESUME`.
- The active runtime still depends on compatibility-owned flight, phase-threshold, telemetry, and outcome behavior.

## Important docs

- `APOLLO_IMPORT_AUDIT.md`
- `EXECUTION_PROGRESS_LOG.md`
- `COMPATIBILITYSCENARIO_REDUCTION.md`
- `PROJECT_PURPOSE_AUDIT.md`
- `AUTHENTICITY.md`
- `AUDIT_REPORT.md`
- `SOURCE_MAPPING.md`

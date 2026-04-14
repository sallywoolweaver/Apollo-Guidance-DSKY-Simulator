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
- The active runtime still depends on compatibility-owned entry, flight, phase, and outcome behavior.

## Important docs

- `APOLLO_IMPORT_AUDIT.md`
- `EXECUTION_PROGRESS_LOG.md`
- `COMPATIBILITYSCENARIO_REDUCTION.md`
- `PROJECT_PURPOSE_AUDIT.md`
- `AUTHENTICITY.md`
- `AUDIT_REPORT.md`
- `SOURCE_MAPPING.md`

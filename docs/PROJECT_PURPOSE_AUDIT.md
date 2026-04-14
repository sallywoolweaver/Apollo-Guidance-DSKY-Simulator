# Project Purpose Audit

## Main purpose compliance

Target purpose:

`Keep Apollo code intact and run it through an emulator-oriented native core.`

- Apollo code kept intact and read-only: `PARTIAL`
- Apollo code actually running through an emulator path: `PARTIAL`
- Overall main-purpose compliance: `PARTIAL`

Reason:

- The repo now has a real read-only Apollo import area for Apollo 11 LM `Luminary099`, Apollo 11 CM `Comanche055`, and Apollo 13 LM `LM131R1`.
- The Apollo 11 LM slice now loads full Apollo-derived rope bytes from `AP11ROPE.binsource` through the native rope path.
- The native CPU now owns bank-aware rope/fixed/erasable addressing plus a larger AGC instruction subset.
- The app still does not execute original Apollo software strongly enough to claim true Apollo runtime execution.
- `CompatibilityScenario` still owns flight physics, phase thresholds, and outcomes.

## Apollo artifact integrity

- `third_party/apollo/apollo11/lm/luminary099/...`
  - untouched imported historical tree
- `third_party/apollo/apollo11/cm/comanche055/...`
  - untouched imported historical tree
- `third_party/apollo/apollo13/lm/lm131r1/...`
  - untouched imported historical tree
- `app/src/main/assets/programs/apollo11/lm/luminary099/AP11ROPE.binsource`
  - Apollo-derived runtime rope asset copied from the import tree
- `app/src/main/assets/programs/apollo11/cm/comanche055/Comanche055.binsource`
  - Apollo-derived runtime rope asset copied from the import tree
- `app/src/main/assets/programs/apollo11/lm/luminary099/erasable_init.cfg`
  - custom erasable initializer
- `app/src/main/assets/bootstrap/apollo11/lm/luminary099/bootstrap.cfg`
  - custom bootstrap data
- `app/src/main/assets/source_annotations/source_annotations.json`
  - project-curated annotation/index data

## Runtime truth

- The app is still not a strict AGC emulator.
- The Apollo 11 LM slice now loads a full Apollo-derived rope image.
- The JNI API remains:
  - `initCore()`
  - `loadProgramImage(...)`
  - `resetScenario()`
  - `pressKey(...)`
  - `stepSimulation(...)`
  - `getSnapshot()`
- The native path now owns:
  - full-rope loading for Apollo 11 LM
  - bank-local rope address normalization
  - banked fixed/erasable memory access
  - channel 7 superbank state
  - erasable reset/loading
  - major-mode display state
  - DSKY command parsing
  - DSKY `COMP ACTY`
  - DSKY `EXEC <label>`
  - DSKY `OPR ERR`
  - DSKY `UNSUPPORTED <label>`
  - 1201/1202 alarm ownership
  - DSKY register arbitration
- The native path does not yet own:
  - a historically validated Apollo boot path
  - enough AGC instruction/peripheral semantics to claim faithful Apollo execution
  - guidance
  - phase transitions as flown software behavior
  - mission outcomes

## Required corrective actions

1. Replace the overlay-driven Apollo 11 LM entry point with a truer Apollo boot/entry path.
2. Keep Apollo artifacts read-only and keep runtime/bootstrap/annotation data separate.
3. Continue increasing CPU/banking/peripheral ownership.
4. Remove phase ownership from `CompatibilityScenario`.
5. Remove mission-outcome ownership from `CompatibilityScenario`.

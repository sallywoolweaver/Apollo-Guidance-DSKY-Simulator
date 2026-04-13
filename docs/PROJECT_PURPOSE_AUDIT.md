# Project Purpose Audit

## 1. Main purpose compliance

Target purpose:

`Keep Apollo code intact and run it through an emulator-oriented native core.`

Mandatory rule:

- Apollo source code, listings, and imported Apollo-derived program artifacts must remain untouched.
- The project must not rewrite, normalize, patch, paraphrase, or "improve" Apollo code.
- If Apollo artifacts have quirks or difficult structure, the emulator/app must adapt around them rather than altering them.

- Apollo code kept intact and read-only: `PARTIAL`
- Apollo code actually running through an emulator path: `NOT COMPLETE`
- Overall main-purpose compliance: `PARTIAL`

Reason:

- The repo now separates Apollo-derived rope-word excerpt assets from custom bootstrap and erasable helper data.
- The native core has an emulator-oriented execution path and native DSKY/peripheral ownership for some command behavior.
- The app still does not execute original Apollo software strongly enough to claim Apollo runtime execution.
- `CompatibilityScenario` still owns descent, alarms, mission progression, and outcomes.
- The repo still lacks a full imported read-only Apollo artifact set under `third_party/apollo/`, so compliance is only partial.

## 2. Apollo artifact integrity

- `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - mission/software: Apollo 11 LM / Luminary 099
  - form: Apollo-derived rope-word excerpt
  - intact/read-only: yes within this repo snapshot
  - classification: executable program-data excerpt, not full rope image
- `app/src/main/assets/programs/apollo11/cm/comanche055/rope_excerpt.cfg`
  - mission/software: Apollo 11 CM / Comanche 055
  - form: Apollo-derived rope-word excerpt
  - intact/read-only: yes within this repo snapshot
  - classification: executable program-data excerpt, not full rope image
- `app/src/main/assets/source_annotations/source_annotations.json`
  - mission/software: primarily Apollo 11 LM / Luminary 099
  - form: extracted/curated annotation index
  - intact/read-only: mixed; this is project-curated metadata, not Apollo executable code
  - classification: annotation/helper data
- `third_party/apollo/apollo11/lm/luminary099/PROVENANCE.md`
  - mission/software: Apollo 11 LM / Luminary 099
  - form: project provenance note
  - intact/read-only: project-authored metadata
  - classification: helper/provenance data
- `third_party/apollo/apollo11/cm/comanche055/PROVENANCE.md`
  - mission/software: Apollo 11 CM / Comanche 055
  - form: project provenance note
  - intact/read-only: project-authored metadata
  - classification: helper/provenance data
- `third_party/apollo/apollo13/lm/lm131r1/PROVENANCE.md`
  - mission/software: Apollo 13 LM family
  - form: project provenance note
  - intact/read-only: project-authored metadata
  - classification: helper/provenance data
- `app/src/main/assets/programs/apollo11/lm/luminary099/erasable_init.cfg`
  - mission/software: Apollo 11 LM / Luminary 099
  - form: custom erasable initializer
  - intact/read-only: yes
  - classification: helper data, not Apollo code
- `app/src/main/assets/programs/apollo11/cm/comanche055/erasable_init.cfg`
  - mission/software: Apollo 11 CM / Comanche 055
  - form: custom erasable initializer
  - intact/read-only: yes
  - classification: helper data, not Apollo code
- `app/src/main/assets/bootstrap/apollo11/lm/luminary099/bootstrap.cfg`
  - mission/software: Apollo 11 LM / Luminary 099
  - form: custom scenario/bootstrap overlay
  - intact/read-only: yes
  - classification: helper data, not Apollo code
- `app/src/main/assets/bootstrap/apollo11/cm/comanche055/bootstrap.cfg`
  - mission/software: Apollo 11 CM / Comanche 055
  - form: custom scenario/bootstrap overlay
  - intact/read-only: yes
  - classification: helper data, not Apollo code

## 3. Runtime truth

- The app is not currently running full Apollo code execution.
- The app is currently running a native emulator-oriented shell plus a dominant custom compatibility runtime.
- Apollo-derived rope-word excerpts are fetched by the native CPU path.
- The current CPU path owns:
  - program image loading into rope memory
  - erasable memory initialization/reset
  - major-mode ownership
  - DSKY verb/noun command parsing and some command execution/display mode changes
  - instruction fetch and opcode-class skeleton stepping
  - DSKY `COMP ACTY` annunciation and execution-status text tied to Apollo-derived rope-label execution state
  - native 1201 / 1202 alarm ownership through `AlarmExecutive`
- The current CPU path does not yet own:
  - real AGC instruction semantics broadly enough to drive the scenario
  - alarm generation
  - guidance
  - mission phase transitions as flown software behavior
  - landing outcomes

## 4. Drift from project purpose

The project drifted in these ways:

- It spent substantial effort on an Apollo-facing shell before Apollo execution was real.
- It introduced a companion simulation loop that became the actual owner of mission outcomes.
- It previously used hybrid bundle formats that mixed Apollo-derived rope data with custom scenario/bootstrap data.
- It added source-annotation and museum-style UI before the runtime had honest Apollo execution ownership.
- It risked sounding more source-faithful than it was by leaning on real names, labels, and alarms while custom logic still dominated.

## 5. Required corrective actions

1. Replace rope excerpts with full assembled rope-image loading for at least one software family.
2. Keep Apollo program artifacts read-only and separate from custom bootstrap and annotation assets.
3. Import Apollo source/listing/program artifacts into `third_party/apollo/` as read-only historical material without modifying them.
4. Continue increasing CPU instruction ownership until runtime outputs stop depending primarily on `CompatibilityScenario`.
5. Move DSKY display/update logic further into emulator/peripheral ownership.
6. Move alarm and phase behavior out of `CompatibilityScenario`.
7. Reduce `CompatibilityScenario` to bootstrap and optional derived-view helper duties only.
8. Do not add more gameplay/shell scope until Apollo-derived execution controls meaningful runtime outcomes.

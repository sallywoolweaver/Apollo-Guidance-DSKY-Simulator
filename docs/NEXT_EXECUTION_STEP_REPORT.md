# Next Execution Step Report

## Exact Apollo artifacts imported and their paths

- `third_party/apollo/apollo11/lm/luminary099/rope_excerpt.cfg`
  - copied in unchanged from the app's Apollo-derived Luminary099 rope excerpt asset
  - treated as read-only historical/program material
- `third_party/apollo/apollo11/cm/comanche055/rope_excerpt.cfg`
  - copied in unchanged from the app's Apollo-derived Comanche055 rope excerpt asset
  - treated as read-only historical/program material
- Existing provenance/import notes remain:
  - `third_party/apollo/apollo11/lm/luminary099/PROVENANCE.md`
  - `third_party/apollo/apollo11/cm/comanche055/PROVENANCE.md`
  - `third_party/apollo/apollo13/lm/lm131r1/PROVENANCE.md`

## Whether they are untouched/read-only

- Yes.
- The imported Apollo-derived rope excerpts were copied without modification.
- No Apollo source or Apollo-derived program artifact was edited, normalized, or rewritten in this step.

## Whether a fuller rope-image path now exists

- No.
- The project still does not have a full read-only Apollo 11 LM `Luminary099` rope image imported in-repo.
- The current runtime still uses a limited Apollo-derived rope excerpt rather than a full assembled rope image.

## What runtime behavior was moved out of CompatibilityScenario

- 1201 / 1202 alarm ownership was moved out of `CompatibilityScenario`.
- Alarm generation now lives in the native `AlarmExecutive` layer.
- `AlarmExecutive` is updated from native CPU/peripheral state rather than from `CompatibilityScenario`.
- The current alarm trigger inputs are:
  - native CPU unsupported-opcode count
  - current native major mode
  - current DSKY display mode

Visible result:

- 1201 / 1202 alarm display, `PROG` light, alarm acknowledgement gating, and alarm status text are no longer emitted by `CompatibilityScenario`.

## What still remains in CompatibilityScenario

- descent kinematics
- throttle trim effects
- mission phase thresholds
- landing outcome classification
- most register payload values
- most phase/event text outside native execution/alarm status overrides

## Whether the app can honestly claim more Apollo-derived execution than before

- Yes, but only narrowly.
- The app can now honestly claim that alarm behavior is more emulator/peripheral-driven than before because 1201 / 1202 ownership no longer comes from `CompatibilityScenario`.
- It still cannot honestly claim full Apollo alarm execution, because the current `AlarmExecutive` is driven by native CPU unsupported-opcode pressure rather than true AGC EXECUTIVE/WAITLIST behavior from a full Luminary099 rope image.

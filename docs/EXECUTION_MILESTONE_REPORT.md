# Execution Milestone Report

## Chosen behavior path

- DSKY execution status and `COMP ACTY` annunciation for the Apollo 11 LM `Luminary099` slice.

This milestone is narrow on purpose. It does not claim full AGC or full DSKY emulation. It claims one smaller thing:

- one visible DSKY behavior path is now driven by native execution over Apollo-derived rope data rather than by `CompatibilityScenario`.

## Exact Apollo-derived artifacts used

- `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - read-only Apollo-derived rope-word excerpt
  - includes read-only labels such as `KEYRUPT1`, `Q_EXCHANGE`, `KEYCOM`, `CHARIN_2CADR`

## Read-only status and repo locations

- Apollo-derived program artifact:
  - `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - treated as read-only program data
- Custom helper artifacts:
  - `app/src/main/assets/programs/apollo11/lm/luminary099/erasable_init.cfg`
  - `app/src/main/assets/bootstrap/apollo11/lm/luminary099/bootstrap.cfg`
- Provenance/import area:
  - `third_party/apollo/apollo11/lm/luminary099/PROVENANCE.md`

## What part of runtime behavior is now emulator-driven

- The native CPU now tracks the currently executing Apollo-derived rope label at the current program counter.
- The DSKY `COMP ACTY` annunciator is now synchronized from native CPU execution state rather than left as a default/fallback UI condition.
- The DSKY status line now shows execution-state text such as `EXEC KEYRUPT1` when the CPU is stepping over Apollo-derived labeled rope words.
- The DSKY `OPR ERR` annunciator is now synchronized from native CPU detection of unsupported opcode execution in Apollo-derived rope words.
- The DSKY status line now switches to `UNSUPPORTED <label>` when the current Apollo-derived rope word falls into an unsupported opcode class.

This path is driven by:

- Apollo-derived rope-word loading
- native program-counter advancement
- native rope-label lookup
- native peripheral synchronization into `DskyIo`

It is not driven by `CompatibilityScenario`.

## What still remains in CompatibilityScenario

- descent kinematics
- throttle trim effects
- overload alarm synthesis
- mission phase thresholds
- landing outcome classification
- most register payload values
- most operator-facing phase/event text outside the execution-status override

## Why this milestone is real and not just more scaffolding

- A visible DSKY behavior now changes because the native core is stepping over Apollo-derived program words and labels.
- That visible behavior is no longer authored by `CompatibilityScenario`.
- The milestone is narrow, but it is observable and execution-owned.

## Current limitation

- This is still not original Apollo software execution in the full sense required by the project.
- The CPU path is still only a partial instruction implementation over a limited Luminary099 excerpt.
- The project still does not have a full read-only Luminary099 rope-image import path in-repo.
- Mission outcomes are still not Apollo-driven.

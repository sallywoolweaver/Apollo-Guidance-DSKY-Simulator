# Execution Progress Log

## 2026-04-13 - Apollo 11 LM full-rope load replaces excerpt-only rope load

- What changed:
  - Imported untouched `Luminary099`, `Comanche055`, and `LM131R1` trees into `third_party/apollo/...`.
  - Added `AP11ROPE.binsource` to the Apollo 11 LM runtime program assets.
  - Changed `ProgramAssetLoader` to convert `AP11ROPE.binsource` into raw rope bytes and package them into `loadProgramImage(...)` without changing the JNI API.
  - Added native support for rope metadata and rope-label sidecar sections.
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- What visible runtime behavior became more emulator-driven:
  - the active Apollo 11 LM slice now loads a full Apollo-derived rope image rather than a rope excerpt
  - native stepping now runs against full-rope memory loaded from the Apollo binsource artifact
- What still remains compatibility-driven:
  - entry-point selection is still overlay-driven
  - descent physics
  - phase thresholds
  - landing outcomes
  - most telemetry values

## 2026-04-13 - DSKY register arbitration moved out of `CompatibilityScenario`

- What changed:
  - moved landing monitor / phase summary / last-alarm register arbitration into `DskyIo`
  - `CompatibilityScenario` now publishes telemetry to `DskyIo` instead of directly formatting register output
- Apollo artifact used:
  - no new Apollo artifact; runtime ownership reduction only
- What visible runtime behavior became more emulator-driven:
  - DSKY display register selection is now owned by the DSKY-side native subsystem rather than `CompatibilityScenario`
- What still remains compatibility-driven:
  - the actual telemetry values being displayed are still compatibility-generated

## 2026-04-14 - bank-local rope addressing and AGC opcode ownership expanded

- What changed:
  - normalized full-rope bank offsets so Apollo-derived rope words and label overlays resolve to the same bank-local addresses at runtime
  - added banked erasable reads/writes plus fixed-bank selection driven by `EB`, `FB`, `BB`, and output channel 7 superbank state
  - corrected the native decode table to the real AGC instruction families for `INDEX`, `DXCH`, `TS`, `XCH`, `AD`, and `MASK`
  - added more AGC instruction/channel coverage including `READ`, `WRITE`, `RAND`, `WAND`, `ROR`, `WOR`, `RXOR`, `BZF`, `BZMF`, `QXCH`, `DCA`, and `SU`
- Apollo artifact used:
  - `third_party/apollo/upstream/virtualagc/yaAGC/agc_engine.c`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
- What visible runtime behavior became more emulator-driven:
  - the active Luminary 099 slice now steps with bank-aware rope fetches instead of mixing full AGC addresses and bank-local offsets
  - unsupported-opcode reporting is less polluted by previously mis-decoded core AGC instruction families
  - channel 7 superbank state now belongs to the native CPU path rather than being absent from execution
- What still remains compatibility-driven:
  - Apollo startup still depends on a custom overlay-selected entry slice
  - telemetry values are still not sourced from real AGC channel/display output
  - descent physics
  - phase thresholds
  - mission outcomes
  - most telemetry values

## 2026-04-14 - DSKY relay rows and channel-11 annunciators now drive visible display state

- What changed:
  - added native latching of channel-10 DSKY relay writes so relay rows survive the `OUT0` pulse/reset pattern
  - decoded Apollo DSKY relay rows into visible `PROG`, `VERB`, `NOUN`, and register digit strings inside `DskyIo`
  - changed `DskyIo` annunciator sync to read channel 11 bits for `COMP ACTY`, `KEY REL`, and `OPR ERR`, while keeping old fallbacks only where Apollo output is still absent
  - changed `DskyIo` display-mode selection to follow Apollo-driven `VERB/NOUN` pairs when relay output is present instead of relying only on the local command-state path
  - stopped applying local `V16/V06/V05` display consequences when Apollo relay output is already present
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/INPUT_OUTPUT_CHANNEL_BIT_DESCRIPTIONS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/T4RUPT_PROGRAM.agc`
  - `third_party/apollo/upstream/virtualagc/yaDSKY/src/callbacks.c`
- What visible runtime behavior became more emulator-driven:
  - visible `PROG`, `VERB`, and `NOUN` digits can now come from Apollo relay traffic on channel 10
  - visible register digits can now come from Apollo relay traffic on channel 10
  - `COMP ACTY`, `KEY REL`, and `OPR ERR` can now come from Apollo channel-11 output rather than only from custom `DskyIo` state
  - the landing-monitor / phase-summary / last-alarm display mode can now follow Apollo-driven `VERB/NOUN` output when present
- What still remains compatibility-driven:
  - keyboard command parsing and local entry buffering
  - local command consequences when Apollo display output is absent
  - alarm popup payload formatting
  - phase labels
  - telemetry source values
  - phase/program transition thresholds
  - mission outcomes

## 2026-04-14 - DSKY key input now enters Apollo-facing channels first

- What changed:
  - changed `DskyIo` key handling so DSKY keys are injected into Apollo-facing input channels instead of being only local UI state
  - mapped DSKY keypad / verb / noun / clear / enter / reset / key-release buttons into channel 15 keycodes using the same codes used by `yaDSKY`
  - mapped `PRO` into a low-polarity pulse on channel 32 bit 14 instead of only a local alarm-acknowledgement action
  - kept local verb/noun buffering and command parsing only as fallback when Apollo display ownership is still absent
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/INPUT_OUTPUT_CHANNEL_BIT_DESCRIPTIONS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/T4RUPT_PROGRAM.agc`
  - `third_party/apollo/upstream/virtualagc/yaDSKY/src/callbacks.c`
- What visible runtime behavior became more emulator-driven:
  - DSKY keypresses now reach Apollo-facing input channels even when local fallback behavior still exists
  - `PRO` can now be seen as an Apollo-style proceed input pulse rather than only a local UI action
- What still remains compatibility-driven:
  - keyboard interrupts and full Pinball command consequences are still not Apollo-owned
  - local verb/noun buffering still exists as fallback
  - local command consequences still exist when Apollo display output is absent

## 2026-04-14 - narrow phase/program transition ownership now accepts Apollo-driven program display

- What changed:
  - added a narrow transition hook so `CompatibilityScenario` adopts `P64` or `P66` when the Apollo-driven DSKY program display changes to those values
  - kept the old altitude-threshold transitions only as fallback when Apollo display ownership is not driving the program number
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/THE_LUNAR_LANDING.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
- What visible runtime behavior became more emulator-driven:
  - displayed `P64` and `P66` can now pull scenario phase ownership along with Apollo-driven DSKY program output instead of only following compatibility altitude thresholds
- What still remains compatibility-driven:
  - the actual guidance/mission conditions that cause the Apollo software to change phase are still not fully emulated
  - fallback altitude-threshold transitions still exist
  - mission outcomes and telemetry remain compatibility-owned

## Preserved earlier gains

- native CPU rope-label execution tracking
- DSKY `COMP ACTY`
- DSKY `EXEC <label>`
- DSKY `OPR ERR`
- DSKY `UNSUPPORTED <label>`
- 1201/1202 alarm ownership in `AlarmExecutive`
- stable JNI API

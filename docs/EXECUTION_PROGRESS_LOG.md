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

## 2026-04-14 - DSKY key input is now a momentary Apollo-facing channel pulse

- What changed:
  - changed channel-15 key injection so keypad / verb / noun / clear / enter / reset / key-release inputs are released back to zero after the next native step instead of remaining latched
  - stopped emitting local `KEY REL` / `RSET` compatibility events when Apollo relay output is already owning the visible DSKY
  - kept the older local `KEY REL` / `RSET` consequences only as fallback when Apollo display ownership is still absent
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/INPUT_OUTPUT_CHANNEL_BIT_DESCRIPTIONS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/T4RUPT_PROGRAM.agc`
  - `third_party/apollo/upstream/virtualagc/yaDSKY/src/callbacks.c`
- What visible runtime behavior became more emulator-driven:
  - DSKY keypresses now behave more like Apollo-facing momentary input pulses instead of a sticky local channel value
  - `KEY REL` and `RSET` stop taking compatibility-side action as soon as Apollo relay output is already owning the visible DSKY
- What still remains compatibility-driven:
  - Apollo input interrupts and full Pinball command consequences are still not implemented
  - local verb/noun buffering still exists as fallback when Apollo display ownership is absent
  - local alarm acknowledgement and reset consequences still exist as fallback when Apollo/peripheral ownership is absent

## 2026-04-14 - `TC Q` now returns through the native CPU path

- What changed:
  - fixed `TC Q` handling in the native CPU so Apollo subroutine returns use the `Q` register as a return address instead of incorrectly treating register `Q` as erasable address `2`
  - preserved the earlier `QXCH`/channel/input work and kept the JNI surface unchanged
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
- What visible runtime behavior became more emulator-driven:
  - no new user-visible DSKY behavior changed immediately in this pass
  - Apollo routines reached through the native path can now return more honestly, which removes a real blocker for Pinball/input consequence work
- What still remains compatibility-driven:
  - keypress consequence ownership is still incomplete because the emulator still lacks enough interrupt/executive behavior to drive `CHARIN` / `PROCKEY` end-to-end
  - fallback DSKY command buffering and phase logic still remain

## 2026-04-14 - exact Luminary 099 `CHARIN` / `PROCKEY` entry points now route DSKY input into Apollo code

- What changed:
  - derived exact Luminary 099 bank/offset entry points for `CHARIN`, `PROCKEY`, and nearby Pinball labels from the untouched `AP11ROPE.binsource` plus the untouched `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - added those exact bank-local labels to the derived runtime rope-label overlay in `app/src/main/assets/programs/...`, without modifying any Apollo artifact
  - added a native core path that writes the keycode into `MPAC` and jumps directly to Apollo `CHARIN` for channel-15 DSKY key input
  - added a native core path that jumps directly to Apollo `PROCKEY` for the `PRO` input pulse
  - changed `DskyIo` so the Luminary 099 playable path no longer falls back to local verb/noun/key consequence handling when those Apollo entry points are available
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/T4RUPT_PROGRAM.agc`
- What visible runtime behavior became more emulator-driven:
  - Luminary 099 DSKY keypresses now enter exact Apollo `CHARIN` / `PROCKEY` routine addresses through the native core instead of taking the old local `DskyIo` consequence path
  - the status/execution path can now truthfully report exact Pinball labels such as `EXEC CHARIN` and `EXEC PROCKEY` when those entry points are reached
- What still remains compatibility-driven:
  - this is still direct routine entry, not a full Apollo-owned `KEYRUPT1` / `T4RUPT` interrupt path
  - full Pinball consequence flow still depends on missing interrupt/executive/peripheral behavior after entry
  - phase thresholds, telemetry values, and mission outcomes remain compatibility-owned

## 2026-04-14 - exact routine/source mapping now reaches the runtime debug path

- What changed:
  - expanded the source-annotation layer into a routine/address/source map for exact Luminary 099 DSKY/input labels
  - added exact bank-local bank/offset/source-section metadata for `CHARIN`, `PROCKEY`, `VBRELDSP`, `KEYRUPT1`, and related input/display labels
  - added current native execution label and bank-local PC to `getSnapshot()` so the source/debug UI can resolve live Apollo execution locations
  - updated the source browser / engineer panel to show exact Apollo bank-local address and source-section metadata where that mapping exists
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/TAGS_FOR_RELATIVE_SETLOC_AND_BLANK_BANK_CARDS.agc`
- What visible runtime behavior became more emulator-driven:
  - the engineer/source path can now identify several exact live Apollo labels and addresses rather than only showing broad historical notes
  - `EXEC <label>` debugging is now backed by an exact label -> bank/address -> source-section map for the active DSKY/input routines covered in this pass
- What still remains compatibility-driven:
  - the source browser is still a limited execution/debugging aid, not a complete Apollo source browser
  - labels outside the mapped DSKY/input set still do not have full exact source coverage
  - full Pinball consequence ownership still depends on missing interrupt/executive/peripheral behavior

## 2026-04-15 - channel-15 keys now enter a partial exact `KEYRUPT1` lead-in before `CHARIN`

- What changed:
  - added native single-instruction stepping so the core can walk through short exact Apollo lead-in sequences instead of only jumping directly to a target label
  - changed channel-15 DSKY key routing to start at exact `KEYRUPT1`, preserve the `KEYCOM` / `ACCEPTUP` lead-in, and then hand off into exact `CHARIN`
  - kept the earlier `PROCKEY` exact-entry path intact for `PRO`
  - kept the JNI surface unchanged
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
- What visible runtime behavior became more emulator-driven:
  - a DSKY keypress on the active Luminary 099 path now exercises exact Apollo interrupt-side labels such as `KEYRUPT1`, `KEYCOM`, and `ACCEPTUP` before entering exact `CHARIN`
  - `EXEC <label>` can now truthfully surface more of the interrupt-side input path instead of only the direct Pinball entry label
- What still remains compatibility-driven:
  - this is still not a full Apollo-owned `KEYRUPT1` interrupt path because the core still skips `LODSAMPT`
  - the handoff from `ACCEPTUP` into `CHARIN` is still native rather than a complete Apollo `NOVAC` / `2CADR` executive request path
  - full Pinball consequence flow, display timing, and most input aftermath still depend on missing interrupt/executive/peripheral behavior

## 2026-04-15 - key input now reaches exact `LODSAMPT` and exact `NOVAC` request capture before `CHARIN`

- What changed:
  - preserved the earlier `KEYRUPT1` / `KEYCOM` / `ACCEPTUP` lead-in
  - stopped skipping `LODSAMPT`; the routed key path now enters exact `LODSAMPT` before returning to the key interrupt flow
  - added exact Luminary 099 labels for `LODSAMPT`, `NOVAC`, `NOVAC2`, and the derived `NOVAC_NEWLOC` request-capture site
  - changed the post-key path so it no longer jumps from `ACCEPTUP` straight into `CHARIN`
  - the native core now waits for exact `TC NOVAC`, lets Apollo `INDEX Q / DCA 0 / DXCH NEWLOC` capture the exact `2CADR CHARIN` request words, and only then dispatches to exact `CHARIN`
  - set the routed switched-bank context explicitly so Apollo `TC Q` and `INDEX Q` behavior can use the correct request bank on this narrow path
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
- What visible runtime behavior became more emulator-driven:
  - the active key-input path now includes real Apollo request capture at `NOVAC / DXCH NEWLOC` instead of a direct native `ACCEPTUP -> CHARIN` handoff
  - the execution-status path can now truthfully surface `LODSAMPT` and `NOVAC` labels on this routed input path
- What still remains compatibility-driven:
  - this is still not full Apollo `NOVAC2` / Executive job-entry behavior
  - the final dispatch after `NOVAC / DXCH NEWLOC` is still a narrow native dispatcher keyed by the exact captured request site
  - full Pinball consequence flow, display timing, and most input aftermath still depend on missing interrupt/executive/peripheral behavior

## 2026-04-15 - the routed key path now reaches `NOVAC2` / `SETLOC` before the remaining native dispatch

- What changed:
  - kept the exact `KEYRUPT1 -> LODSAMPT -> KEYCOM -> ACCEPTUP -> NOVAC` path intact
  - changed the `NOVAC` hook so it now only captures the exact `2CADR` request label instead of dispatching immediately to `CHARIN`
  - moved the remaining native dispatch later to the Apollo-side `RESUME` edge, so the routed path now continues through more of the real Executive aftermath first
  - seeded the active Luminary 099 erasable image with the narrow Executive fresh-start words derived from `FRESH_START_AND_RESTART.agc`:
    - all eight `PRIORITY` slots set to `-0`
    - `NEWJOB` set to `-0`
    - `VAC1USE` through `VAC5USE` set to the exact fresh-start available-VAC addresses
  - corrected the native `CCS` behavior used by Executive availability scanning so `NOVAC2` branch behavior matches the real Apollo sign/zero cases more closely
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/FRESH_START_AND_RESTART.agc`
  - `third_party/apollo/apollo11/lm/luminary099/ERASABLE_ASSIGNMENTS.agc`
- What visible runtime behavior became more emulator-driven:
  - no new on-device-visible DSKY consequence is claimed in this pass because device verification was unavailable
  - the routed key-input path now provably reaches more of the real Apollo Executive aftermath before the remaining dispatch
- What still remains compatibility-driven:
  - the routed path still uses a narrow native dispatch after the Apollo-side `RESUME` edge rather than a full Executive scheduler/job switch
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-15 - DSKY relay-row decoding was corrected against `yaDSKY`

- What changed:
  - audited the channel-10 relay-row decoding against the imported `yaDSKY` implementation from Virtual AGC
  - corrected the register decoding so:
    - `R1` uses row `8` right digit, row `7` two digits plus sign, and row `6` two digits plus minus sign
    - `R2` uses rows `5`, `4`, and the left digit of row `3`
    - `R3` uses the right digit of row `3`, then rows `2` and `1`
  - corrected sign precedence so `+` wins over `-` when both relay bits are present, matching `yaDSKY`
- Apollo artifact used:
  - `third_party/apollo/upstream/virtualagc/yaDSKY/src/callbacks.c`
- What visible runtime behavior became more emulator-driven:
  - Apollo-owned relay output now maps to more correct visible DSKY register digits and signs on the Android shell
- What still remains compatibility-driven:
  - the app still falls back to compatibility-driven register payloads whenever Apollo relay output is absent
  - a correct relay decode does not by itself mean the launch display is fully Apollo-owned end-to-end

## 2026-04-15 - the routed key path now reaches Apollo `WAITLIST` `RESUME` preparation before dispatch

- What changed:
  - kept the existing exact `KEYRUPT1 -> LODSAMPT -> KEYCOM -> ACCEPTUP -> NOVAC -> NOVAC2 -> NOVAC3 -> CORFOUND -> SETLOC` path intact
  - added exact `WAITLIST` labels for `RESUME`, `NOQRSM`, and `NOQBRSM`
  - moved the remaining native dispatch later again:
    - it no longer fires at the `TC RESUME` call site in `KEYRUPT`
    - it now waits until the routed path has returned through the exact Apollo `WAITLIST` `RESUME` entry and reached the final Apollo `RESUME` opcode boundary
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
- What visible runtime behavior became more emulator-driven:
  - no newly confirmed visible DSKY consequence is claimed from this pass alone
  - the routed key path now provably includes more exact Apollo interrupt-return preparation before the remaining dispatch
- What still remains compatibility-driven:
  - the final dispatch still occurs at the Apollo `RESUME` opcode boundary rather than through a full AGC interrupt-return / scheduler implementation
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-15 - the routed key path now crosses real `WAITLIST RESUME` semantics before the remaining fallback

- What changed:
  - corrected native double-word instruction pairing so Apollo word pairs now use `K` and `K+1` instead of the earlier incorrect reverse pairing
  - that correction now applies to:
    - `DCA`
    - `DAS`
    - `DXCH`
  - `DXCH Z` and `DXCH FBANK` now honor the control-transfer side effects expected by Apollo `DTCB` / `DTCF`-style usage because the paired write now updates `Z`
  - added native handling for the exact Apollo `RESUME` special instruction word at `WAITLIST`
  - before direct `KEYRUPT1` entry, the routed key path now primes a more honest interrupt lead-in state:
    - `ARUPT` seeded from the interrupted `A`
    - `LRUPT` seeded from the interrupted `L`
    - `BRUPT` seeded from the interrupted bank-local return address
    - `A` seeded with the interrupted `BBANK` content expected by the `KEYRUPT1` lead-in
  - removed the old remaining dispatch at the final `WAITLIST RESUME` opcode boundary
  - the routed key path now keeps stepping after real Apollo `RESUME` instead of dispatching exactly there
  - the remaining fallback dispatch, if still needed, only happens after a larger post-`RESUME` Apollo execution budget is exhausted
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/INTERRUPT_LEAD_INS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
  - `third_party/apollo/apollo11/lm/luminary099/ERASABLE_ASSIGNMENTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
- What visible runtime behavior became more emulator-driven:
  - no new device-confirmed DSKY digit or program consequence is claimed from this pass alone
  - the active key-input path now provably uses more real Apollo CPU semantics and more real Apollo interrupt-return aftermath before the remaining fallback can occur
  - `EXEC <label>` can now honestly reach beyond the old `WAITLIST RESUME` stop point when the routed path continues after the special `RESUME` instruction
- What still remains compatibility-driven:
  - the routed path still retains a narrow final fallback dispatch if the requested job has not become self-dispatching after the extended Apollo aftermath window
  - full Apollo Executive scheduling, core-set switching, and job ownership after `SETLOC` are still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## Preserved earlier gains

- native CPU rope-label execution tracking
- DSKY `COMP ACTY`
- DSKY `EXEC <label>`
- DSKY `OPR ERR`
- DSKY `UNSUPPORTED <label>`
- 1201/1202 alarm ownership in `AlarmExecutive`
- stable JNI API

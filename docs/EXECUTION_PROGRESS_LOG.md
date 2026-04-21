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

## 2026-04-16 - launch-state instrumentation was added and the routed post-`RESUME` Apollo window was extended

- What changed:
  - added lightweight launch/lifecycle instrumentation in `MainActivity`
  - added lightweight app-state transition logging in `ApolloSimViewModel`
  - verified launch state with clean `logcat`, `am start -W`, PID, and `dumpsys activity activities`
  - increased the routed `KEYRUPT1` execution budget from `384` to `768` instructions before the remaining fallback dispatch is allowed
- Apollo artifact used:
  - the active routed path preserved exact Apollo labels through:
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
- What visible runtime behavior became more emulator-driven:
  - no new device-confirmed DSKY digit/program consequence is claimed from the longer post-`RESUME` window alone
  - the remaining fallback dispatch now occurs later, after a larger block of real Apollo post-`RESUME` execution
  - launch-state verification is now grounded in app-side lifecycle/screen logs rather than only external adb guesses
- What still remains compatibility-driven:
  - the routed key path still has a later fallback dispatch if the requested job still has not become self-dispatching
  - full Apollo Executive scheduling, core-set switching, and job ownership after `SETLOC` remain incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-16 - the remaining request fallback now dispatches from captured `NEWLOC` `2CADR` state instead of a hard-coded `CHARIN` jump

- What changed:
  - preserved the existing exact routed path through:
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
  - replaced the old hard-coded native `CHARIN_2CADR -> CHARIN` fallback with a generic dispatch built from the exact `NEWLOC` / `NEWLOC+1` words captured by Apollo `NOVAC`
  - the native core now decodes the captured `2CADR` target from Apollo-owned Executive state, restores the bank/superbank word to channel `7` plus `BBANK`-driven fixed-bank state, and dispatches to the decoded target rather than to a single hard-coded label
  - after that late dispatch, the core now keeps stepping for another bounded execution window instead of treating the dispatch itself as the end of routed Apollo work
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
- What visible runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the remaining request fallback is now less fake because it is driven by Apollo-captured `2CADR` request state rather than a hard-coded `CHARIN` destination
  - the routed key path now continues deeper after the late dispatch, so more post-request interaction flow can come from Apollo-executed code before control returns to the normal simulation loop
- What still remains compatibility-driven:
  - the routed key path still uses a late emulator-side dispatch primitive rather than a complete Apollo Executive scheduler/core-set/job-switch implementation
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-16 - the late `2CADR` dispatch now enters exact Apollo `SUPDXCHZ`

- What changed:
  - preserved the existing exact routed path through:
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
  - derived exact Apollo `SUPDXCHZ` at bank `02`, offset `3165`, from `EXECUTIVE.agc` and the Luminary 099 rope image
  - replaced the remaining direct emulator jump-to-target step with:
    - load Apollo-captured `NEWLOC` into `A`
    - load Apollo-captured `NEWLOC+1` into `L`
    - enter exact Apollo `SUPDXCHZ`
  - this lets Apollo own the bank/superbank transfer and `TC L` control transfer for the late dispatch instead of the emulator writing those effects directly
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the routed key path now provably exercises more exact Apollo Executive transfer behavior after the captured request state is handed off
  - the remaining late dispatch primitive is smaller because Apollo `SUPDXCHZ` now owns the actual bank/superbank/job-target transfer
- What still remains compatibility-driven:
  - the emulator still decides when to trigger the late request dispatch instead of a full Apollo scheduler/job-switch mechanism deciding it end-to-end
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-16 - the remaining late dispatch trigger is now tied to exact Apollo `RESUME` rather than only a flat key-entry timer

- What changed:
  - preserved the exact routed path through:
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
    - `SUPDXCHZ`
  - changed the remaining late request trigger so it now prefers an Apollo boundary:
    - wait until the exact Apollo `RESUME` special instruction has executed
    - then allow only a bounded post-`RESUME` Apollo execution window before the remaining late request handoff
  - increased the total routed instruction safety budget from `768` to `1024` so the route still has room to reach the exact `RESUME` boundary and then consume the bounded post-`RESUME` window
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the remaining late handoff is now less arbitrary because it is anchored to exact Apollo `RESUME` completion rather than only to a flat instruction count from key-entry
  - more of the keypress aftermath before the remaining handoff is now provably Apollo-owned because the trigger waits for the exact routed interrupt-return boundary first
- What still remains compatibility-driven:
  - the emulator still owns the final decision to hand the pending request into `SUPDXCHZ` after the bounded post-`RESUME` window
  - the Apollo-owned replacement target for that remaining trigger is still the deeper Executive scheduler/job-switch path around `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB`
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-16 - post-`SUPDXCHZ` continuation now waits for the Apollo-requested target instead of using only a flat post-dispatch window

- What changed:
  - preserved the exact routed path through:
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
    - `SUPDXCHZ`
  - after the late `SUPDXCHZ` handoff, the native core now waits for the Apollo-requested target bank/offset or label to become active
  - only after that exact requested target is reached does the core allow a smaller bounded post-target execution window
  - this reduces the old flat post-dispatch timer by making the continuation boundary depend on Apollo-requested target activation
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the routed path now provably waits for the Apollo-requested job target to become active after `SUPDXCHZ` instead of treating the transfer itself as the only meaningful boundary
  - this makes more of the post-dispatch path Apollo-driven before the remaining emulator-side trigger can end the routed flow
- What still remains compatibility-driven:
  - the emulator still decides when to invoke the late `SUPDXCHZ` handoff after the bounded post-`RESUME` window
  - the Apollo-owned replacement target for that remaining trigger is still the deeper Executive scheduler/job-switch path around `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB`
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-16 - launcher icon integration was completed, but exact scheduler-label derivation is still blocked

- What changed:
  - wired the provided launcher logo into the Android resource path:
    - `app/src/main/res/drawable/ic_launcher_logo.png`
    - adaptive launcher XMLs now point at `@drawable/ic_launcher_logo`
    - legacy copied PNGs were removed from `mipmap-anydpi` so the Android resource merger can build cleanly
  - verified the app still assembles, installs, launches, and keeps a live PID on device after the launcher-icon change
  - investigated the next Executive reduction target around:
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
    - `CHANJOB`
  - confirmed that the current bank-02 derived disassembly file does not line up cleanly enough with the imported `EXECUTIVE.agc` source block before `SUPDXCHZ` to stamp those labels as exact runtime addresses honestly
  - attempted to build a local `yaYUL` binary to generate a stricter Luminary 099 listing, but the checked-in Windows build path failed on GNU-style statement-expression macros before a listing could be produced
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/yaYUL/*`
- What visible/runtime behavior became more emulator-driven:
  - no new scheduler/job-switch behavior is being claimed from this pass
  - the launcher icon now uses the provided Apollo LM/Earth logo and still builds into the app cleanly
  - device launch/install stability remained intact after the launcher-icon change
- What still remains compatibility-driven:
- the remaining late `SUPDXCHZ` invocation trigger still exists
- the exact Apollo-owned replacement target is still the deeper scheduler/job-switch path around `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB`
- local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
- phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - the derived Luminary 099 listing build path now produces a local `yaYUL.exe`, but listing emission is still blocked by host execution denial

- What changed:
  - replaced the earlier broken Windows helper path with a derived-only build workflow in:
    - `tools/build_derived_yayul.ps1`
  - that helper now:
    - copies `third_party/apollo/upstream/virtualagc/yaYUL/*` into `third_party/_derived_tools/yaYUL-compat-src/`
    - patches only the derived `yaYUL.h` `MSC_VS` macros for Windows-compatible compound-literal syntax
    - imports the Visual Studio environment from `VsDevCmd.bat`
    - compiles the derived sources with `clang --target=x86_64-pc-windows-msvc`
    - links them successfully with `clang -fuse-ld=link`
    - produces `third_party/_derived_tools/yaYUL.exe`
  - added a derived listing runner in:
    - `tools/generate_luminary099_listing.ps1`
  - confirmed the older fallback script:
    - `tools/prove_luminary099_executive_labels.py`
    is not trustworthy enough for exact Executive work because its validation mismatches known exact labels
  - attempted to run the derived `yaYUL.exe` against a copied Luminary 099 source tree, but Windows denied execution with:
    - `Access is denied`
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/MAIN.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/yaYUL/*`
- What visible/runtime behavior became more emulator-driven:
  - no runtime behavior changed in this pass
  - no new scheduler/job-switch label is being claimed from this pass
- What still remains fallback/custom:
  - no trustworthy Luminary 099 `.lst` or exact Executive extract was emitted yet
- exact `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB` mappings remain intentionally unclaimed
- the remaining late `SUPDXCHZ` invocation trigger still exists as documented technical debt

## 2026-04-17 - the local Executive proof path now has a stronger negative alignment check, but still no honest scheduler-label proof

- what changed:
  - added `tools/check_luminary099_executive_alignment.py`
  - the script compares exact Executive source opcode sequences against the already-generated `third_party/_derived_tools/luminary099_bank02_full.dtest.txt`
  - it writes:
    - `third_party/_derived_tools/luminary099_executive_alignment_check.json`
    - `third_party/_derived_tools/luminary099_executive_alignment_check.txt`
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/_derived_tools/luminary099_bank02_full.dtest.txt`
- what the stronger check proved:
  - exact `SUPDXCHZ` still aligns cleanly at `02:3165`
  - the current bank-02 disassembly contains no intact exact or opcode-only local sequence matches for:
    - `CHANJOB`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
- what became more emulator-driven:
  - none
  - this pass was proof-path work, not a runtime-ownership gain
- what still remains fallback/custom:
  - the remaining late `SUPDXCHZ` invocation trigger still exists
  - exact scheduler/job-switch labels around that trigger remain intentionally unclaimed until a trustworthy listing or equivalent exact proof path exists

## 2026-04-17 - bank-split Executive proof now establishes exact scheduler labels, and the late `SUPDXCHZ` invocation trigger now prefers proven Apollo scheduler boundaries

- What changed:
  - corrected the Executive proof path bank split:
    - `CHANJOB`, `DUMMYJOB`, `ADVAN`, and `NUDIRECT` are proven from bank `01`
    - `SUPDXCHZ` remains proven from bank `02`
  - updated `tools/check_luminary099_executive_alignment.py` so it compares:
    - bank-01 scheduler labels against `third_party/_derived_tools/luminary099_bank01_full.dtest.txt`
    - `SUPDXCHZ` against `third_party/_derived_tools/luminary099_bank02_full.dtest.txt`
  - strengthened the alignment script so it normalizes source-side symbolic operands to the exact disassembly forms used in the imported rope disassembly
  - the alignment report now proves exact hits for:
    - `CHANJOB` at `01:2706`
    - `DUMMYJOB` at `01:3206`
    - `ADVAN` at `01:3214`
    - `NUDIRECT` at `01:3225`
    - `SUPDXCHZ` at `02:3165`
  - added those exact scheduler labels to the derived runtime rope-label overlay
  - reduced the remaining late `SUPDXCHZ` invocation trigger:
    - after exact `RESUME`, the routed key path now prefers exact scheduler/job-switch labels
      - `CHANJOB`
      - `ADVAN`
      - `NUDIRECT`
    - only if those scheduler boundaries are not reached in time does the old bounded post-`RESUME` timer still fire
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the late request-dispatch trigger is now less timer-driven and more Apollo-scheduler-driven because it waits for proven scheduler/job-switch labels before falling back
  - `EXEC <label>` can now honestly surface the current routed key path inside:
    - `CHANJOB`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
- What still remains fallback/custom:
  - the late `SUPDXCHZ` invocation trigger still exists as a fallback timer when the proven scheduler labels are not reached in time
  - the post-`SUPDXCHZ` completion trigger still exists
  - full Apollo-owned scheduler/core-set/job-switch aftermath after `SUPDXCHZ` is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - deeper exact Executive aftermath labels are now proven, and the post-`SUPDXCHZ` completion timer is reduced by exact `ENDPRCHG` / `INTRSM` boundaries

- What changed:
  - extended the bank-split proof path beyond the current scheduler slice
  - added exact proof for:
    - `ENDPRCHG` at `01:2765`
    - `NUCHANG2` at `01:3011`
    - `INTRSM` at `03:2050`
  - generated a bank-03 interpreter disassembly window in:
    - `third_party/_derived_tools/luminary099_bank03_interp_window.dtest.txt`
  - extended `tools/check_luminary099_executive_alignment.py` so it now proves labels from:
    - `EXECUTIVE.agc`
    - `INTERPRETER.agc`
    against:
    - `luminary099_bank01_full.dtest.txt`
    - `luminary099_bank02_full.dtest.txt`
    - `luminary099_bank03_interp_window.dtest.txt`
  - added the new exact labels to the derived runtime rope-label overlay
  - reduced the post-`SUPDXCHZ` completion timer:
    - `continueAfterExecutiveDispatch(...)` now returns on exact `ENDPRCHG`
    - it also returns on exact `INTRSM`
    - only if those exact completion boundaries are not reached does the older post-target budget still apply
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/INTERPRETER.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the post-`SUPDXCHZ` routed path now has exact Apollo-owned completion boundaries in Executive/Interpreter space instead of relying only on target activation plus a flat instruction budget
  - `EXEC <label>` can now honestly surface:
    - `ENDPRCHG`
    - `NUCHANG2`
    - `INTRSM`
- What still remains fallback/custom:
  - the late `SUPDXCHZ` invocation trigger still exists as a fallback timer if the proven scheduler labels are not reached
  - the post-`SUPDXCHZ` completion budget still exists as fallback when `ENDPRCHG` / `INTRSM` are not reached
  - deeper Apollo-owned scheduler/core-set/job-switch aftermath beyond these exact labels is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - the late invocation handoff now waits for exact natural `SUPDXCHZ` transfer state instead of dispatching at the first scheduler label

- What changed:
  - preserved the exact proven scheduler/job-switch slice:
    - `CHANJOB`
    - `ADVAN`
    - `NUDIRECT`
  - preserved the exact proven completion slice:
    - `ENDPRCHG`
    - `INTRSM`
  - removed the earlier behavior where the routed key path injected the pending request as soon as it reached one of the scheduler labels
  - the routed path now continues stepping through more of the exact Apollo middle transition logic until it reaches exact natural transfer state:
    - `SUPDXCHZ`
    - `SUPDXCHZ +1`
  - only at that exact transfer state does the runtime arm the captured `NEWLOC` / `NEWLOC+1` request into `A+L`
  - only if those exact transfer states are not reached in time does the old bounded post-`RESUME` timer still fire
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/INTERPRETER.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the late invocation path is now less timer-driven and less scheduler-label-triggered, because it waits for Apollo to reach exact `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before the remaining handoff occurs
  - more of the exact middle scheduler/core-set transition path can now execute before the remaining fallback injection happens
- What still remains fallback/custom:
  - the late invocation fallback timer still exists if the exact natural transfer states are not reached in time
  - the post-`SUPDXCHZ` completion budget still exists as fallback when `ENDPRCHG` / `INTRSM` are not reached
  - deeper Apollo-owned scheduler/core-set/job-switch aftermath beyond these exact boundaries is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - the remaining late invocation timer now starts only after exact transition-gap labels instead of immediately after `RESUME`

- What changed:
  - proved exact transition-gap labels in the middle path:
    - `JOBSLP1` at `01:2776`
    - `JOBSLP2` at `01:3007`
  - added those exact labels to the derived runtime rope-label overlay
  - reduced the remaining late invocation timer again:
    - it no longer starts immediately after exact `RESUME`
    - it now starts only after the routed path reaches exact transition-gap labels:
      - `JOBSLP1`
      - `JOBSLP2`
      - `NUCHANG2`
    - if Apollo reaches exact natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state first, the remaining timer never fires
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - more of the Apollo-owned transition-gap path can now execute before the remaining fallback timer is even eligible to fire
  - `EXEC <label>` can now honestly surface:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
- What still remains fallback/custom:
  - the late invocation timer still exists if the exact transition-gap and natural transfer states are not reached in time
  - the post-`SUPDXCHZ` completion budget still exists as fallback when `ENDPRCHG` / `INTRSM` are not reached
  - deeper Apollo-owned scheduler/core-set/job-switch aftermath beyond these exact boundaries is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - the separate late invocation timer is gone; the routed key path now waits on exact Apollo state until natural transfer or overall routed-step exhaustion

- What changed:
  - removed the separate late invocation timer from `runInstructionRoutedApolloInput(...)`
  - preserved the exact transition-gap labels:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
  - preserved exact natural transfer-state handling:
    - `SUPDXCHZ`
    - `SUPDXCHZ +1`
  - after exact `RESUME`, the routed path now keeps stepping on exact Apollo-owned state until:
    - exact natural transfer state is reached
    - or the overall routed-step budget ends and the remaining fallback primitive is forced
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the last separate invocation timer debt is gone
  - more of the exact transition-gap and natural-transfer path can now execute before any fallback is allowed
- What still remains fallback/custom:
  - the remaining invocation fallback primitive still exists if the routed path never reaches natural transfer state before the overall routed-step budget ends
  - the post-`SUPDXCHZ` completion budget still exists as fallback when `ENDPRCHG` / `INTRSM` are not reached
  - deeper Apollo-owned scheduler/core-set/job-switch aftermath beyond these exact boundaries is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-17 - the final pre-transfer fallback primitive is narrower, and post-`SUPDXCHZ` completion can now stop on exact `TASKOVER`

- What changed:
  - proved exact `TASKOVER` at `02:3261` from:
    - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
    - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
    - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
  - added `TASKOVER` to the derived runtime rope-label overlay and the exact routine map
  - reduced the final routed-step exhaustion fallback primitive:
    - if routed-step exhaustion happens after Apollo has already entered the exact final pre-transfer transition slice:
      - `JOBSLP1`
      - `JOBSLP2`
      - `NUCHANG2`
      - `DUMMYJOB`
      - `ADVAN`
      - `NUDIRECT`
      the runtime now gives that exact Apollo slice one more continuation window to reach natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before any forced handoff is allowed
  - reduced the post-`SUPDXCHZ` completion fallback budget:
    - `continueAfterExecutiveDispatch(...)` now also returns on exact `TASKOVER`
    - only if exact `ENDPRCHG`, `TASKOVER`, or `INTRSM` are not reached does the fallback post-target budget still end the routed flow
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
  - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
- What visible/runtime behavior became more emulator-driven:
  - no new device-confirmed visible DSKY consequence is claimed from this pass alone
  - the remaining routed-step exhaustion fallback is narrower because exact Apollo state in the final transition slice gets one more chance to reach natural transfer before any forced handoff is allowed
  - post-`SUPDXCHZ` completion is less budget-driven because exact `TASKOVER` is now another real Apollo-owned completion boundary
  - `EXEC <label>` can now honestly surface:
    - `TASKOVER`
- What still remains fallback/custom:
  - the final forced handoff still exists if Apollo still never reaches natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before the overall routed-step exhaustion plus the exact final-slice continuation window ends
  - the post-`SUPDXCHZ` completion budget still exists when exact `ENDPRCHG`, `TASKOVER`, or `INTRSM` are not reached
  - deeper Apollo-owned scheduler/core-set/job-switch aftermath beyond these exact boundaries is still incomplete
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-20 - exact `SELFRET` fresh-start state is now seeded, and the final pre-transfer slice can surface exact `SELFBANK`

- What changed:
  - extended the Apollo-derived custom erasable initializer with one more exact fresh-start Executive word:
    - `SELFRET` at erasable `1361`
    - seeded to exact Apollo fresh-start value `06102`
    - derived from untouched `FRESH_START_AND_RESTART.agc`:
      - `CAF LESCHK`
      - `TS SELFRET`
      - `LESCHK GENADR SELFCHK`
  - added exact `SELFBANK` at `01:3224` to the derived rope-label overlay and exact routine map
  - preserved all current proven routed boundaries:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
    - natural `SUPDXCHZ` / `SUPDXCHZ +1`
    - completion-side `ENDPRCHG` / `TASKOVER` / `INTRSM`
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/FRESH_START_AND_RESTART.agc`
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/ERASABLE_ASSIGNMENTS.agc`
  - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- Which exact Apollo labels in the final remaining pre-transfer segment are now exercised:
  - the previously proven slice remains:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
  - exact runtime visibility now also extends one label deeper to:
    - `SELFBANK`
- What fallback primitive or completion budget was reduced or removed:
  - none removed in this pass
  - this pass closes an Apollo-owned state gap inside the final pre-transfer slice rather than claiming a reduction that has not been runtime-proven yet
- What visible/runtime consequence is now more Apollo-driven:
  - the exact `ADVAN -> SELFBANK -> SUPDXCHZ +1` idle/self-check dispatch path now has its Apollo fresh-start return word present instead of relying on an unseeded zero-valued custom gap at `SELFRET`
  - the runtime/debug path can now honestly surface `SELFBANK` if Apollo reaches that deeper final-slice label
- What still remains fallback/custom:
  - the final forced handoff still exists if Apollo still never reaches natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before routed-step exhaustion
  - the post-`SUPDXCHZ` completion budget still exists when exact `ENDPRCHG`, `TASKOVER`, or `INTRSM` are not reached
  - the pending routed request is still not made self-dispatching purely from Apollo-owned scheduler/job-switch state
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-20 - Executive one’s-complement arithmetic is now closer to Apollo on the routed scheduler path, but the last forced handoff still remains

- What changed:
  - corrected the native CPU’s single-precision one’s-complement arithmetic for the Executive path:
    - `AD`
    - `ADS`
    - `SU`
  - these opcodes now use end-around-carry addition instead of plain masked binary addition
  - preserved all current proven routed boundaries:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - natural `SUPDXCHZ` / `SUPDXCHZ +1`
    - completion-side `ENDPRCHG` / `TASKOVER` / `INTRSM`
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
  - `third_party/apollo/upstream/virtualagc/yaAGC/agc_engine.c`
- Which exact Apollo labels in the final remaining transition segment are now exercised:
  - no newly runtime-proven final-segment labels were added in this pass
  - the currently proven exercised slice remains:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
- What final fallback primitive or completion budget was reduced or removed:
  - none removed in this pass
  - this pass removes a real arithmetic mismatch on the Apollo-owned scheduler path, but the remaining forced handoff at routed-step exhaustion has not yet been honestly eliminated
- What runtime consequence is now more Apollo-driven:
  - Executive priority/new-job arithmetic on the routed path is now closer to real Apollo behavior in source-proven instructions such as:
    - `SETLOC`
    - `SPECTEST`
    - `ENDPRCHG`
  - this reduces emulator-owned distortion in the scheduler/job-switch state that decides whether Apollo should make its own way into the natural `SUPDXCHZ` transfer corridor
- What still remains fallback/custom:
  - the final forced handoff still exists if Apollo still does not reach natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before routed-step exhaustion
  - the post-`SUPDXCHZ` completion budget still exists when exact `ENDPRCHG`, `TASKOVER`, or `INTRSM` are not reached
  - the pending routed request is still not made self-dispatching purely from Apollo-owned scheduler/job-switch state
  - local fallback command parsing and entry buffering still remain when Apollo display/input ownership is absent
  - phase ownership, telemetry, and mission outcomes remain compatibility-owned

## 2026-04-20 - executable erasable/central instruction fetch is now Apollo-owned, but the last forced handoff still remains

- What changed:
  - corrected native instruction fetch so the CPU no longer always fetches from rope
  - when `Z` points into executable erasable/central space below `02000`, the native CPU now fetches the actual erasable/central word at that address
  - fixed-memory fetch and rope-label lookup remain unchanged for fixed-bank instruction addresses
  - preserved all current exact routed boundaries:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - natural `SUPDXCHZ` / `SUPDXCHZ +1`
    - completion-side `ENDPRCHG` / `TASKOVER` / `INTRSM`
- Apollo artifact used:
  - `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
  - `third_party/apollo/apollo11/lm/luminary099/WAITLIST.agc`
- Which exact Apollo labels in the final remaining transition segment are now exercised:
  - no newly runtime-proven final-segment labels were added in this pass
  - the currently proven exercised slice remains:
    - `JOBSLP1`
    - `JOBSLP2`
    - `NUCHANG2`
    - `DUMMYJOB`
    - `ADVAN`
    - `NUDIRECT`
- What final forced handoff or completion budget was reduced or removed:
  - none removed in this pass
  - this pass removes a real CPU execution-semantic mismatch on the exact transfer/completion path, but the final forced handoff and the post-`SUPDXCHZ` completion budget were not claimed reduced without runtime proof
- What runtime consequence is now more Apollo-driven:
  - Apollo-owned executable erasable/central instructions can now actually execute as instructions instead of being replaced by an incorrect rope fetch
  - this is directly relevant to exact Apollo aftermath on this path where control transfers depend on executable register/erasable state, including:
    - `SUPDXCHZ` `TC L`
    - `ENDPRCHG` / `DTCB`-style return sequencing
  - the remaining transfer/completion path is therefore less emulator-distorted even though no new runtime label proof is claimed yet
- What still remains fallback/custom:
  - the final forced handoff still exists if Apollo still does not reach natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before routed-step exhaustion
  - the post-`SUPDXCHZ` completion budget still exists when exact `ENDPRCHG`, `TASKOVER`, or `INTRSM` are not reached
  - the pending routed request is still not made self-dispatching purely from Apollo-owned scheduler/job-switch state
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

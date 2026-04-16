# CompatibilityScenario Reduction

## Moved out already

- native CPU rope-label execution tracking
- DSKY `COMP ACTY`
- DSKY execution status such as `EXEC <label>`
- DSKY unsupported-opcode status such as `UNSUPPORTED <label>`
- 1201/1202 alarm ownership in `AlarmExecutive`
- banked rope/fixed/erasable address ownership in the native CPU
- channel 7 superbank ownership in the native CPU
- DSKY relay-row ownership for visible:
  - `PROG`
  - `VERB`
  - `NOUN`
  - register digits
- channel-11 annunciator ownership for visible:
  - `COMP ACTY`
  - `KEY REL`
  - `OPR ERR`
- DSKY key injection into Apollo-facing input channels for:
  - keypad keys on channel 15
  - `VERB`
  - `NOUN`
  - `CLR`
  - `ENTR`
  - `RSET`
  - `KEY REL`
  - `PRO` pulse on channel 32 bit 14
- exact Luminary 099 DSKY input entry routing for:
  - a partial exact `KEYRUPT1` / `KEYCOM` / `ACCEPTUP` lead-in on channel-15 key input
  - exact `LODSAMPT` on channel-15 key input
  - exact `NOVAC` request capture on channel-15 key input
  - exact `NOVAC2` / `NOVAC3` / `CORFOUND` / `SETLOC` Executive aftermath on channel-15 key input
  - exact `WAITLIST` `RESUME` / `NOQRSM` / `NOQBRSM` interrupt-return preparation on channel-15 key input
  - later `NEWLOC`/`2CADR`-derived request dispatch on channel-15 key input
  - `PROCKEY` on `PRO`
- momentary release of channel-15 DSKY key input back to zero after the next native step
- exact execution/source mapping for mapped Luminary 099 DSKY/input labels in the engineer/source debug path
- DSKY register arbitration for:
  - landing monitor
  - phase summary
  - last-alarm display
- Apollo-corrected relay-row decoding for visible DSKY register digits/signs when channel-10 output is present
- corrected Apollo double-word CPU semantics for `DCA` / `DAS` / `DXCH` pair ordering
- real Apollo `WAITLIST RESUME` CPU semantics for the routed key path
- more honest interrupt lead-in seeding for the routed key path:
  - `ARUPT`
  - `LRUPT`
  - `BRUPT`
  - interrupted `BBANK` handoff into `KEYRUPT1`

## Still owned by CompatibilityScenario

- descent physics
- mission time advancement
- throttle trim effects
- target vertical velocity fallback logic
- load-ratio synthesis
- fallback phase/program transition thresholds for `P63` to `P64` and `P64` to `P66`
- mission outcomes
- telemetry source values:
  - altitude
  - vertical velocity
  - horizontal velocity
  - fuel
- visible phase labels
- non-execution event text such as touchdown or fuel exhaustion
- fallback local verb/noun buffering and command parsing when Apollo DSKY entry routing is absent
- fallback local `KEY REL` / `RSET` / alarm acknowledgement behavior when Apollo/peripheral ownership is absent
- missing Apollo scheduler/job-switch and final interrupt-return aftermath after the current `KEYRUPT1` / `LODSAMPT` / `NOVAC2` / `SETLOC` / `RESUME` lead-in

## Newly reduced this pass

- the old narrow native dispatch at the final `WAITLIST RESUME` opcode boundary is gone
- the routed key path now gets a real `RESUME` instruction plus a longer post-`RESUME` Apollo execution window before any fallback dispatch is allowed
- the routed key path now enters `KEYRUPT1` with a more honest rupture/return register setup instead of only a bank jump into the label
- the later fallback dispatch now waits through a larger routed post-`RESUME` execution budget than before
- the old hard-coded `CHARIN_2CADR -> CHARIN` fallback is gone
- the remaining late dispatch now uses Apollo-captured `NEWLOC` / `NEWLOC+1` request state instead of a single hard-coded target label
- the routed key path now continues stepping after that late dispatch, so more of the post-request consequence can come from Apollo-executed code before returning to the normal simulation loop

## Next candidate to remove

- phase/program transition ownership

Reason:

- The displayed program digits can now be Apollo-driven, and `P64`/`P66` adoption can now follow that output.
- The local DSKY input consequence path has been reduced for the active Luminary 099 path.
- The remaining blocker is still the lack of enough Apollo-owned Executive scheduling/job-switch/final interrupt-return/peripheral state to eliminate the fallback thresholds honestly.

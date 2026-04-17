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
  - exact `SUPDXCHZ` transfer on that late request dispatch
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
- the remaining late dispatch now enters exact Apollo `SUPDXCHZ` for bank/superbank/job-target transfer instead of performing that transfer entirely in emulator code
- the remaining late dispatch trigger now waits for exact Apollo `RESUME` plus a bounded post-`RESUME` Apollo window instead of being only a flat key-entry instruction timer
- post-`SUPDXCHZ` continuation now waits for the Apollo-requested target to become active before starting a smaller bounded post-target window
- the remaining late dispatch trigger now prefers exact Apollo scheduler/job-switch labels:
  - `CHANJOB`
  - `ADVAN`
  - `NUDIRECT`
  before the older bounded post-`RESUME` timer is allowed to fire
- post-`SUPDXCHZ` completion can now stop on exact deeper Apollo aftermath labels:
  - `ENDPRCHG`
  - `INTRSM`
  before the older post-target instruction budget is allowed to fire
- the remaining late dispatch trigger now waits for exact natural Apollo transfer state:
  - `SUPDXCHZ`
  - `SUPDXCHZ +1`
  instead of dispatching at the first proven scheduler-label boundary
- the old separate late invocation timer after `RESUME` is gone
- the routed key path now waits on exact Apollo transition-gap and natural transfer state until the overall routed-step bound is exhausted

## Next candidate to remove

- phase/program transition ownership

Reason:

- The displayed program digits can now be Apollo-driven, and `P64`/`P66` adoption can now follow that output.
- The local DSKY input consequence path has been reduced for the active Luminary 099 path.
- The remaining blocker is still the lack of enough Apollo-owned Executive scheduling/job-switch/final interrupt-return/peripheral state to eliminate the fallback thresholds honestly.

## Remaining hard-coded technical debt

- Path: `NativeApolloCore::runInstructionRoutedApolloInput` late request trigger
  - Why it exists: the emulator still does not implement enough Apollo-owned Executive scheduler/job-switch aftermath to know purely from Apollo state when the pending request should be handed from the routed interrupt-return path into the job-dispatch path
  - Apollo-owned replacement target: exact Executive scheduler boundaries around `DUMMYJOB`, `ADVAN`, `NUDIRECT`, `CHANJOB`, and their core-set/job-switch aftermath
  - Reduced this batch: yes; the trigger no longer dispatches at the first proven scheduler label, now waits for exact natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state, and the separate late invocation timer is gone

- Path: `NativeApolloCore::continueAfterExecutiveDispatch` post-`SUPDXCHZ` completion trigger
  - Why it exists: the emulator still does not implement enough Apollo-owned scheduler/job-switch aftermath to know purely from Apollo state when the dispatched job has fully taken ownership
  - Apollo-owned replacement target: deeper exact Executive scheduler/job-switch aftermath after `SUPDXCHZ`, especially around `DUMMYJOB`, `ADVAN`, `NUDIRECT`, and `CHANJOB`
  - Reduced this batch: yes; exact `ENDPRCHG` and `INTRSM` now terminate the routed post-dispatch window before the fallback post-target budget is allowed to fire

- Path: exact scheduler-label derivation around `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB`
  - Why it exists: broader Executive work still needs more exact labels than the currently proven set, and the local `.lst` path is still blocked
  - Apollo-owned replacement target: a stricter exact listing/disassembly path that can prove the real Luminary 099 addresses for those scheduler labels without modifying Apollo artifacts
  - Reduced this batch: yes; the bank-split disassembly proof now proves exact `CHANJOB`, `DUMMYJOB`, `ADVAN`, `NUDIRECT`, and `SUPDXCHZ`

- Path: deeper exact Executive aftermath derivation beyond the current scheduler slice
  - Why it exists: the routed key path still needs more Apollo-owned completion boundaries after scheduler/job-switch transfer
  - Apollo-owned replacement target: exact deeper labels after `SUPDXCHZ` and their runtime consequences
  - Reduced this batch: yes; exact `ENDPRCHG`, `NUCHANG2`, and `INTRSM` are now proven and mapped

- Path: deeper exact transition-gap derivation between the scheduler slice and natural transfer state
  - Why it exists: the routed key path still needed exact labels in the middle transition gap so the remaining invocation timer could be tied to Apollo state rather than to `RESUME` alone
  - Apollo-owned replacement target: exact middle transition labels and eventual elimination of the remaining invocation timer
  - Reduced this batch: yes; exact `JOBSLP1` and `JOBSLP2` are now proven and mapped, and the remaining separate invocation timer is gone

- Path: local `yaYUL` listing tool build
  - Why it exists: the derived Windows build path now succeeds far enough to produce `third_party/_derived_tools/yaYUL.exe`, but the host still denies executing that derived helper from this workspace, so it still cannot emit a listing
  - Apollo-owned replacement target: a working local assembler/listing path that can assemble or list Luminary 099 without changing the imported Apollo artifacts
  - Reduced this batch: yes

- Path: `DskyIo::pressKey` local verb/noun entry buffering and command parsing fallback
  - Why it exists: Apollo-owned display/input routing is still incomplete when relay output or routed Apollo input ownership is absent
  - Apollo-owned replacement target: end-to-end Apollo Pinball/input consequence flow from channel input through Executive/Pinball ownership
  - Reduced this batch: no

- Path: `DskyIo` local `RSET` / `KEY REL` / alarm acknowledgement fallback
  - Why it exists: Apollo/peripheral ownership still does not fully cover those consequences when Apollo relay output and routed input ownership are absent
  - Apollo-owned replacement target: real Apollo-owned DSKY/peripheral/alarm consequence paths
  - Reduced this batch: no

- Path: `CompatibilityScenario` fallback phase/program thresholds for `P63 -> P64` and `P64 -> P66`
  - Why it exists: the runtime still does not honestly produce enough Apollo-owned mission/program state to replace those transitions end-to-end
  - Apollo-owned replacement target: Apollo-owned program/phase transitions from native execution and peripheral/display state
  - Reduced this batch: no

- Path: `CompatibilityScenario` mission/telemetry/outcome ownership
  - Why it exists: descent physics, mission time, telemetry source values, and outcomes are still not owned by real Apollo execution in the current runtime
  - Apollo-owned replacement target: broader Apollo-owned runtime execution and peripheral/state ownership, with compatibility logic reduced to bootstrap only
  - Reduced this batch: no

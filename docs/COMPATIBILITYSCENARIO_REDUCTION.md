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
  - `ENDOFJOB`
  - `ENDPRCHG`
  - `INTRSM`
  before the older post-target instruction budget is allowed to fire
- the remaining late dispatch trigger now waits for exact natural Apollo transfer state:
  - `SUPDXCHZ`
  - `SUPDXCHZ +1`
  instead of dispatching at the first proven scheduler-label boundary
- the old separate late invocation timer after `RESUME` is gone
- the routed key path now waits on exact Apollo transition-gap and natural transfer state until the overall routed-step bound is exhausted
- the native CPU now fetches executable erasable/central instructions from actual erasable/central state instead of always substituting a rope fetch, which is required by exact Apollo control-transfer aftermath such as `SUPDXCHZ` `TC L`
- if the overall routed-step bound is exhausted after Apollo has already entered the exact final pre-transfer transition slice:
  - `JOBSLP1`
  - `JOBSLP2`
  - `NUCHANG2`
  - `DUMMYJOB`
  - `ADVAN`
  - `SELFBANK`
  - `NUDIRECT`
  the runtime now gives that exact Apollo slice one more continuation window to reach natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before the remaining forced handoff is allowed
- post-`SUPDXCHZ` completion can now also stop on exact `TASKOVER` before the fallback post-target budget is allowed to fire
- the custom erasable initializer now includes the exact Apollo fresh-start `SELFRET` word used by the `ADVAN -> SELFBANK -> SUPDXCHZ +1` idle/self-check dispatch path
- the routed key path now captures the real Apollo `2CADR CHARIN` request words at exact `NOVAC / DXCH NEWLOC` instead of a zeroed request, because the native CPU now:
  - clears stale routed interrupt `INDEX` / `EXTEND` state
  - preserves `EXTEND INDEX` into the following instruction
  - carries full Apollo-style `Z` / `Q` return addresses
  - uses Apollo-correct `DCA` / `DXCH` pair ordering on that path
- the routed post-capture aftermath now executes through the previously blocking Apollo opcode classes:
  - extended `DCS`
  - extended `AUG`
  instead of stopping on unsupported-opcode traps at the exact routed sites previously proven in bank `03`

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
  - Reduced this batch: not yet; this pass corrected native one’s-complement `AD` / `ADS` / `SU` arithmetic used on the routed Executive path, but the remaining forced handoff still exists until Apollo-owned scheduler/job-switch state can make the pending request self-dispatch without emulator intervention

- Path: `NativeApolloCore::continueAfterExecutiveDispatch` post-`SUPDXCHZ` completion trigger
  - Why it exists: the emulator still does not implement enough Apollo-owned scheduler/job-switch aftermath to know purely from Apollo state when the dispatched job has fully taken ownership
  - Apollo-owned replacement target: deeper exact Executive scheduler/job-switch aftermath after `SUPDXCHZ`, especially around `DUMMYJOB`, `ADVAN`, `NUDIRECT`, and `CHANJOB`
  - Reduced this batch: yes; exact `ENDPRCHG`, `TASKOVER`, and `INTRSM` now terminate the routed post-dispatch window before the fallback post-target budget is allowed to fire

- Path: native Executive arithmetic on the routed scheduler path
  - Why it exists: the routed path still depended on plain masked binary addition where Apollo Executive state uses one’s-complement arithmetic with end-around carry
  - Apollo-owned replacement target: Apollo-correct arithmetic for the Executive priority/new-job state transitions inside `SETLOC`, `SPECTEST`, `ENDPRCHG`, and adjacent scheduler code
  - Reduced this batch: yes; `AD`, `ADS`, and `SU` now use one’s-complement end-around-carry arithmetic in the native CPU

- Path: exact scheduler-label derivation around `DUMMYJOB` / `ADVAN` / `NUDIRECT` / `CHANJOB`
  - Why it exists: broader Executive work still needs more exact labels than the currently proven set, and the local `.lst` path is still blocked
  - Apollo-owned replacement target: a stricter exact listing/disassembly path that can prove the real Luminary 099 addresses for those scheduler labels without modifying Apollo artifacts
  - Reduced this batch: yes; the bank-split disassembly proof now proves exact `CHANJOB`, `DUMMYJOB`, `ADVAN`, `NUDIRECT`, and `SUPDXCHZ`

- Path: deeper exact Executive aftermath derivation beyond the current scheduler slice
  - Why it exists: the routed key path still needs more Apollo-owned completion boundaries after scheduler/job-switch transfer
  - Apollo-owned replacement target: exact deeper labels after `SUPDXCHZ` and their runtime consequences
  - Reduced this batch: yes; exact `ENDPRCHG`, `NUCHANG2`, `TASKOVER`, and `INTRSM` are now proven and mapped

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

- Path: routed `NOVAC` request capture semantics
  - Why it existed: the routed path had been capturing a zero request because the emulator was mis-handling pending `EXTEND` / `INDEX`, indexed instruction formation, `Q` return addresses, and `DCA` / `DXCH` pair ordering
  - Apollo-owned replacement target: exact Apollo `INDEX Q / DCA 0 / DXCH NEWLOC` capture of the caller's `2CADR`
  - Reduced this batch: yes; the routed key path now captures the real `2CADR CHARIN` words `02077 / 60101` from the exact callsite instead of a zero request

- Path: routed post-capture opcode coverage
  - Why it exists: after exact `NOVAC` request capture, the emulator still lacks enough Apollo opcode coverage for the routed aftermath to continue to natural transfer on its own
  - Apollo-owned replacement target: exact support for the now-proven blocking opcode classes encountered after request capture, currently `0140` and `0124` on the routed path, plus the next exact opcode exposed after those are fixed
  - Reduced this batch: yes; the exact blocking opcode classes `0140` (`DCS`) and `0124` (`AUG`) are now supported, so this is no longer the active blocker on the routed path

- Path: forced-dispatch `2CADR` decode / target consumption
  - Why it exists: the routed path now captures exact Apollo `2CADR` words, but the remaining emulator-side forced-dispatch path still does not consume those words with full Apollo-owned semantics
  - Apollo-owned replacement target: Apollo-owned scheduler/job-switch aftermath that naturally reaches `SUPDXCHZ` / `SUPDXCHZ +1`, or at minimum an exact `2CADR` decode/dispatch path that honors bank and superbank semantics of the captured request words
  - Reduced this batch: yes; the remaining observer/target-consumer now decodes the captured `NEWLOC +1` bank word with exact `2CADR` superbank semantics, so the routed trace now proves the effective target as `40:0077` instead of the earlier custom-misdecoded `30:0077`

- Path: custom post-`SUPDXCHZ` `CHARIN2` stop
  - Why it existed: after the forced `SUPDXCHZ` handoff, the routed key path was still stopping on a local `CHARIN2` shortcut rather than allowing the Apollo Pinball path to run to its own exit boundary
  - Apollo-owned replacement target: exact bank-40 Pinball aftermath through `CHARIN_PREENTRY` / `CHARIN` / `CHARIN2` and then exact Executive `ENDOFJOB`
  - Reduced this batch: yes; the local `CHARIN2` stop is gone, and the routed trace now proves Apollo-owned completion at exact `ENDOFJOB`

- Path: routed-input `CHARIN2` completion exit
  - Why it existed: even after the post-`SUPDXCHZ` path was allowed to run through exact `ENDOFJOB`, the outer routed-input/final-transition flow still treated `CHARIN2` itself as a success boundary
  - Apollo-owned replacement target: exact Apollo completion boundaries only, namely `ENDOFJOB`, `ENDPRCHG`, `TASKOVER`, and `INTRSM`
  - Reduced this batch: yes; the remaining hard-coded `CHARIN2` completion exits are gone, so routed completion now stops only on those exact Apollo boundaries

- Path: pre-transfer routed handoff trigger after exact `NOVAC` request capture
  - Why it exists: Apollo still does not carry the pending request all the way to natural `SUPDXCHZ` / `SUPDXCHZ +1` before routed-step exhaustion
  - Apollo-owned replacement target: exact bank-03 interpreter aftermath that leads from post-capture execution into `RESUME` / `INTRSM` and then the later proven scheduler/transfer corridor
  - Reduced this batch: no; the blocker is now documented more precisely as exact exhaustion at `03:0223` with `resume=no` and `finalSlice=no`

- Path: routed handoff trigger at exact executable-erasable `VAC1ADRC` loop
  - Why it exists: the routed path still does not carry the pending request to natural transfer on its own; after exact request capture it falls into executable erasable at `0223`, whose current seeded word is the `VAC1ADRC` self-pointer `00223`
  - Apollo-owned replacement target: exact Apollo return/transfer semantics and/or exact erasable state that should carry the path from that `VAC1ADRC` availability state onward into `RESUME` / later scheduler transfer ownership without emulator handoff
  - Reduced this batch: yes; the remaining handoff is no longer keyed only to flat routed-step exhaustion when that state is reached, and now uses an exact-stall continuation window plus an exact-stall-triggered handoff

- Path: bank-03 post-capture `TC 0177` drop into executable erasable core-set state
  - Why it exists: the routed trace now proves the pre-transfer drop is not just the later `0223` self-loop; bank-03 rope aftermath executes a real `TC 0177`, then linear execution walks erasable words until the later `0223` loop
  - Apollo-owned replacement target: exact Executive/interpreter/core-set state or return/transfer semantics that should keep Apollo from executing the anonymous erasable pool beginning at `0177`
  - Reduced this batch: no runtime path removed, but the blocker is now identified precisely enough to replace the earlier vague “bank-03 aftermath” description

- 2026-04-24 clarification for the `TC 0177` blocker
  - the latest routed proof identifies `0177` as dynamic core set 1 `MODE`
  - the following saved core-set words are:
    - `0200` -> `LOC`
    - `0201` -> `BANKSET`
    - `0202` -> `PUSHLOC`
    - `0203` -> `PRIORITY`
  - Reduced this batch: yes; the remaining continuation/handoff path is now keyed to the exact core-set-drop origin at `03:0177` rather than only the downstream `03:0223` loop
  - Apollo-owned replacement target: exact return/transfer/core-set semantics that should provide valid suspended-job or resumed interpretive state at that drop instead of dormant core-set contents

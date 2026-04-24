# Routine Address Map

## Scope

This file documents the exact Apollo routine addresses currently derived and used for Luminary 099 DSKY/input work.

It is not a claim of full Pinball ownership.
It is not a claim of full interrupt ownership.
It is a record of exact bank-local addresses derived from untouched Apollo artifacts and then used by the emulator/runtime around those artifacts.

The separate listing/disassembly proof status for unresolved Executive labels is documented in:

- `docs/LISTING_PROOF_PATH.md`

## Derivation method

The addresses below were derived from imported Apollo artifacts without modifying them:

1. Source labels and source sections were read from:
   - `third_party/apollo/apollo11/lm/luminary099/PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
   - `third_party/apollo/apollo11/lm/luminary099/KEYRUPT,_UPRUPT.agc`
   - `third_party/apollo/apollo11/lm/luminary099/T4RUPT_PROGRAM.agc`
2. Bank placement was verified from:
   - `third_party/apollo/apollo11/lm/luminary099/TAGS_FOR_RELATIVE_SETLOC_AND_BLANK_BANK_CARDS.agc`
3. Exact bank-local offsets were derived from the real rope image:
   - `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
4. The rope was disassembled with the imported Virtual AGC disassembler, without changing Apollo source:
   - `third_party/apollo/upstream/virtualagc/Tools/disassemblerAGC/disassemblerAGC.py`
5. Disassembly sequences were matched against the imported source sequences to confirm the exact routine entry points.

Derived disassembly working files used during this pass:

- `third_party/_derived_tools/luminary099_bank40.dtest.txt`
- `third_party/_derived_tools/luminary099_bank04_keyrupt.dtest.txt`

Those files are derived debugging artifacts, not Apollo artifacts.

## Exact map

### Exact Apollo labels

- `KEYRUPT1` -> bank `04`, offset `3274`, source `KEYRUPT,_UPRUPT.agc`, section `KEYRUPT1`
- `KEYCOM` -> bank `04`, offset `3303`, source `KEYRUPT,_UPRUPT.agc`, section `KEYCOM`
- `ACCEPTUP` -> bank `04`, offset `3307`, source `KEYRUPT,_UPRUPT.agc`, section `ACCEPTUP`
- `LODSAMPT` -> bank `02`, offset `0400`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `LODSAMPT`
- `NOVAC` -> bank `02`, offset `1072`, source `EXECUTIVE.agc`, section `NOVAC`
- `NOVAC2` -> bank `02`, offset `1261`, source `EXECUTIVE.agc`, section `NOVAC2`
- `NOVAC3` -> bank `02`, offset `1264`, source `EXECUTIVE.agc`, section `NOVAC3`
- `CORFOUND` -> bank `02`, offset `1272`, source `EXECUTIVE.agc`, section `CORFOUND`
- `SPECTEST` -> bank `02`, offset `1305`, source `EXECUTIVE.agc`, section `SPECTEST`
- `SETLOC` -> bank `02`, offset `1315`, source `EXECUTIVE.agc`, section `SETLOC`
- `NEXTCORE` -> bank `02`, offset `1330`, source `EXECUTIVE.agc`, section `NEXTCORE`
- `CHANJOB` -> bank `01`, offset `2706`, source `EXECUTIVE.agc`, section `CHANJOB`
- `ENDPRCHG` -> bank `01`, offset `2765`, source `EXECUTIVE.agc`, section `ENDPRCHG`
- `JOBSLP1` -> bank `01`, offset `2776`, source `EXECUTIVE.agc`, section `JOBSLP1`
- `JOBSLP2` -> bank `01`, offset `3007`, source `EXECUTIVE.agc`, section `JOBSLP2`
- `NUCHANG2` -> bank `01`, offset `3011`, source `EXECUTIVE.agc`, section `NUCHANG2`
- `DUMMYJOB` -> bank `01`, offset `3206`, source `EXECUTIVE.agc`, section `DUMMYJOB`
- `ADVAN` -> bank `01`, offset `3214`, source `EXECUTIVE.agc`, section `ADVAN`
- `SELFBANK` -> bank `01`, offset `3224`, source `EXECUTIVE.agc`, section `SELFBANK`
- `NUDIRECT` -> bank `01`, offset `3225`, source `EXECUTIVE.agc`, section `NUDIRECT`
- `ENDOFJOB` -> bank `02`, offset `3155`, source `EXECUTIVE.agc`, section `ENDOFJOB`
- `SUPDXCHZ` -> bank `02`, offset `3165`, source `EXECUTIVE.agc`, section `SUPDXCHZ`
- `TASKOVER` -> bank `02`, offset `3261`, source `WAITLIST.agc`, section `TASKOVER`
- `INTRSM` -> bank `03`, offset `2050`, source `INTERPRETER.agc`, section `INTRSM`
- `RESUME` -> bank `02`, offset `3270`, source `WAITLIST.agc`, section `RESUME`
- `NOQRSM` -> bank `02`, offset `3272`, source `WAITLIST.agc`, section `NOQRSM`
- `NOQBRSM` -> bank `02`, offset `3274`, source `WAITLIST.agc`, section `NOQBRSM`
- `UPOK` -> bank `04`, offset `3350`, source `KEYRUPT,_UPRUPT.agc`, section `UPOK`
- `HI10` -> bank `04`, offset `3374`, source `KEYRUPT,_UPRUPT.agc`, section `HI10`
- `ELRCODE` -> bank `04`, offset `3377`, source `KEYRUPT,_UPRUPT.agc`, section `ELRCODE`
- `CHARIN` -> bank `40`, offset `2100`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `CHARIN`
- `CHARIN2` -> bank `40`, offset `2112`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `CHARIN2`
- `CHARIN_PREENTRY` -> bank `40`, offset `2077`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `immediate pre-CHARIN bank-40 entry stub`
- `ENTERJMP` -> bank `40`, offset `2157`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `ENTERJMP`
- `89TEST` -> bank `40`, offset `2161`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `89TEST`
- `NUM` -> bank `40`, offset `2175`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `NUM`
- `PROCKEY` -> bank `40`, offset `3452`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `PROCKEY`
- `VBRESEQ` -> bank `40`, offset `3457`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `VBRESEQ`
- `VBRELDSP` -> bank `40`, offset `3461`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `VBRELDSP`

### Derived helper aliases

- `CHARIN_2CADR` -> bank `04`, offset `3311`, source `KEYRUPT,_UPRUPT.agc`, section `ACCEPTUP / 2CADR CHARIN`
- `NOVAC_NEWLOC` -> bank `02`, offset `1100`, source `EXECUTIVE.agc`, section `NOVAC / DXCH NEWLOC`

`CHARIN_2CADR` is not an Apollo label. It is a derived helper alias attached to the exact `2CADR CHARIN` request word so the runtime/debugging layer can identify that specific executive-request site honestly.

## Runtime paths currently using this map

- Native input routine entry:
  - `app/src/main/cpp/NativeApolloCore.cpp`
  - `PRO` enters exact `PROCKEY`
  - channel-15 DSKY keys now enter a partial interrupt-side path:
    - start at exact `KEYRUPT1`
    - execute exact `LODSAMPT`
    - preserve the `KEYCOM` / `ACCEPTUP` lead-in
    - execute exact `TC NOVAC`
    - capture the exact `2CADR CHARIN` request at `NOVAC / DXCH NEWLOC`
    - continue through exact `NOVAC2` / `NOVAC3` / `CORFOUND` / `SETLOC`
    - return through exact `WAITLIST` `RESUME` / `NOQRSM` / `NOQBRSM`
    - now execute the exact Apollo `RESUME` special instruction itself
    - the remaining late handoff now waits for exact Apollo `RESUME`
    - after `RESUME`, it now lets Apollo continue through the proven scheduler/job-switch slice instead of dispatching at the first scheduler label boundary
    - the remaining late dispatch now waits for exact natural Apollo transfer state:
      - `SUPDXCHZ` entry
      - `SUPDXCHZ +1` entry
    - there is no longer a separate late invocation timer after `RESUME`
    - the routed key path now keeps stepping on exact Apollo-owned state until:
      - exact natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state is reached
      - or the overall routed-step budget ends and the remaining fallback primitive is forced
    - if the overall routed-step budget ends after Apollo has already entered the exact final pre-transfer transition slice:
      - `JOBSLP1`
      - `JOBSLP2`
      - `NUCHANG2`
      - `DUMMYJOB`
      - `ADVAN`
      - `SELFBANK`
      - `NUDIRECT`
      the runtime now gives that exact Apollo slice one last continuation window to reach natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state before any forced late dispatch is allowed
    - if the requested job still has not become self-dispatching after the extended post-`RESUME` Apollo execution window, the remaining fallback now loads the Apollo-captured `NEWLOC` / `NEWLOC+1` `2CADR` request state into `A+L` and enters exact `SUPDXCHZ` instead of jumping directly to a decoded target label
    - after `SUPDXCHZ`, post-dispatch completion can now stop on exact Executive/Interpreter aftermath labels:
      - `ENDOFJOB`
      - `ENDPRCHG`
      - `TASKOVER`
      - `INTRSM`
    - only if those exact completion boundaries are not reached does the older post-target instruction budget still end the routed flow
    - after that late `2CADR`-derived dispatch, the core now waits for the Apollo-requested target bank/offset or label to become active and only then allows a smaller bounded post-target execution window
  - this is still a narrow emulator-side request-dispatch primitive wrapped around exact `SUPDXCHZ`, not a complete Apollo Executive scheduler or job switcher
- Native label lookup:
  - `app/src/main/cpp/AgcMemoryImage.cpp`
  - label -> bank/address lookup is used by the native core before routine entry
- Derived rope-label overlay:
  - `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - exact Pinball and KEYRUPT labels are added here as derived runtime metadata, not to `third_party/apollo`
- UI/source debugging:
  - `app/src/main/assets/source_annotations/source_annotations.json`
  - exact label -> bank/address -> source file/section mapping is surfaced in the engineer/source path
  - `getSnapshot()` now carries the current label and bank-local PC so the source/debug panel can resolve exact live Apollo locations

## What this improves honestly

- The runtime no longer needs to guess the entry points for `CHARIN`, `PROCKEY`, `LODSAMPT`, or `NOVAC`.
- The native core can route DSKY input through more exact Apollo interrupt/input/request labels rather than inventing more local command consequences.
- The active key-input path now exercises more real Executive aftermath after request capture before any native dispatch occurs.
- The active key-input path now exercises more of Apollo's interrupt-return preparation before the remaining dispatch.
- The active key-input path now uses more honest interrupt lead-in storage and a real Apollo `RESUME` instruction before the remaining fallback can occur.
- The remaining late dispatch is now sourced from Apollo-captured `2CADR` request words and handed to exact Apollo `SUPDXCHZ` rather than to a single hard-coded `CHARIN` target.
- The routed `NOVAC` request capture is now runtime-proven to pull the real `2CADR CHARIN` words from the exact caller site:
  - `04:3311` -> `02077`
  - `04:3312` -> `60101`
  using Apollo-correct `INDEX Q / DCA 0 / DXCH NEWLOC` semantics rather than a zeroed emulator substitute.
- Those captured words now have an exact Apollo-owned decode path in the runtime:
  - `02077` is the captured `2CADR` S-register target word
  - `60101` is the captured bank/BBCON word
  - per exact `Parse2CADR.c` semantics, `60101` carries:
    - E-bank `1`
    - fixed bank `030`
    - superbank `4`
  - so the effective switched-fixed request is `40:0077`
- The remaining post-capture observer no longer flattens that request to bank `30`; routed tracing now proves the exact post-`SUPDXCHZ` target activation at `40:0077`.
- The exact routed post-`SUPDXCHZ` aftermath is now runtime-proven to continue through the real bank-40 Pinball path:
  - `CHARIN_PREENTRY` at `40:2077`
  - `CHARIN` at `40:2100`
  - `CHARIN2` at `40:2112`
  - `ENDOFJOB` at `02:3155`
  rather than stopping at the old custom `CHARIN2` shortcut.
- The routed outer completion path now also uses only exact Apollo completion boundaries:
  - `ENDOFJOB`
  - `ENDPRCHG`
  - `TASKOVER`
  - `INTRSM`
  and no longer treats `CHARIN2` itself as a completion boundary.
- The remaining routed-step exhaustion fallback is now narrower because exact Apollo state in the final pre-transfer transition slice gets its own continuation path to natural transfer before any forced handoff is allowed.
- The current Luminary 099 erasable initializer now also carries the exact Apollo fresh-start `SELFRET` word needed by the `ADVAN -> SELFBANK -> SUPDXCHZ +1` idle/self-check dispatch path.
- The native CPU now uses one’s-complement end-around-carry arithmetic for `AD`, `ADS`, and `SU`, which is directly relevant to the routed Executive state transitions that decide whether Apollo marks `NEWJOB` and reaches the natural transfer corridor on its own.
- The native CPU now also fetches executable erasable/central instructions from actual erasable/central state instead of always substituting a rope word, which is directly relevant to exact Apollo aftermath that depends on instructions such as `TC L` after `SUPDXCHZ`.
- The source/debug path can now resolve several exact live Apollo labels to real source files and sections.

## What remains uncertain or incomplete

- This is still not a full Apollo-owned `KEYRUPT1` interrupt path.
- This is still not a full Apollo-owned `T4RUPT` proceed path.
- The emulator still lacks enough interrupt/executive/peripheral behavior for Pinball consequences to be fully Apollo-owned end-to-end after the routine entry point is reached.
- The current key path now reaches `NOVAC2` / `SETLOC`, the `WAITLIST` `RESUME` entry, the exact `RESUME` special instruction, exact scheduler/job-switch labels in bank `01`, and exact `SUPDXCHZ`.
- The current key path now also has exact proof and runtime visibility for deeper Executive aftermath labels:
  - `ENDPRCHG` at `01:2765`
  - `JOBSLP1` at `01:2776`
  - `JOBSLP2` at `01:3007`
  - `NUCHANG2` at `01:3011`
  - `TASKOVER` at `02:3261`
  - `INTRSM` at `03:2050`
- The final pre-transfer slice now also includes exact runtime visibility for:
  - `SELFBANK` at `01:3224`
- The current remaining invocation trigger is smaller than before because it now waits for exact natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state instead of dispatching at the earlier scheduler labels, and the old separate post-`RESUME` invocation timer is gone, but it is still not full Apollo job scheduling and interrupt return.
- The current remaining invocation trigger is still not gone, because the pending request is not yet proven to become self-dispatching purely from Apollo-owned scheduler state even after the arithmetic correction in this pass.
- The current remaining invocation trigger is also not yet proven gone after the executable-erasable fetch correction in this pass; no new runtime label proof is claimed yet from that semantic fix alone.
- The current remaining invocation trigger is now pinned more narrowly to exact bank-03 interpreter aftermath:
  - routed-step exhaustion fires from `03:0223`
  - `resume=no`
  - `finalSlice=no`
  - `newjob=77777`
  - `loc=00000`
  - `bankset=77777`
  - `priority=00000`
  so the active blocker is earlier than the later proven `RESUME` / scheduler / natural-transfer corridor.
- The current routed trace now proves that this visible `03:0223` stall is actually executable erasable state:
  - the remaining exact-stall detector only matches when `programCounterInErasable=yes`
  - erasable address `0223` is currently seeded in `erasable_init.cfg` as `00223`
  - that word is the seeded `VAC1ADRC` self-pointer from the custom fresh-start Executive availability chain
- The routed trace now also proves the exact lead-in to that later `0223` stop:
  - bank-03 rope aftermath executes through:
    - `03:1032`
    - `03:1033`
    - `03:1034`
    - `03:1004`
    - `03:0061`
    - `03:0062`
    - `03:0063`
    - `03:0065`
    - `03:0071`
    - `03:0072`
    - `03:0074`
  - that bank-03 path then executes a real `TC 0177`
  - execution enters executable erasable at `0177` and linearly walks erasable words until the later exact `0223` loop
- The remaining handoff trigger is now narrower than plain routed-step exhaustion:
  - when Apollo reaches that exact executable-erasable `0223` stall state, the runtime now gives it one exact continuation window first
  - only if that exact-stall continuation still does not reach `RESUME`, the final slice, or natural transfer does the remaining handoff occur
- The routed path still does not reach natural transfer on its own after that now-correct `2CADR` capture, because runtime tracing now shows exact unsupported opcode classes after the capture:
  - `0140`
  - `0124`
  these are now the narrowest proven post-capture semantic blockers ahead of natural `SUPDXCHZ` / `SUPDXCHZ +1`.
- Those exact post-capture opcode blockers are now supported:
  - `0140` -> extended `DCS`
  - `0124` -> extended `AUG`
  and the routed trace no longer reports those unsupported sites.
- The remaining forced-dispatch decoder is still custom and not yet a full Apollo-owned `2CADR` consumer, even though the captured `2CADR` words themselves are now Apollo-correct and their superbank-bearing bank word is now decoded exactly in the remaining observer/target-consumer.
- The active pre-transfer blocker is now more precise than before:
  - later exact tracing now narrows that further:
    - the routed path first drops through a real bank-03 `TC 0177`
    - then enters executable erasable core-set state at `0177`
    - the later `0223` self-loop is downstream of that earlier core-set drop
  - the unresolved gap is not generic “more Executive work”
  - it is the exact executable-erasable `VAC1ADRC` stall path and the Apollo-owned return/transfer behavior or erasable state that should keep that path moving toward `RESUME` / `SUPDXCHZ`
- A stronger local alignment check now exists in:
  - `third_party/_derived_tools/luminary099_executive_alignment_check.txt`
  - it confirms exact:
    - `CHANJOB` at `01:2706`
    - `ENDPRCHG` at `01:2765`
    - `JOBSLP1` at `01:2776`
    - `JOBSLP2` at `01:3007`
    - `NUCHANG2` at `01:3011`
    - `DUMMYJOB` at `01:3206`
    - `ADVAN` at `01:3214`
    - `NUDIRECT` at `01:3225`
    - `SUPDXCHZ` at `02:3165`
    - `INTRSM` at `03:2050`
- A derived local `yaYUL` build path now exists and can produce `third_party/_derived_tools/yaYUL.exe` from a copied upstream source tree without modifying Apollo artifacts.
- That still does not finish the proof path, because Windows currently denies execution of that derived helper from this workspace with `Access is denied`, so no trustworthy Luminary 099 `.lst` has been emitted yet.
- The active Luminary 099 erasable image is still a custom initializer. It now seeds only the Executive fresh-start words derived from Apollo source that are needed for this narrow path.
- Some older overlay labels are still helper aliases rather than exact Apollo labels. Those should remain clearly marked as derived aliases, not historical labels.

- 2026-04-24 clarification for the active pre-transfer blocker:
  - `0177` is dynamic core set 1 `MODE`
  - `0200` is dynamic core set 1 `LOC`
  - `0201` is dynamic core set 1 `BANKSET`
  - `0202` is dynamic core set 1 `PUSHLOC`
  - `0203` is dynamic core set 1 `PRIORITY`
  - the remaining forced-handoff trigger is now keyed to that exact core-set-drop origin rather than only the later `0223` symptom
  - exact special transfer-pair semantics are also now aligned with Apollo:
    - `DTCB = DXCH Z` uses `Z/BBANK`
    - `DTCF = DXCH FBANK` uses `FBANK/Z`
  - Apollo source comments further pin the remaining restore path to:
    - `CHANJOB` at `01:2706`
    - `ENDPRCHG` at `01:2765`
    - `INTRSM` at `03:2050`
  - those are now the exact Apollo-owned replacement targets for the blocked `TC 0177 -> dynamic core set 1 MODE` transition

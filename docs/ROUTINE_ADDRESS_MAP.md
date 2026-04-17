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
- `NUCHANG2` -> bank `01`, offset `3011`, source `EXECUTIVE.agc`, section `NUCHANG2`
- `DUMMYJOB` -> bank `01`, offset `3206`, source `EXECUTIVE.agc`, section `DUMMYJOB`
- `ADVAN` -> bank `01`, offset `3214`, source `EXECUTIVE.agc`, section `ADVAN`
- `NUDIRECT` -> bank `01`, offset `3225`, source `EXECUTIVE.agc`, section `NUDIRECT`
- `SUPDXCHZ` -> bank `02`, offset `3165`, source `EXECUTIVE.agc`, section `SUPDXCHZ`
- `INTRSM` -> bank `03`, offset `2050`, source `INTERPRETER.agc`, section `INTRSM`
- `RESUME` -> bank `02`, offset `3270`, source `WAITLIST.agc`, section `RESUME`
- `NOQRSM` -> bank `02`, offset `3272`, source `WAITLIST.agc`, section `NOQRSM`
- `NOQBRSM` -> bank `02`, offset `3274`, source `WAITLIST.agc`, section `NOQBRSM`
- `UPOK` -> bank `04`, offset `3350`, source `KEYRUPT,_UPRUPT.agc`, section `UPOK`
- `HI10` -> bank `04`, offset `3374`, source `KEYRUPT,_UPRUPT.agc`, section `HI10`
- `ELRCODE` -> bank `04`, offset `3377`, source `KEYRUPT,_UPRUPT.agc`, section `ELRCODE`
- `CHARIN` -> bank `40`, offset `2100`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `CHARIN`
- `CHARIN2` -> bank `40`, offset `2112`, source `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`, section `CHARIN2`
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
    - after `RESUME`, it now prefers exact scheduler/job-switch labels:
      - `CHANJOB`
      - `ADVAN`
      - `NUDIRECT`
    - only if those proven scheduler boundaries are not reached in time does it fall back to the bounded post-`RESUME` timer
    - if the requested job still has not become self-dispatching after the extended post-`RESUME` Apollo execution window, the remaining fallback now loads the Apollo-captured `NEWLOC` / `NEWLOC+1` `2CADR` request state into `A+L` and enters exact `SUPDXCHZ` instead of jumping directly to a decoded target label
    - after `SUPDXCHZ`, post-dispatch completion can now stop on exact Executive/Interpreter aftermath labels:
      - `ENDPRCHG`
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
- The source/debug path can now resolve several exact live Apollo labels to real source files and sections.

## What remains uncertain or incomplete

- This is still not a full Apollo-owned `KEYRUPT1` interrupt path.
- This is still not a full Apollo-owned `T4RUPT` proceed path.
- The emulator still lacks enough interrupt/executive/peripheral behavior for Pinball consequences to be fully Apollo-owned end-to-end after the routine entry point is reached.
- The current key path now reaches `NOVAC2` / `SETLOC`, the `WAITLIST` `RESUME` entry, the exact `RESUME` special instruction, exact scheduler/job-switch labels in bank `01`, and exact `SUPDXCHZ`.
- The current key path now also has exact proof and runtime visibility for deeper Executive aftermath labels:
  - `ENDPRCHG` at `01:2765`
  - `NUCHANG2` at `01:3011`
  - `INTRSM` at `03:2050`
- The current remaining trigger is smaller than before, but it is still not full Apollo job scheduling and interrupt return.
- A stronger local alignment check now exists in:
  - `third_party/_derived_tools/luminary099_executive_alignment_check.txt`
  - it confirms exact:
    - `CHANJOB` at `01:2706`
    - `ENDPRCHG` at `01:2765`
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

# Routine Address Map

## Scope

This file documents the exact Apollo routine addresses currently derived and used for Luminary 099 DSKY/input work.

It is not a claim of full Pinball ownership.
It is not a claim of full interrupt ownership.
It is a record of exact bank-local addresses derived from untouched Apollo artifacts and then used by the emulator/runtime around those artifacts.

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
    - only fall back to exact `CHARIN` later if the requested job still has not become self-dispatching after the extended post-`RESUME` Apollo execution window
  - this is still a narrow native request fallback, not a complete Apollo Executive scheduler or job switcher
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
- The source/debug path can now resolve several exact live Apollo labels to real source files and sections.

## What remains uncertain or incomplete

- This is still not a full Apollo-owned `KEYRUPT1` interrupt path.
- This is still not a full Apollo-owned `T4RUPT` proceed path.
- The emulator still lacks enough interrupt/executive/peripheral behavior for Pinball consequences to be fully Apollo-owned end-to-end after the routine entry point is reached.
- The current key path now reaches `NOVAC2` / `SETLOC`, the `WAITLIST` `RESUME` entry, and the exact `RESUME` special instruction, but it still uses a narrow later fallback dispatch instead of full Apollo job scheduling and interrupt return.
- The active Luminary 099 erasable image is still a custom initializer. It now seeds only the Executive fresh-start words derived from Apollo source that are needed for this narrow path.
- Some older overlay labels are still helper aliases rather than exact Apollo labels. Those should remain clearly marked as derived aliases, not historical labels.

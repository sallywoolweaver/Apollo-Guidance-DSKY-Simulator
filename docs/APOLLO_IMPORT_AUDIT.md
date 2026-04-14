# Apollo Import Audit

Date: 2026-04-14

## Full upstream git clones present

- `third_party/apollo/upstream/virtualagc/`
  - full git clone of the Virtual AGC repository
  - role: authoritative untouched upstream source base for Apollo AGC families and tooling
- `third_party/apollo/upstream/Apollo-11/`
  - full git clone of the `chrislgarry/Apollo-11` repository
  - role: secondary Apollo 11 source reference

These clones are separate from the mission-family import folders and do not replace the runtime asset separation rules.

## Exact Apollo artifacts present in repo

### Apollo 11 LM `Luminary099`

- Path: `third_party/apollo/apollo11/lm/luminary099/`
- Source family imported intact from Virtual AGC `Luminary099`
- File count: `101`
- Folder count: `1`
- Key filenames:
  - `MAIN.agc`
  - `AP11ROPE.binsource`
  - `Luminary099.binsource`
  - `DISPLAY_INTERFACE_ROUTINES.agc`
  - `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
  - `THE_LUNAR_LANDING.agc`
  - `LUNAR_LANDING_GUIDANCE_EQUATIONS.agc`
  - `Makefile`
- Untouched/read-only status: yes

### Apollo 11 CM `Comanche055`

- Path: `third_party/apollo/apollo11/cm/comanche055/`
- Source family imported intact from Virtual AGC `Comanche055`
- File count: `92`
- Folder count: `1`
- Key filenames:
  - `MAIN.agc`
  - `Comanche055.binsource`
  - `DISPLAY_INTERFACE_ROUTINES.agc`
  - `EXECUTIVE.agc`
  - `P61-P67.agc`
  - `REENTRY_CONTROL.agc`
  - `Makefile`
- Untouched/read-only status: yes

### Apollo 13 LM `LM131R1`

- Path: `third_party/apollo/apollo13/lm/lm131r1/`
- Source family imported intact from Virtual AGC `LM131R1`
- File count: `95`
- Folder count: `1`
- Key filenames:
  - `MAIN.agc`
  - `Luminary131PlusLM131R1ModuleDump.bin`
  - `LM131R1.buggers`
  - `DISPLAY_INTERFACE_ROUTINES.agc`
  - `THE_LUNAR_LANDING.agc`
  - `LUNAR_LANDING_GUIDANCE_EQUATIONS.agc`
  - `Makefile`
- Untouched/read-only status: yes

## Runtime asset separation

- `third_party/apollo/...`
  - untouched imported historical artifacts only
- `app/src/main/assets/programs/...`
  - runtime program assets only
  - Apollo 11 LM now includes `AP11ROPE.binsource`
  - Apollo 11 CM now includes `Comanche055.binsource`
- `app/src/main/assets/bootstrap/...`
  - custom bootstrap/scenario data only
- `app/src/main/assets/source_annotations/...`
  - annotation/index data only

## What is still missing

- No broader Apollo-family import matrix beyond these three families.
- No active CM runtime path.
- No active Apollo 13 LM runtime path.
- No historically validated Apollo 11 LM erasable/boot-state package in the active path.
- Apollo 11 LM uses `AP11ROPE.binsource` as the imported rope artifact; the app converts it into rope bytes at load time rather than importing a prebuilt binary file for that family.

## Bottom line

- `third_party/apollo/` is now a real import area for Apollo 11 LM `Luminary099`, Apollo 11 CM `Comanche055`, and Apollo 13 LM `LM131R1`.
- `third_party/apollo/upstream/` now also contains actual git-cloned upstream repositories rather than only copied snapshots.
- It is still incomplete as a broader Apollo archive, but it is no longer mostly placeholder content for these imported families.

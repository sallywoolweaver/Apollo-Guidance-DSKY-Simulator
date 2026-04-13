# Apollo Guidance DSKY Simulator

Apollo Guidance DSKY Simulator is a native Android Kotlin + Jetpack Compose app built around a native C++ core behind JNI. The current build preserves an emulator-oriented architecture and one runnable Apollo 11 LM vertical slice, but it still does not execute original Apollo software strongly enough to claim true Apollo runtime execution.

## How to run

1. Open the project in Android Studio.
2. Sync Gradle.
3. Run the `app` configuration on an emulator or device running Android 12L / API 31 or later.

## Architecture summary

- `app`: activity and view-model shell.
- `core`: JNI-facing Kotlin snapshot model and native bridge wrapper.
- `app/src/main/cpp`: native emulator-core / companion simulation loop.
- `app/src/main/assets/programs/...`: Apollo-derived rope/program assets plus erasable initializers.
- `app/src/main/assets/bootstrap/...`: custom bootstrap/scenario helper data only.
- `app/src/main/assets/source_annotations/...`: extracted annotation indexes only.
- `third_party/apollo/...`: read-only Apollo provenance/import area.
- `dsky`: DSKY display model used by the Compose shell.
- `ui`: Compose screens, panels, and theme.
- `docs`: authenticity boundary, source mapping, assumptions, and verification backlog.

## Native core API

- `initCore()`
- `loadProgramImage(...)`
- `resetScenario()`
- `pressKey(...)`
- `stepSimulation(...)`
- `getSnapshot()`

The native core is treated as the computer-side runtime. The Compose layer only renders the snapshot, routes keypresses, and manages shell-level screens.

## Current program asset layout

- Apollo-derived program data:
  - `programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - `programs/apollo11/cm/comanche055/rope_excerpt.cfg`
- Custom erasable initializers:
  - `programs/apollo11/lm/luminary099/erasable_init.cfg`
  - `programs/apollo11/cm/comanche055/erasable_init.cfg`
- Custom bootstrap overlays:
  - `bootstrap/apollo11/lm/luminary099/bootstrap.cfg`
  - `bootstrap/apollo11/cm/comanche055/bootstrap.cfg`

These are structurally separated on disk. They are still not full rope images or proof of original Apollo software execution.

## Feature slice

- Native C++ emulator-core / companion simulation architecture via Android NDK and JNI.
- Apollo 11 LM vertical slice that loads separated rope-excerpt, erasable-init, and bootstrap assets through the native core.
- Mission/software family setup screen with explicit support/authenticity labels.
- DSKY-like program / verb / noun / register presentation.
- Touch keypad with `VERB`, `NOUN`, `CLR`, `ENTR`, `PRO`, `KEY REL`, and `RSET`.
- Working command-entry support for `V16 N68`, `V06 N63/N64/N60`, `V05 N09`, and `V37 N63`.
- Fuel, altitude, vertical velocity, and horizontal velocity updates.
- 1201 / 1202-style overload alarms with acknowledgement flow.
- Debrief outcome classification for safe landing, hard landing, crash, and fuel exhaustion.
- Engineer Mode source panel with curated Luminary099 routine/file/comment annotations.
- Source Browser screen for browsing the initial annotation set.

## Design philosophy

1. Realism first.
2. Clarity second.
3. Mobile usability third.
4. Spectacle last.

## Realism disclaimer

This build is not a full AGC emulator. Program numbers, DSKY framing, and some command/peripheral behavior are now native-core owned, and the CPU path fetches and steps over Apollo-derived rope-word excerpts. But mission progression, alarms, descent physics, and outcomes are still dominated by a temporary compatibility layer rather than original Apollo software execution.

## Source influence

Primary historical influence comes from the Apollo 11 `Luminary099` code and Virtual AGC documentation, especially the DSKY display/interface routines, lunar landing guidance tables, ignition/phase structure, and Don Eyles's 1201/1202 alarm analysis. See `SOURCE_MAPPING.md`.

## Source annotation extraction

- `tools/extract_source_annotations.py` provides a simple local extraction utility for Apollo AGC source trees.
- `app/src/main/assets/source_annotations/source_annotations.json` is the current curated annotation asset used by the app.
- Annotation mapping tiers are explicit in the UI:
  - currently executing
  - related to current phase
  - historical background only
- The current build only uses contextual mappings; it does not claim exact live routine execution.

## Limitations

- No true AGC instruction-level emulation yet.
- No full assembled Apollo rope image is executed yet.
- No full training mode, free simulator mode, or museum mode yet.
- No source-linked in-app document browser yet.
- Alarm generation and guidance behavior are native abstractions, not a line-by-line reproduction of the flight software.
- Apollo 11 CM, Apollo 13 LM, and rendezvous-family entries are selection scaffolding only and are not executable scenarios yet.

## Future realism upgrades

- Replace the current native companion model with a true AGC-emulator-backed execution core.
- Replace scripted overload pressure with deeper executive / waitlist scheduling models.
- Add source-traceable DSKY display routines and additional verbs/nouns.
- Expand to tutorial, free simulator, and reference modes on the same simulation contracts.

## Migration roadmap toward Luminary-derived execution

1. Keep the existing JNI contract and Compose shell stable.
2. Replace rope excerpts with full rope-image loading while keeping erasable and bootstrap data separate.
3. Move mission-step advancement from `CompatibilityScenario` into AGC-driven execution and peripheral state updates.
4. Relegate remaining landing/gameplay-only values to a thin translation layer derived from emulator state.
5. Remove compatibility-only alarm synthesis as EXECUTIVE/WAITLIST-backed behavior becomes available.

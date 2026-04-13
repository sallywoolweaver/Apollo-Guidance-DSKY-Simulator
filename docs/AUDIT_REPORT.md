# Audit Report

## Current architecture summary

- Android shell:
  - Kotlin + Jetpack Compose
  - MVVM-style state holder in `ApolloSimViewModel`
  - UI renders a coarse snapshot returned from JNI
- Native layer:
  - C++ core built with NDK + CMake
  - JNI bridge is centralized and coarse-grained
  - native code is split into CPU-like, memory-image, DSKY I/O, compatibility-scenario, and snapshot-builder subsystems
- Data/authenticity overlay:
  - source-annotation asset and browser
  - curated mapping layer for Engineer Mode

## What the current app truly is

- It is not a strict AGC emulator.
- It is not executing Luminary instructions from a real assembled rope image.
- It is an emulator-oriented Android shell with a custom native compatibility simulator behind a stable JNI interface.
- It now loads small Apollo-derived rope-word excerpts into the native memory-image path, but those words are not yet the primary driver of runtime behavior.
- The architecture is genuinely compatible with a future emulator-backed core, but the present behavior is still mostly custom simulation logic.

## Native core authenticity boundary

### Strict assessment

- `AgcCpu`:
  - not an instruction-level AGC CPU
  - currently an instruction-step skeleton with limited opcode-class ownership
  - this is scaffolding, not emulator execution
- `AgcMemoryImage`:
  - now loads Apollo-derived rope-word excerpts and separate erasable initializers through a distinct native path
  - custom bootstrap data is stored separately under `app/src/main/assets/bootstrap/...`
  - this is progress toward real mission-software loading, but it is not yet equivalent to loading and running a full rope image
- `DskyIo`:
  - contains a custom DSKY interaction state machine inspired by Pinball/Display Interface terminology
  - this is source-informed, but not source-faithful peripheral emulation
- `CompatibilityScenario`:
  - owns the actual descent, alarm, and phase logic that makes the app work
  - this is a custom simulator with Apollo framing
  - it is currently the functional heart of the app
- `SnapshotBuilder`:
  - technically clean and appropriate
  - mostly a presentation assembly layer

### Classification of current native behavior

- Source-derived now:
  - major program identifiers `P63`, `P64`, `P66`, `P68`
  - DSKY vocabulary such as verb/noun/program/`PRO`/`KEY REL`/`RSET`
  - alarm-family naming for `1201`/`1202`
  - source-annotation content and file/routine naming
  - small bundled rope-word excerpts from `Luminary099` and `Comanche055` listings
- Inspired-by-source now:
  - DSKY command-entry flow
  - phase progression across braking/approach/landing
  - overload/alarm framing around EXECUTIVE-style overload conditions
  - source panel mapping to current phase or alarm context
- Custom approximations now:
  - all powered-descent kinematics
  - all load-ratio calculations
  - all alarm triggering thresholds and timing windows
  - landing classification thresholds
  - scenario bootstrap and mission-state progression
- Pure scaffolding now:
  - AGC CPU state object
  - AGC-like memory image arrays
  - some of the subsystem split itself, which exists mainly to prepare for stricter execution later

### Bottom-line classification

- The current native core is best described as:
  - an AGC-compatible shell plus a custom native simulator
- It is not honestly describable as:
  - a strict AGC instruction-level emulator
- Calling it merely an "emulator core" without qualification would be misleading.

## Source mapping quality

### What is strong

- The docs reference real, relevant Luminary source areas:
  - `PINBALL_GAME_BUTTONS_AND_LIGHTS.agc`
  - `DISPLAY_INTERFACE_ROUTINES.agc`
  - `THE_LUNAR_LANDING.agc`
  - `LUNAR_LANDING_GUIDANCE_EQUATIONS.agc`
  - `BURN, BABY, BURN -- MASTER IGNITION ROUTINE.agc`
  - `WAITLIST.agc`
  - `RESTARTS_ROUTINE.agc`
  - `ALARM_AND_ABORT.agc`
  - `PHASE_TABLE_MAINTENANCE.agc`
  - `RADAR_LEADIN_ROUTINES.agc`
- The docs also correctly identify Virtual AGC/yaAGC/yaDSKY as direction-setting references rather than flight-simulation logic.

### What is still weak or vague

- The mapping still mixes "this file informed the app" with "this file is a likely future implementation anchor".
- It does not yet separate:
  - current code that actually uses a source-driven behavior
  - source areas that are only roadmap targets
- It still lacks a mission/software-family matrix for LM/CM variants and their evidence quality.

## JNI/API architecture

### Strengths

- The coarse API is technically sound:
  - `initCore()`
  - `loadProgramImage(...)`
  - `resetScenario()`
  - `pressKey(...)`
  - `stepSimulation(...)`
  - `getSnapshot()`
- The API is stable enough for a stricter emulator-backed implementation later.
- The UI is not tightly coupled to native internal classes.

### Weaknesses

- `loadProgramImage(...)` is overloaded conceptually:
  - today it loads a custom scenario/control image
  - long term it should load a real assembled program image plus possibly scenario boot data
- `getSnapshot()` is still partially scenario-derived rather than emulator-derived.

### Recommendation

- Keep the current JNI API.
- Do not add many new JNI entry points.
- If a small extension is needed, prefer metadata-style queries such as software-variant listing rather than low-level execution calls.

## UI authenticity

### What is honest

- The UI generally avoids claiming exact Apollo reproduction.
- Engineer Mode source-note tiers are explicit and mostly honest.
- The DSKY wording is framed as Apollo-inspired/source-informed.

### What still risks overclaiming

- Earlier surface wording like "LMY-99 DSKY" read more exact than the runtime deserved. That branding has been removed.
- The simulator screen still feels more Apollo-specific than the runtime actually is, because the underlying native logic is custom.
- The source browser makes authenticity stronger, but could be misread as live routine tracing if the tier labels were removed or softened.

## Scenario image / control image

- The previous `apollo11_descent_image.cfg` was only a custom native scenario/control image.
- The current Apollo 11 LM asset set is split into:
  - `app/src/main/assets/programs/apollo11/lm/luminary099/rope_excerpt.cfg`
  - `app/src/main/assets/programs/apollo11/lm/luminary099/erasable_init.cfg`
  - `app/src/main/assets/bootstrap/apollo11/lm/luminary099/bootstrap.cfg`
- This split removes the old on-disk hybrid ambiguity.
- It is still not a full rope image and not a proof of Apollo software execution.

### Current split asset format

- Apollo-derived fields now:
  - `rope_word=bank:offset:word[:label]`
  - these are sourced from Apollo listings and loaded into native rope memory
- Custom erasable-init fields now:
  - `erasable_word=address:word[:label]`
  - these are kept separate from both rope data and bootstrap data
- Custom bootstrap fields now:
  - `requested_initial_program`
  - `initial_altitude_m`
  - `initial_vertical_velocity_mps`
  - `initial_horizontal_velocity_mps`
  - `initial_fuel_kg`
  - `initial_throttle`
- Metadata fields now:
  - `image_type`
  - `source_name`
  - `source_kind`

### Target full-image path

- Full software image:
  - assembled rope image or binary derived from Apollo source/binsource
- Erasable-memory initialization:
  - explicit erasable state block or scenario bootstrap block kept separate from the rope image
- Optional scenario bootstrap:
  - only for non-flight-sourced initial conditions or teaching overlays
- The current split excerpt/init/bootstrap format is transitional and should not be mistaken for a final Apollo program-image format.

## Documentation quality

### Strong points

- The docs do not currently claim strict AGC execution.
- Authenticity boundaries are already explicit.
- The source-annotation limitations are documented.

### Gaps

- There is not yet a hard software-family support matrix covering LM and CM candidates.
- `SOURCE_MAPPING.md` should explicitly distinguish:
  - source-backed current behavior
  - roadmap-target source areas
- `SOURCES_TO_VERIFY.md` needs software-family availability and support-status questions.

## What is technically strong

- The Android/Compose shell is cleanly decoupled from native internals.
- The JNI surface is centralized and restrained.
- The native subsystem split is the right direction for future emulator work.
- The source-annotation system is layered on top rather than entangled with execution logic.
- The app remains shippable and builds cleanly while evolving.

## What is misleading or too vague

- "Emulator core" is too flattering unless qualified with "emulator-oriented" or "AGC-compatible shell".
- The current CPU and memory-image layers look more mature than they functionally are.
- The current scenario image path can be mistaken for software loading unless explicitly described as companion/bootstrap data.

## What must change to get closer to a strict emulator-backed build

1. Replace config-style program loading with real assembled AGC image loading.
2. Move runtime ownership from `CompatibilityScenario` into emulator/peripheral state.
3. Make DSKY behavior increasingly peripheral-driven rather than custom UI-state-driven.
4. Derive snapshot fields from emulator state wherever possible.
5. Restrict the compatibility layer to:
   - scenario bootstrap
   - UI-friendly derived values
   - non-AGC mission/game framing that genuinely cannot come from the emulator

## Prioritized roadmap

### Phase 2

1. Add audit-driven software-family scaffolding with explicit support levels.
2. Keep JNI stable and preserve current build quality.
3. Separate "software image" from "scenario bootstrap" in the design and docs.
4. Add LM/CM/software-family selection infrastructure without claiming execution support where none exists.

### Phase 3

1. Implement a real assembled-image loading path for at least one Apollo software family.
2. Introduce opcode-/cycle-driven native execution ownership.
3. Start replacing compatibility alarm/phase behavior with emulator-driven state.
4. Promote source annotation tiers only when runtime mapping becomes honest.

### Phase 4

1. Add a true mission/software matrix:
   - Apollo 11 LM `Luminary 099/1`
   - Apollo 11 CM `Comanche 055`
   - Apollo 13 LM `LM131 rev 1` / related reconstructed chain
2. Decide whether CM and rendezvous scenarios are:
   - source-backed execution
   - source-informed companion layers
   - planned only

## Best next historically grounded expansion

- The best next expansion is not "more gameplay".
- It is software-family scaffolding plus real program-image handling groundwork, starting with:
  - Apollo 11 LM `Luminary 099/1`
  - Apollo 11 CM `Comanche 055`
- Those are strong next candidates because the source status is comparatively well documented by Virtual AGC.

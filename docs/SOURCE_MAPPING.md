# Source Mapping

## Sources inspected

### Apollo 11 source repository

- `chrislgarry/Apollo-11`
- Top-level focus: `Luminary099`
- Practical detailed inspection path: the Virtual AGC hyperlinked listings for the same LM source corpus

### Virtual AGC materials

- Luminary overview page: `https://www.ibiblio.org/apollo/Luminary.html`
- Colossus overview page: `https://www.ibiblio.org/apollo/Colossus.html`
- Hyperlinked AGC listings for LM source files
- Hyperlinked AGC listings for CM source files such as `Comanche055/MAIN.agc`
- Don Eyles / Cherry alarm analysis: `CherryApollo11Exegesis.pdf`
- LM guidance/program documentation references surfaced from the Virtual AGC document library

## Relevant source areas identified

### DSKY input / display behavior

- `DISPLAY_INTERFACE_ROUTINES.agc`
  - Informs display priority, normal vs priority displays, and astronaut-facing display flow.
- `PINBALL_GAME_BUTTONS_AND_LIGHTS.agc`
  - Most relevant file family for keypad semantics, light handling, and DSKY-facing interaction behavior.
- `ASSEMBLY_AND_OPERATION_INFORMATION.agc`
  - Informs terminology and the operational framing of verbs, nouns, and AGC usage.

### Mission-phase and landing-program structure

- `THE_LUNAR_LANDING.agc`
  - Relevant to the high-level landing sequence and powered-descent control flow.
- `LUNAR_LANDING_GUIDANCE_EQUATIONS.agc`
  - Identifies the landing flight-sequence tables and the phase mapping used around braking, approach, and vertical/landing guidance.
  - Shows source references such as `STARTP64`, `P65START`, and the `V06N63`, `V06N64`, `V06N60` symbols.
- `BURN, BABY, BURN -- MASTER IGNITION ROUTINE.agc`
  - Informs the ignition / program-entry framing around `P63`.
- `PHASE_TABLE_MAINTENANCE.agc`
  - Relevant to restartable phase management and program resumption structure.
- `RADAR_LEADIN_ROUTINES.agc`
  - Relevant to descent/radar coupling and landing-radar-related flow during powered descent.

### Alarm behavior

- `CherryApollo11Exegesis.pdf`
  - Explains that Apollo 11 experienced five 1201/1202 executive-overflow alarms.
  - Explains the executive / coreset / VAC framing, restart behavior, and why overload alarms could occur during landing.
- `WAITLIST.agc`
  - Relevant because landing jobs were scheduled through waitlist/task mechanisms that shaped overload and restart behavior.
- `RESTARTS_ROUTINE.agc`
  - Relevant because 1201/1202 recovery involved restart behavior and queue cleanup.
- `ALARM_AND_ABORT.agc`
  - Relevant to the broader LM alarm/abort control path and operator-facing fault handling.

### Program reference material

- Virtual AGC LM documentation and the program-description references linked from the Luminary page
  - Confirms that LM landing programs include `P63`, `P64`, `P65`, `P66`, and `P68`.
- Virtual AGC repository areas
  - `yaAGC`: CPU/emulator execution reference direction.
  - `yaDSKY` and related peripherals: DSKY/peripheral emulation direction.
  - Virtual AGC README guidance that Virtual AGC is a computer/peripheral emulator rather than a complete lander simulation.

## Mission/software family availability notes

- Apollo 11 LM:
  - `Luminary099` is clearly available as source/listings in Virtual AGC and is the current primary historical basis for this app.
- Apollo 11 CM:
  - `Comanche055` is clearly available as source/listings in Virtual AGC.
  - `Comanche055/MAIN.agc` identifies it as Apollo 11 CM software.
- Apollo 13 LM:
  - the Luminary page documents `LM131 rev 1` / `131/1` as the final Apollo 13 LM flight software family and states that source and binary are available in Virtual AGC.
- Rendezvous-focused next candidates:
  - CM software families such as `Comanche055` and related Colossus/Comanche material are stronger immediate candidates than inventing a custom rendezvous stack because the source provenance is clearer.

## What each source informed in the app

- `P63`, `P64`, `P66`, `P68` naming: directly source-derived.
- DSKY verb/noun/program framing: directly source-derived terminology, behaviorally approximated implementation.
- Native-core architecture choice:
  - not source-derived from Apollo itself
  - chosen as an emulator-compatible boundary so AGC execution can later live in NDK code rather than being reimplemented in Kotlin by hand.
- Native phase-2 refactor:
  - `AgcCpu`, `AgcMemoryImage`, `DskyIo`, and `CompatibilityScenario` now separate CPU/execution state, memory/program-image state, DSKY I/O state, and approximation-specific mission logic.
  - This split is an engineering boundary for future emulator integration rather than a claim that the current native code is executing Luminary instructions.
- Mission/software selection scaffolding:
  - Apollo 11 LM, Apollo 11 CM, Apollo 13 LM, and rendezvous-family entries are now modeled as availability/support metadata.
  - only the Apollo 11 LM slice is currently runnable.
- `V16 N68`, `V06 N63/N64/N60`, `V05 N09`, `V37 N63` support:
  - historically plausible command framing
  - exact command effects in this app are approximated and limited to the slice.
- 1201 / 1202 alarms:
  - historically grounded alarm family
  - current trigger logic is a source-informed abstraction, not a reproduction of EXECUTIVE/WAITLIST scheduling.
- descent phase transitions:
  - source-derived phase naming
  - mobile slice uses simplified thresholds for phase changes.
- program image handling:
  - current `loadProgramImage(...)` path packages separate rope, erasable, and bootstrap assets into one JNI call
  - Apollo-derived rope excerpts are kept separate on disk from custom bootstrap/scenario files
  - it is not yet loading and executing a full Luminary rope image with AGC opcode semantics.
- execution ownership transition:
  - current native CPU now performs an instruction-step skeleton over loaded rope words and owns major-mode/program state
  - this is still not enough to claim Apollo software execution because mission outcomes remain compatibility-driven.

## Behaviors still requiring verification

- Exact Apollo 11 LM DSKY register contents during each descent phase.
- Exact operational handling of overload alarms in this specific UI flow.
- A tighter mapping from `Luminary099` landing displays to the current register groupings.
- Whether current phase thresholds should be replaced by a more source-backed guidance-state transition model.
- Which subset of Luminary/yaAGC state should become the stable source for the current `getSnapshot()` contract.
- How to map a real rope image plus erasable state into the current Android-facing scenario boot flow.
- Which runtime state would justify promoting a source annotation from "related to current phase" to "currently executing".
- Which exact CM family is the best first rendezvous-focused target for this app after Apollo 11 LM.
- Whether the app should model Apollo 13 LM as `LM131 rev 1` terminology everywhere rather than the looser `Luminary 131/1` shorthand.

## Notes on implementation honesty

- The repository structure and Virtual AGC listing corpus were inspected before writing the simulation contracts.
- This app does not claim instruction-level AGC reproduction.
- The current native core is emulator-compatible in architecture, but it still loads rope excerpts rather than full Apollo rope binaries.
- The current native compatibility layer intentionally isolates descent/gameplay approximations so they can be removed or replaced as stricter AGC execution becomes practical.
- The source annotation asset is a curated subset focused on DSKY, alarms, ignition, and descent flow rather than a complete Luminary index.
- Every major subsystem is labeled in the surrounding docs as source-derived, approximated, or a mobile concession.

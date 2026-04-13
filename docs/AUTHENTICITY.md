# Authenticity Boundaries

## Apollo Artifact Rule

- Apollo source code, listings, and imported Apollo-derived program artifacts must remain untouched.
- The project must not rewrite, normalize, patch, paraphrase, or "improve" Apollo code.
- Authenticity gains must come from better emulator/peripheral execution around Apollo artifacts, not from rewriting those artifacts into app-owned logic.

## Exact reproductions in this slice

- Apollo program-number framing for `P63`, `P64`, `P66`, and `P68`.
- DSKY interaction vocabulary: verb, noun, program, `PRO`, `KEY REL`, `RSET`.
- 1201 / 1202 overload alarm family and their association with executive overflow conditions.

## Close approximations

- DSKY command-entry state machine and invalid-entry handling inside the native core.
- Program phase progression from braking to approach to landing.
- Overload pressure model shaped by descent phase, display use, input activity, and source-inspired event windows.

## Simplified abstractions

- The native core currently runs a documented vertical-slice flight model, not Luminary's full programmed guidance equations.
- Touch throttle trim is a mobile usability concession and not an authentic DSKY control path.
- Register content is adapted to keep key landing data visible on a phone-sized display.

## Current Native Core vs Strict AGC Target

- Current native core:
  - uses AGC-like CPU/program-image/DSKY/state boundaries
  - preserves a stable emulator-style JNI surface
  - can now load small Apollo-derived rope-word excerpts into the native memory-image path
  - still executes a compatibility scenario layer rather than AGC instructions from a full Luminary rope image
- Strict AGC target:
  - CPU state advancement driven by AGC instruction execution
  - Luminary-derived rope image and erasable state loaded into the native core
  - DSKY behavior and many alarm/display interactions sourced more directly from AGC peripheral behavior rather than a custom compatibility loop
- Remaining divergence points:
  - powered-descent kinematics are still a compatibility model
  - overload alarms are still synthesized from a source-informed load model
  - snapshot fields are still partly scenario-derived rather than entirely emulator-derived
  - bundled Apollo-derived words are not yet being executed as original Apollo behavior
  - current instruction stepping fetches loaded rope words and classifies opcode skeletons, but does not yet control mission outcomes

## Source Annotation Layer

- Exact:
  - file names, routine names, and original comment text included in `source_annotations/source_annotations.json` are preserved from Apollo source material where extracted or manually curated from the source listings.
- Contextual:
  - the current in-app Engineer Mode does not claim exact live routine execution unless the runtime can expose that honestly.
  - current simulator-phase mappings are marked as related/current-background context rather than exact execution bindings.
- Separation:
  - source annotations are an authenticity overlay only; they are not used to drive native execution.

## Scenario / Software Authenticity Tiers

- `Exact source-backed execution`
  - reserved for future cases where the app can honestly say the native core is executing original Apollo software behavior directly enough to justify the claim.
- `Source-informed execution with companion translation`
  - current Apollo 11 LM vertical slice, even with Apollo-derived rope-word excerpts loaded.

## Snapshot Field Ownership

- Emulator-derived now:
  - `program`
  - `verb`
  - `noun`
  - `phaseProgram`
  - DSKY display-mode effects for `V16 N68`, `V06 N63/N64/N60`, `V05 N09`
  - `PRO`, `RSET`, `KEY REL`, and verb/noun entry handling
  - DSKY `COMP ACTY` annunciation during Luminary099 rope execution
  - DSKY `OPR ERR` annunciation when native CPU execution reaches unsupported Apollo-derived opcode classes
  - 1201 / 1202 alarm ownership and alarm-display gating through `AlarmExecutive`
- Derived from emulator state now:
  - major-mode ownership via `AgcCpu`
  - program display strings routed through `DskyIo`
  - execution-status text such as `EXEC KEYRUPT1` routed from native CPU rope-label tracking through `DskyIo`
  - unsupported-opcode status text such as `UNSUPPORTED KEYRUPT1` routed from native CPU execution state through `DskyIo`
  - 1201 / 1202 alarm selection derived from native CPU unsupported-opcode pressure plus native major-mode/display-mode state
- Compatibility fallback now:
  - `register1`
  - `register2`
  - `register3`
  - `phaseLabel`
  - `statusLine` when written by the scenario layer for non-execution events
  - `activeAlarmCode`
  - `activeAlarmTitle`
  - `altitudeMeters`
  - `verticalVelocityMps`
  - `horizontalVelocityMps`
  - `fuelKg`
  - `throttle`
  - `targetVerticalVelocityMps`
  - `missionTimeSeconds`
  - `loadRatio`
  - `throttleTrim`
  - `missionResult`
  - `missionResultSummary`
  - `lastEvent`
- `Historical approximation`
  - reserved for scenarios whose historical framing is meaningful but whose runtime behavior is mostly custom.
- `Planned / placeholder`
  - current Apollo 11 CM, Apollo 13 LM, and rendezvous-family scaffolding in the setup UI.

## Future upgrades for deeper realism

- Replace the current native companion model with a pluggable AGC / yaAGC-backed execution layer.
- Implement more exact DSKY display arbitration based on source display-interface routines and Pinball behavior.
- Model executive coresets, VAC areas, waitlist interactions, and restart behavior more directly.
- Add source-linked annotations for each verb, noun, annunciator, and alarm path.

# Source Mapping

## Sources inspected

- Virtual AGC `Luminary099`
- Virtual AGC `Comanche055`
- Virtual AGC `LM131R1`
- Apollo 11 source repository for cross-checking family availability

## Relevant source areas identified

- DSKY input/display:
  - `DISPLAY_INTERFACE_ROUTINES.agc`
  - `PINBALL_GAME__BUTTONS_AND_LIGHTS.agc`
- LM landing program structure:
  - `THE_LUNAR_LANDING.agc`
  - `LUNAR_LANDING_GUIDANCE_EQUATIONS.agc`
  - `BURN,_BABY,_BURN_--_MASTER_IGNITION_ROUTINE.agc`
  - `PHASE_TABLE_MAINTENANCE.agc`
- Alarm behavior:
  - `ALARM_AND_ABORT.agc`
  - `EXECUTIVE.agc`
  - `WAITLIST.agc`
  - `RESTARTS_ROUTINE.agc`

## What each source informed in the app

- Import area:
  - `third_party/apollo/...` now carries untouched `Luminary099`, `Comanche055`, and `LM131R1` trees
- Program image handling:
  - `loadProgramImage(...)` now packages separate rope, rope metadata, rope labels, erasable, and bootstrap sections into one JNI call
  - Apollo 11 LM now loads `AP11ROPE.binsource` through a binsource-to-binary conversion path before native rope load
- Execution ownership transition:
  - current native CPU performs instruction-step skeleton work over a full Apollo 11 LM rope image
  - DSKY register arbitration now lives in `DskyIo` rather than `CompatibilityScenario`

## Notes on implementation honesty

- Apollo artifacts remain untouched in `third_party/apollo/...`.
- The active Apollo 11 LM slice now runs against more real Apollo rope memory than before.
- The app still does not claim full Apollo software execution because phase, flight, and outcome behavior remain compatibility-driven.

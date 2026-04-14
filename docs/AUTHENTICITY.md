# Authenticity Boundaries

## Apollo artifact rule

- Apollo source code, listings, and imported Apollo-derived program artifacts must remain untouched.
- Authenticity gains must come from better emulator/peripheral execution around Apollo artifacts, not from rewriting those artifacts into app logic.

## Current native core vs strict AGC target

- Current native core:
  - uses AGC-like CPU/program-image/DSKY/state boundaries
  - preserves a stable emulator-style JNI surface
  - now loads Apollo 11 LM `AP11ROPE.binsource` into a full native rope image
  - now uses bank-aware rope and erasable/fixed memory access for the active execution slice
  - now owns a broader AGC instruction subset plus basic channel operations
  - still uses a compatibility-owned entry overlay and compatibility flight behavior
- Strict AGC target:
  - CPU state advancement driven by AGC instruction execution
  - Luminary-derived rope and erasable state loaded into the native core
  - DSKY behavior and alarm/display interactions sourced more directly from AGC peripheral behavior

## Snapshot field ownership

- Emulator-derived now:
  - `phaseProgram`
  - DSKY display-mode effects for `V16 N68`, `V06 N63/N64/N60`, `V05 N09`
  - DSKY register arbitration across landing monitor / phase summary / last-alarm views
  - `PRO`, `RSET`, `KEY REL`, and verb/noun entry handling
  - 1201 / 1202 alarm ownership
- Derived from emulator state now:
  - visible `program`, `verb`, and `noun` digits when Apollo relay row output is present
  - visible DSKY register digits when Apollo relay row output is present
  - DSKY `COMP ACTY`, `KEY REL`, and `OPR ERR` when Apollo channel-11 output is present
  - display-mode selection from Apollo-driven `VERB/NOUN` output when relay rows are present
  - DSKY key injection into Apollo-facing input channels
  - `PRO` as an Apollo-style channel-32 proceed input pulse
  - narrow `P64` / `P66` adoption when Apollo-driven program display changes to those values
  - major-mode ownership via `AgcCpu`
  - execution-status text such as `EXEC <label>`
  - unsupported-opcode text such as `UNSUPPORTED <label>`
  - banked rope/fixed/erasable fetch ownership
  - channel 7 superbank state
- Compatibility fallback now:
  - local keyboard command parsing and entry buffering
  - local command consequences when Apollo relay output is absent
  - fallback phase/program thresholds when Apollo output is absent
  - `phaseLabel`
  - most telemetry values
  - mission time
  - load ratio
  - target vertical velocity
  - mission outcomes
  - non-execution event text

## Hard boundary

- The app now runs more real Apollo-derived rope memory than before.
- The app still does not honestly qualify as running Apollo software in the strict sense, because `CompatibilityScenario` still owns too much visible runtime behavior.

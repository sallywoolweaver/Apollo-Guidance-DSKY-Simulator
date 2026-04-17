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
  - fallback `PRO`, `RSET`, `KEY REL`, and verb/noun entry handling only when Apollo DSKY entry routing is absent
  - 1201 / 1202 alarm ownership
- Derived from emulator state now:
  - visible `program`, `verb`, and `noun` digits when Apollo relay row output is present
  - visible DSKY register digits when Apollo relay row output is present
  - DSKY `COMP ACTY`, `KEY REL`, and `OPR ERR` when Apollo channel-11 output is present
  - display-mode selection from Apollo-driven `VERB/NOUN` output when relay rows are present
  - DSKY key injection into Apollo-facing input channels
  - momentary release of channel-15 DSKY key input after the next native step
  - `PRO` as an Apollo-style channel-32 proceed input pulse
  - exact Luminary 099 direct entry into Apollo `PROCKEY` through the native core
  - a partial exact Luminary 099 `KEYRUPT1` / `LODSAMPT` / `KEYCOM` / `ACCEPTUP` lead-in before exact `NOVAC` request capture, exact `NOVAC2` / `SETLOC` Executive aftermath, exact `WAITLIST` `RESUME` entry work, the exact Apollo `RESUME` special instruction, and a later remaining fallback dispatch built from Apollo-captured `NEWLOC` `2CADR` state that now enters exact Apollo `SUPDXCHZ`; that remaining handoff is now anchored first to exact Apollo `RESUME`, then allowed to continue through the proven scheduler/job-switch slice `CHANJOB` / `ADVAN` / `NUDIRECT`, and only injects the captured request when Apollo reaches exact natural `SUPDXCHZ` / `SUPDXCHZ +1` transfer state; only if those transfer states are not reached in time does the older bounded post-`RESUME` emulator timer still fire; post-`SUPDXCHZ` continuation still waits for the Apollo-requested target to become active, and can now terminate on exact deeper aftermath labels `ENDPRCHG` / `INTRSM`, before the remaining bounded emulator window can end the routed flow
  - corrected Apollo double-word CPU semantics for `DCA`, `DAS`, and `DXCH`
  - more honest interrupt lead-in seeding for the routed key path via `ARUPT`, `LRUPT`, `BRUPT`, and interrupted `BBANK`
  - Apollo-corrected channel-10 relay-row decoding for visible DSKY register digits and signs
  - exact live label/bank/source-section mapping for the mapped Luminary 099 DSKY/input routines
  - narrow `P64` / `P66` adoption when Apollo-driven program display changes to those values
  - major-mode ownership via `AgcCpu`
  - `TC Q` subroutine returns through the native CPU path
  - execution-status text such as `EXEC <label>`
  - unsupported-opcode text such as `UNSUPPORTED <label>`
  - banked rope/fixed/erasable fetch ownership
  - channel 7 superbank state
- Compatibility fallback now:
  - the full Apollo-owned `KEYRUPT1` / `T4RUPT` interrupt path is still missing
  - the current key-input path still uses a narrow later emulator-side trigger for request dispatch rather than full Apollo job scheduling, core-set switching, and interrupt return
- exact Executive scheduler/job-switch labels now proven by the current bank-split disassembly path:
  - `CHANJOB`
  - `ENDPRCHG`
  - `NUCHANG2`
  - `DUMMYJOB`
  - `ADVAN`
  - `NUDIRECT`
  - `SUPDXCHZ`
  - `INTRSM`
- broader Executive scheduler/job-switch labels beyond that currently proven set remain intentionally unclaimed until the Luminary 099 listing proof path can emit a trustworthy local listing or an equally exact disassembly proof
- the derived local `yaYUL` build path now produces a helper binary without modifying Apollo artifacts, but Windows still denies executing that helper from this workspace, so exact scheduler-label proof is still incomplete
- the newer local alignment check now proves:
  - `CHANJOB` at `01:2706`
  - `ENDPRCHG` at `01:2765`
  - `NUCHANG2` at `01:3011`
  - `DUMMYJOB` at `01:3206`
  - `ADVAN` at `01:3214`
  - `NUDIRECT` at `01:3225`
  - `SUPDXCHZ` at `02:3165`
  - `INTRSM` at `03:2050`
  - the active Luminary 099 erasable image is still a custom initializer; it now seeds only the exact Executive fresh-start words needed for the narrow routed key path
  - local keyboard command parsing and entry buffering still exist only as fallback when Apollo DSKY entry routing is absent
  - local command consequences still exist only as fallback when Apollo relay output and Apollo DSKY entry routing are absent
  - local `KEY REL` / `RSET` / alarm acknowledgement consequences still exist only as fallback when Apollo/peripheral ownership is absent
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

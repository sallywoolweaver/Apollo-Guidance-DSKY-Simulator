# Scaffolding Removal Audit

## 2026-04-27

This audit covers the remaining helper, scaffolding, fallback, and compatibility-owned paths in the current runtime. It is intentionally narrow: remove only what Apollo-owned execution already replaces, and mark the rest against the current top blocker, the `CHANJOB / ENDPRCHG / INTRSM` restore-dispatch corridor and the resulting `TC 0177` / dynamic core-set drop.

| File / function / asset | What it does | Why it exists | Current status | Apollo-owned replacement target | Removed in this batch |
| --- | --- | --- | --- | --- | --- |
| `app/src/main/cpp/NativeApolloCore.cpp:isSchedulerDispatchBoundaryLabel()` | Old label classifier for scheduler dispatch boundaries | Earlier routed-trace scaffolding | `OBSOLETE / DEAD` | None; current routed path no longer uses it | Yes |
| `app/src/main/cpp/NativeApolloCore.cpp:primeApolloKeyruptLeadInState()` | Seeds routed interrupt lead-in state before exact `KEYRUPT1` | Routed key path still lacks full Apollo interrupt-vector ownership | `NOT YET REMOVABLE` | Full Apollo-owned interrupt lead-in through the real rupture vector and interrupted runnable-context state | No |
| `app/src/main/cpp/NativeApolloCore.cpp:dispatchCapturedNovacRequest()` | Captures Apollo-owned `NEWLOC/NEWLOC+1` request words after exact `NOVAC` | Apollo still does not self-dispatch the request naturally | `NOT YET REMOVABLE` | Natural Apollo-owned scheduler/request consumption before `SUPDXCHZ` | No |
| `app/src/main/cpp/NativeApolloCore.cpp:dispatchPendingExecutiveRequest()` | Performs the remaining forced handoff into `SUPDXCHZ` | Apollo still fails to carry the request to natural transfer before exhaustion | `NOT YET REMOVABLE` | Natural `SUPDXCHZ / SUPDXCHZ +1` transfer from exact Executive/job-switch state | No |
| `app/src/main/cpp/NativeApolloCore.cpp:continueInterpreterStallToNaturalTransfer()` | Gives the exact `03:0177 -> 0223` stall corridor extra Apollo execution before forced handoff | Needed to anchor the remaining handoff to exact Apollo state rather than flat exhaustion | `NOT YET REMOVABLE` | Exact `CHANJOB / ENDPRCHG / INTRSM` restore-dispatch semantics that avoid the dormant core-set drop | No |
| `app/src/main/cpp/NativeApolloCore.cpp:continueFinalTransitionToNaturalTransfer()` | Gives the final proven pre-transfer slice a continuation window to reach natural transfer | Needed because Apollo still does not always reach `SUPDXCHZ` on its own | `NOT YET REMOVABLE` | Deeper Apollo-owned scheduler/job-switch aftermath that makes transfer inevitable | No |
| `app/src/main/cpp/NativeApolloCore.cpp:kPostDispatchTargetBudget` and `continueAfterExecutiveDispatch()` | Bounded post-`SUPDXCHZ` continuation after the target becomes active | Apollo aftermath still does not always reach exact completion boundaries soon enough | `NOT YET REMOVABLE` | More Apollo-owned post-dispatch aftermath ending naturally at `ENDOFJOB`, `ENDPRCHG`, `TASKOVER`, or `INTRSM` | No |
| `app/src/main/cpp/NativeApolloCore.cpp:jumpToLabelWithSwitchedBank()` | Generic helper for routed label entry with explicit switched bank setup | Still used by routed `KEYRUPT1`/`PROCKEY` entry and forced `SUPDXCHZ` handoff | `NOT YET REMOVABLE` | Natural Apollo vectoring / scheduler dispatch without emulator-side bank-jump helpers | No |
| `app/src/main/cpp/DskyIo.cpp:pressKey()` local verb/noun buffering and command parsing fallback | Local DSKY entry behavior when Apollo-owned input/display path is absent | Needed when relay output or routed Apollo input ownership is absent | `NOT YET REMOVABLE` | End-to-end Apollo Pinball/input consequence flow | No |
| `app/src/main/cpp/DskyIo.cpp:executePendingCommand()` local command consequences | Local handling for commands like `V37 N63`, `V16 N68`, `V05 N09` | Apollo does not yet own all command consequences when fallback input mode is active | `NOT YET REMOVABLE` | Apollo-owned DSKY/Executive command consequence path | No |
| `app/src/main/cpp/DskyIo.cpp` fallback `KEY REL` / `RSET` / alarm acknowledgement branches | Local reset/acknowledgement consequences | Apollo/peripheral ownership is still incomplete when fallback mode is active | `NOT YET REMOVABLE` | Apollo-owned peripheral/alarm consequence path | No |
| `app/src/main/cpp/CompatibilityScenario.cpp:step()` | Compatibility-owned descent physics, mission time, telemetry, and outcomes | Real Apollo runtime still does not own mission-side behavior | `NOT YET REMOVABLE` | Broader Apollo-owned runtime and peripheral/state ownership | No |
| `app/src/main/cpp/CompatibilityScenario.cpp:updatePhaseAndOutcome()` | Compatibility-owned `P63 -> P64 -> P66` thresholds and touchdown outcomes | Apollo display/program state is still not sufficient to own these transitions end-to-end | `NOT YET REMOVABLE` | Apollo-owned program/phase transitions and mission completion behavior | No |
| `app/src/main/assets/programs/apollo11/lm/luminary099/erasable_init.cfg` | Custom erasable initializer for the active Luminary runtime | Needed because the runtime still cannot bootstrap the required Executive words from a fully Apollo-owned startup path | `NOT YET REMOVABLE` | More exact Apollo-owned fresh-start / interrupt / scheduler state | No |
| `app/src/main/assets/bootstrap/apollo11/lm/luminary099/bootstrap.cfg` | Custom scenario/bootstrap package for initial app state | App startup and scenario ownership are still compatibility/bootstrap-driven | `NOT YET REMOVABLE` | Apollo-owned startup behavior with compatibility reduced toward bootstrap-only | No |
| Derived alias `CHARIN_2CADR` in runtime/debug artifacts | Names the exact `2CADR CHARIN` request site for tracing | Needed for honest runtime proof while the remaining dispatch path is still custom | `UNCERTAIN` | Natural Apollo-owned request consumption plus less alias-heavy tracing | No |

## Current blocker linkage

- Blocked directly by `CHANJOB / ENDPRCHG / INTRSM`:
  - `primeApolloKeyruptLeadInState()`
  - `dispatchPendingExecutiveRequest()`
  - `continueInterpreterStallToNaturalTransfer()`
  - `continueFinalTransitionToNaturalTransfer()`
- Removable independently of that blocker:
  - dead local label helpers like `isSchedulerDispatchBoundaryLabel()`
  - stale comments/docs once runtime ownership has already replaced them
- Still load-bearing, but not directly blocked on `TC 0177`:
  - `DskyIo` fallback entry/consequence behavior
  - `CompatibilityScenario` mission ownership
  - custom bootstrap and erasable-init assets

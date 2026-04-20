# Main Goals Alignment Audit

Date: 2026-04-17

## Overall verdict

The project is still aligned with the main mission, but only partially aligned on several of the highest-priority runtime-ownership goals.

The best recent work stayed on the right axis: emulator-core ownership, exact Apollo proof paths, Executive routing, relay-driven DSKY output, and reducing fallback timers. That is real strategic alignment. The project has not recently drifted into presentation-first work in a way that dominates the roadmap.

There is still meaningful lag in the most important areas:

- `CompatibilityScenario` still owns too much mission behavior.
- `DskyIo` still owns too much fallback input/consequence behavior.
- the runtime still depends on emulator-side fallback at the end of the routed Executive/job-switch path.

No higher priority has overtaken the current emulator/core-set/job-switch work. The current next step should still be deeper Executive/core-set/job-switch aftermath work, not graphics, source-browser growth, or broader scenario work.

## Requirement-by-requirement alignment check

### 1. Keep Apollo artifacts untouched and read-only

Status: `ALIGNED`

Evidence:

- [docs/APOLLO_IMPORT_AUDIT.md](docs/APOLLO_IMPORT_AUDIT.md)
- [docs/AUTHENTICITY.md](docs/AUTHENTICITY.md)
- `third_party/apollo/apollo11/lm/luminary099/...`
- `third_party/apollo/apollo11/cm/comanche055/...`
- `third_party/apollo/apollo13/lm/lm131r1/...`
- derived tooling/output lives under `third_party/_derived_tools/...`, not `third_party/apollo/...`

What is going well:

- The imported Apollo families are clearly treated as untouched historical artifacts.
- Proof tooling and generated output are being placed in derived locations instead of back-writing into Apollo trees.
- Recent proof-path work explicitly adapted tooling around Apollo sources instead of modifying Apollo artifacts.

What is still violating or lagging:

- Repo hygiene is not perfect. `git status` currently shows:
  - `? third_party/apollo/upstream/virtualagc`
  - `?? tools/__pycache__/check_luminary099_executive_alignment.cpython-314.pyc`
- This is not an Apollo-artifact corruption problem, but it is a boundary/hygiene problem that can confuse provenance if left sloppy.

What should happen next:

- Keep derived output out of `third_party/apollo/...`.
- Clean Python cache noise from `tools/__pycache__/`.
- Keep upstream clone/proof-tool status documented so derived tooling is never mistaken for source-of-truth artifacts.

### 2. Actually run Apollo code through the emulator core

Status: `PARTIALLY ALIGNED`

Evidence:

- [docs/EXECUTION_PROGRESS_LOG.md](docs/EXECUTION_PROGRESS_LOG.md)
- [docs/ROUTINE_ADDRESS_MAP.md](docs/ROUTINE_ADDRESS_MAP.md)
- [app/src/main/cpp/NativeApolloCore.cpp](app/src/main/cpp/NativeApolloCore.cpp)
- [app/src/main/cpp/AgcCpu.cpp](app/src/main/cpp/AgcCpu.cpp)
- [docs/README.md](docs/README.md)

What is going well:

- Real Apollo rope artifacts are loaded into the native core.
- The routed Luminary 099 key path now executes a real Apollo path through:
  - `KEYRUPT1`
  - `LODSAMPT`
  - `KEYCOM`
  - `ACCEPTUP`
  - `NOVAC`
  - `NOVAC2`
  - `NOVAC3`
  - `CORFOUND`
  - `SETLOC`
  - `RESUME`
  - scheduler-side slice
  - natural `SUPDXCHZ` transfer state
  - completion-side slice
- Exact label/address proof for the current Executive slice is now materially stronger and source-proven enough for this routed path.

What is still violating or lagging:

- The app still does not honestly run Apollo mission behavior end-to-end.
- The final routed Executive handoff still depends on fallback when the overall routed-step budget is exhausted.
- Much of the visible mission state is still not Apollo-owned.

What should happen next:

- Continue deeper Executive/core-set/job-switch aftermath work.
- Keep reducing the remaining fallback primitive only where Apollo-owned state can replace it.

### 3. Keep Apollo historical artifacts, runtime assets, bootstrap data, and source-annotation data strictly separated

Status: `ALIGNED`

Evidence:

- [docs/README.md](docs/README.md)
- [docs/APOLLO_IMPORT_AUDIT.md](docs/APOLLO_IMPORT_AUDIT.md)
- `third_party/apollo/...`
- `app/src/main/assets/programs/...`
- `app/src/main/assets/bootstrap/...`
- `app/src/main/assets/source_annotations/...`

What is going well:

- The repo layout still reflects the required category split.
- Runtime conversion/proof outputs are not being mixed back into Apollo import folders.
- Bootstrap/scenario behavior remains outside the Apollo import trees.

What is still violating or lagging:

- Upstream clone material under `third_party/apollo/upstream/...` is legitimate source material, but it increases the burden to keep derived/generated files clearly separate and documented.

What should happen next:

- Preserve the current category boundaries.
- Keep all generated proof output in `third_party/_derived_tools/...` or docs/tooling locations, never under `third_party/apollo/...`.

### 4. Reduce and eventually eliminate `CompatibilityScenario` ownership

Status: `PARTIALLY ALIGNED`

Evidence:

- [docs/COMPATIBILITYSCENARIO_REDUCTION.md](docs/COMPATIBILITYSCENARIO_REDUCTION.md)
- [app/src/main/cpp/CompatibilityScenario.cpp](app/src/main/cpp/CompatibilityScenario.cpp)

What is going well:

- Some visible ownership has moved out already:
  - more DSKY display ownership
  - Apollo-driven `P64` / `P66` adoption
  - alarm ownership partly moved to `AlarmExecutive`
  - routed keypath ownership moved deeper into the native core

What is still violating or lagging:

- `CompatibilityScenario` still owns:
  - descent physics
  - mission time advancement
  - throttle trim effects
  - telemetry source values
  - mission outcomes
  - fallback phase/program transition logic
  - visible phase labels
  - event text
- This is still a major strategic gap relative to the mission.

What should happen next:

- Continue removing ownership only when Apollo runtime state can honestly replace it.
- Do not replace `CompatibilityScenario` logic with a new fake layer elsewhere.

### 5. Make DSKY/input/peripheral behavior Apollo-driven

Status: `PARTIALLY ALIGNED`

Evidence:

- [app/src/main/cpp/DskyIo.cpp](app/src/main/cpp/DskyIo.cpp)
- [app/src/main/cpp/NativeApolloCore.cpp](app/src/main/cpp/NativeApolloCore.cpp)
- [docs/EXECUTION_PROGRESS_LOG.md](docs/EXECUTION_PROGRESS_LOG.md)
- [docs/AUTHENTICITY.md](docs/AUTHENTICITY.md)

What is going well:

- Apollo relay/channel output can now own visible:
  - `PROG`
  - `VERB`
  - `NOUN`
  - register digits/signs
  - `COMP ACTY`
  - `KEY REL`
  - `OPR ERR`
- The active key path now routes through real Apollo interrupt/executive labels instead of a purely local DSKY consequence path.
- The relay decode was corrected against `yaDSKY` expectations rather than left as arbitrary UI formatting.

What is still violating or lagging:

- `DskyIo` still owns fallback behavior for:
  - verb/noun buffering
  - command parsing
  - `RSET`
  - `KEY REL`
  - alarm acknowledgement
  - fallback display-mode selection
  - fallback telemetry/register formatting
- Input consequence flow is not yet fully Apollo-owned end-to-end.

What should happen next:

- Keep shrinking `DskyIo` only where Apollo-owned input/peripheral behavior truly replaces it.
- Continue prioritizing routed Executive/peripheral aftermath over local DSKY state-machine cleanup for its own sake.

### 6. Make mission/program behavior Apollo-driven

Status: `PARTIALLY ALIGNED`

Evidence:

- [app/src/main/cpp/CompatibilityScenario.cpp](app/src/main/cpp/CompatibilityScenario.cpp)
- [docs/COMPATIBILITYSCENARIO_REDUCTION.md](docs/COMPATIBILITYSCENARIO_REDUCTION.md)
- [docs/AUTHENTICITY.md](docs/AUTHENTICITY.md)

What is going well:

- Visible `P64` / `P66` adoption can now come from Apollo DSKY program output instead of only from compatibility thresholds.

What is still violating or lagging:

- Actual mission/program transitions are still mostly compatibility-owned.
- `P63 -> P64` and `P64 -> P66` fallback thresholds still remain.
- Mission telemetry and outcomes are still custom simulation behavior.

What should happen next:

- Do not force mission/program ownership changes before the Executive/peripheral path is strong enough.
- Continue deeper core/executive work first, then move one real program behavior at a time.

### 7. Support Apollo software families separately and honestly

Status: `PARTIALLY ALIGNED`

Evidence:

- [docs/APOLLO_IMPORT_AUDIT.md](docs/APOLLO_IMPORT_AUDIT.md)
- `third_party/apollo/apollo11/lm/luminary099/...`
- `third_party/apollo/apollo11/cm/comanche055/...`
- `third_party/apollo/apollo13/lm/lm131r1/...`

What is going well:

- Apollo 11 LM, Apollo 11 CM, and Apollo 13 LM family imports are present and documented separately.
- The docs do not pretend that imported families are all active runtime families.

What is still violating or lagging:

- Only the Apollo 11 LM path is materially active in the current runtime work.
- CM and Apollo 13 LM are imported honestly, but not yet meaningful execution targets.

What should happen next:

- Keep the family separation.
- Do not overclaim runtime support for the non-active families.
- Continue execution work on one family honestly before broadening.

### 8. Keep docs brutally honest

Status: `ALIGNED`

Evidence:

- [docs/AUTHENTICITY.md](docs/AUTHENTICITY.md)
- [docs/EXECUTION_PROGRESS_LOG.md](docs/EXECUTION_PROGRESS_LOG.md)
- [docs/COMPATIBILITYSCENARIO_REDUCTION.md](docs/COMPATIBILITYSCENARIO_REDUCTION.md)
- [docs/LISTING_PROOF_PATH.md](docs/LISTING_PROOF_PATH.md)
- [docs/APOLLO_IMPORT_AUDIT.md](docs/APOLLO_IMPORT_AUDIT.md)
- [README.md](README.md)
- [docs/README.md](docs/README.md)

What is going well:

- The docs repeatedly state that the app is not yet a strict AGC emulator.
- The docs distinguish Apollo-owned, emulator-derived, and fallback/custom behavior.
- Proof-path limits and remaining blockers are being documented instead of hand-waved.

What is still violating or lagging:

- Some README-level wording is necessarily compressed and can sound more complete than the detailed authenticity docs unless read carefully.
- Repo hygiene noise (`tools/__pycache__`, untracked upstream clone status) makes provenance slightly less clean than it should be.

What should happen next:

- Keep the detailed docs as the source of truth.
- Keep documenting technical debt in explicit “what / why / replacement target / reduced this batch” form.

### 9. Add graphics/cockpit/mission visuals later, after execution is stronger

Status: `ALIGNED`

Evidence:

- Recent work is overwhelmingly in native core, proof tooling, runtime mapping, and execution docs.
- The one notable visual/logo change did not redirect the roadmap.

What is going well:

- Presentation work is not driving the roadmap.
- Emulator work is still clearly prioritized over graphics.

What is still violating or lagging:

- No major violation. The only caution is to keep future logo/UI requests from diluting the runtime agenda.

What should happen next:

- Keep visuals explicitly secondary until runtime ownership is stronger.

### 10. Keep source-browser/overlays as an execution/debugging tool, not a museum distraction

Status: `PARTIALLY ALIGNED`

Evidence:

- [docs/ROUTINE_ADDRESS_MAP.md](docs/ROUTINE_ADDRESS_MAP.md)
- [docs/LISTING_PROOF_PATH.md](docs/LISTING_PROOF_PATH.md)
- [docs/EXECUTION_PROGRESS_LOG.md](docs/EXECUTION_PROGRESS_LOG.md)

What is going well:

- Recent source/proof work was used to support exact routing, label proof, and runtime debugging.
- The source/debug layer has stayed subordinate to emulator progress.

What is still violating or lagging:

- The source browser itself remains limited.
- The project still carries some risk of treating mapping/source views as visible product progress when they are only support infrastructure.

What should happen next:

- Keep source tooling focused on execution visibility and proof, not browsing polish.

## Apollo-artifact integrity check

- Apollo artifacts appear to remain untouched/read-only in `third_party/apollo/...`.
- No helper/bootstrap/generated data was found leaking into the imported family folders.
- Runtime logic does not appear to depend on modified Apollo artifacts; it depends on:
  - imported Apollo source/binsource artifacts
  - derived proof/disassembly outputs outside the Apollo tree
  - emulator-side glue/fallback code in native/app layers
- Derived/proof tooling is currently distinguishable from original Apollo artifacts because it lives under:
  - `third_party/_derived_tools/...`
  - `tools/...`
  - `docs/...`

Audit caution:

- `third_party/apollo/upstream/virtualagc` appears as an untracked nested repository in the parent repo status.
- That is not evidence of artifact mutation, but it is a provenance/repo-hygiene wrinkle that should stay explicitly documented.

## Runtime-ownership check

### Apollo-owned now

- rope loading from imported Apollo-derived program artifacts
- substantial routed Luminary 099 key-input path through exact Apollo labels
- relay/channel-driven visible DSKY output for:
  - `PROG`
  - `VERB`
  - `NOUN`
  - register digits/signs
  - `COMP ACTY`
  - `KEY REL`
  - `OPR ERR`
- exact proven scheduler/transfer/completion boundaries for the current Executive slice

### Emulator-derived but not yet Apollo-owned

- proof/disassembly tooling
- routed-step budgets and fallback boundaries in `NativeApolloCore`
- DSKY relay decode and adapter behavior
- JNI/app/native adaptation layers

### Still fallback/custom

- `DskyIo` fallback input/consequence behavior
- `CompatibilityScenario` mission/telemetry/outcome ownership
- remaining fallback primitive when routed-step exhaustion occurs
- post-`SUPDXCHZ` completion fallback budget in cases where exact completion ownership is not yet enough

### Trend check

Fallback/custom ownership is shrinking, not growing. That is the strongest sign the project is still aligned. The shrinkage is real but still incomplete, especially in mission behavior.

## Technical debt check

### Shrinking

- remaining late invocation fallback logic in `NativeApolloCore`
- post-`SUPDXCHZ` completion fallback behavior
- proof-path uncertainty around the Executive scheduler slice

### Stuck

- `DskyIo` fallback command/input behavior
- `CompatibilityScenario` mission/phase/telemetry/outcome ownership
- local `yaYUL` execution denial for emitting a full local listing

### Growing

- No major runtime-architecture debt appears to be growing right now.
- Minor repo-hygiene/provenance clutter is growing slightly:
  - `tools/__pycache__/...`
  - nested upstream repo status noise

## Roadmap check

The project should continue with deeper Executive/core-set/job-switch aftermath work.

Nothing in the current state justifies elevating graphics, source-browser polish, branding, or new scenarios above the current emulator/core ownership work.

The only issue that could temporarily jump ahead is repo/provenance hygiene if Apollo/derived boundaries become blurred, but that is not yet the dominant blocker.

## Hard truth

The app is now a partially Apollo-driven Android shell around a real native emulator path that executes a meaningful Luminary 099 slice and can drive visible DSKY output from Apollo relay/channel state.

It is still not an honest full Apollo mission execution app. It still relies on fallback/custom layers for major parts of DSKY input consequence flow, mission/program behavior, telemetry, outcomes, and the last fallback primitive in the routed Executive handoff.

The single biggest misalignment risk right now is mistaking deeper routed-label progress for completion of Apollo runtime ownership. The project is aligned only if it keeps using those gains to remove fallback/custom ownership, not if it starts celebrating the routing layer as though the mission behavior were already Apollo-owned.

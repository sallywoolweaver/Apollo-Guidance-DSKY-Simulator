# Assumptions

## Historical uncertainties

- The slice does not claim exact Apollo 11 descent initial state, timing, or trajectory. Initial altitude, fuel, and velocities are mobile-simulation approximations.
- Alarm timing is inspired by the historical pattern discussed in Cherry's 1969 analysis, but not matched to exact mission seconds.
- The current Luminary099 asset set is split into rope excerpt, erasable initializer, and bootstrap overlay files. It is still not a full original AGC rope image plus sourced erasable state.

## Simulation simplifications

- The flight model uses lunar gravity with a simplified thrust and damping model.
- Horizontal motion is reduced through a coarse damping term rather than full attitude-coupled translational dynamics.
- Landing outcomes are classified with explicit simulator thresholds rather than sourced lunar-module structural limits.

## Mobile usability concessions

- Throttle trim buttons were added because a phone/tablet needs an actionable control surface beyond pure keypad entry.
- The DSKY layout uses readable Compose components rather than pixel-faithful hardware dimensions.
- Several registers are multiplexed into a simpler presentation to preserve readability on smaller screens.
- The JNI API is coarse-grained by design so the UI layer remains stable when the native core becomes more historically exact.
- Engineer Mode currently maps simulator state to source annotations by curated phase/alarm relationships rather than exact runtime label execution.

## Source annotation extraction assumptions

- The initial annotation asset is a curated Luminary099 subset rather than a complete source-index of every label.
- The extraction utility uses simple heuristic parsing for comments and labels and should be treated as an indexing aid, not a proof tool.
- Some file-path/title normalization may differ slightly between GitHub source files and ibiblio listing names; this should be revisited when a full local Apollo source import is used.

## Software-family scaffolding assumptions

- The current setup screen lists source-backed software families even when the app does not execute them yet.
- Apollo 11 CM `Comanche055` and Apollo 13 LM `LM131 rev 1` are treated as strong next candidates based on Virtual AGC availability, but they are not implemented runtimes yet.
- The rendezvous-family entry is intentionally broad because the exact first rendezvous target still needs tighter source-backed selection.

## Rationale

- These choices keep the first slice playable, testable, and honest while preserving a clean path toward a more source-driven simulator later.

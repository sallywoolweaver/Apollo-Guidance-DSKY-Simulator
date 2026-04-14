# CompatibilityScenario Reduction

## Moved out already

- native CPU rope-label execution tracking
- DSKY `COMP ACTY`
- DSKY execution status such as `EXEC <label>`
- DSKY unsupported-opcode status such as `UNSUPPORTED <label>`
- 1201/1202 alarm ownership in `AlarmExecutive`
- banked rope/fixed/erasable address ownership in the native CPU
- channel 7 superbank ownership in the native CPU
- DSKY relay-row ownership for visible:
  - `PROG`
  - `VERB`
  - `NOUN`
  - register digits
- channel-11 annunciator ownership for visible:
  - `COMP ACTY`
  - `KEY REL`
  - `OPR ERR`
- DSKY key injection into Apollo-facing input channels for:
  - keypad keys on channel 15
  - `VERB`
  - `NOUN`
  - `CLR`
  - `ENTR`
  - `RSET`
  - `KEY REL`
  - `PRO` pulse on channel 32 bit 14
- DSKY register arbitration for:
  - landing monitor
  - phase summary
  - last-alarm display

## Still owned by CompatibilityScenario

- descent physics
- mission time advancement
- throttle trim effects
- target vertical velocity fallback logic
- load-ratio synthesis
- fallback phase/program transition thresholds for `P63` to `P64` and `P64` to `P66`
- mission outcomes
- telemetry source values:
  - altitude
  - vertical velocity
  - horizontal velocity
  - fuel
- visible phase labels
- non-execution event text such as touchdown or fuel exhaustion
- local verb/noun buffering and command parsing when Apollo display ownership is absent
- local alarm acknowledgement fallback when Apollo/peripheral ownership is absent

## Next candidate to remove

- phase/program transition ownership

Reason:

- It is the next visible user-facing behavior after DSKY display arbitration.
- The displayed program digits can now be Apollo-driven, and `P64`/`P66` adoption can now follow that output.
- The remaining blocker is the lack of enough Apollo-owned guidance/peripheral state to eliminate the fallback thresholds entirely.

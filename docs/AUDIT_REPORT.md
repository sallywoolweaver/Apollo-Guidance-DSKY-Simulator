# Audit Report

## Current architecture summary

- Android shell:
  - Kotlin + Compose
- Native layer:
  - C++ core built with NDK + CMake
  - coarse stable JNI surface
- Data/authenticity:
  - real Apollo import area under `third_party/apollo/...`
  - separate runtime program/bootstrap/annotation assets

## What the current app truly is

- It is not yet a strict AGC emulator.
- It is an emulator-oriented Android shell with a custom native compatibility layer behind a stable JNI interface.
- It now loads a full Apollo-derived Apollo 11 LM rope image from `AP11ROPE.binsource` into native rope memory.
- It now executes a larger bank-aware AGC instruction subset with native ownership of fixed/erasable banking and basic channel operations.
- It still does not execute Apollo software strongly enough to claim true Apollo runtime execution.

## Native core authenticity boundary

- `AgcMemoryImage`
  - now loads full Apollo-derived Apollo 11 LM rope bytes
  - now normalizes bank-local rope offsets so rope labels and rope words line up in the same address space
  - keeps bootstrap data separate
- `AgcCpu`
  - now owns banked erasable/fixed memory access, superbank-sensitive fixed-bank selection, and a broader AGC opcode subset
- `DskyIo`
  - now owns DSKY register arbitration
- `CompatibilityScenario`
  - still owns the actual descent, phase, and outcome logic

## Bottom line classification

- Best description:
  - emulator-oriented native core plus a still-dominant compatibility runtime
- Not an honest description:
  - full Apollo software execution

## Best next step

1. Replace overlay-driven Luminary 099 entry behavior with a truer Apollo boot or restart path.
2. Implement the AGC display/channel behavior needed for real DSKY digit ownership.
3. Remove phase ownership from `CompatibilityScenario` only after native execution can honestly drive it.

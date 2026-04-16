# Apollo Guidance DSKY Simulator

Project documentation lives in [docs/README.md](docs/README.md).

The repo now contains real untouched Apollo import trees under `third_party/apollo/...` for Apollo 11 LM `Luminary099`, Apollo 11 CM `Comanche055`, and Apollo 13 LM `LM131R1`.

The current app is still not a full AGC emulator, but the Apollo 11 LM slice now loads a full Apollo-derived rope image from `AP11ROPE.binsource` through the native core while keeping erasable/init/bootstrap data separate.

The active Luminary 099 key path now routes through exact Apollo labels including `KEYRUPT1`, `LODSAMPT`, `KEYCOM`, `ACCEPTUP`, `NOVAC`, `NOVAC2`, `NOVAC3`, `CORFOUND`, `SETLOC`, and `RESUME`, and visible DSKY digits/annunciators can now come from Apollo relay/channel output. The biggest remaining blocker is still the Apollo-owned Executive/core-set/job-switch aftermath after `SETLOC` / `RESUME`, so flight, phase-threshold, telemetry, and outcome behavior are not yet fully Apollo-owned.

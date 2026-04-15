# Crash Fix Report

## 1. Exact crash cause

Two separate crashes were fixed in this stability pass:

1. Launch crash:
   - Android was trying to instantiate `com.sallyw.apolloguidancedsky.MainActivity`, but the actual activity class in the app source was `com.example.myapplication.MainActivity`.
   - This caused an immediate launch-time `ClassNotFoundException` before the app could reach Compose or the native core.

2. "Play"/scenario-start crash:
   - `ProgramAssetLoader.parseBinsourceToRopeBinary()` tried to parse `@` as octal while loading `AP11ROPE.binsource`.
   - The real Apollo binsource uses `@` as a filler/non-word marker in some rows.
   - This caused a `NumberFormatException` when the user tried to start a scenario.

## 2. Failing file/method/line

1. Launch crash:
   - Failing file: [AndroidManifest.xml](/abs/path/C:/Users/p00121261/AndroidStudioProjects/MyApplication/app/src/main/AndroidManifest.xml)
   - Failing entry: `<activity android:name=".MainActivity" ...>`
   - Failing runtime target from logcat:
     - `com.sallyw.apolloguidancedsky.MainActivity`
   - Related source class:
     - [MainActivity.kt](/abs/path/C:/Users/p00121261/AndroidStudioProjects/MyApplication/app/src/main/java/com/example/myapplication/MainActivity.kt)

2. Scenario-start crash:
   - Failing file: [ProgramAssetLoader.kt](/abs/path/C:/Users/p00121261/AndroidStudioProjects/MyApplication/app/src/main/java/com/example/myapplication/data/program/ProgramAssetLoader.kt)
   - Failing method: `parseBinsourceToRopeBinary`
   - Failing line from logcat:
     - `ProgramAssetLoader.kt:99`

## 3. Why it happened

1. Launch crash:
   - `app/build.gradle.kts` sets:
     - `namespace = "com.sallyw.apolloguidancedsky"`
     - `applicationId = "com.sallyw.apolloguidancedsky"`
   - The manifest used a relative activity name, `.MainActivity`, so Android resolved it to `com.sallyw.apolloguidancedsky.MainActivity`.
   - The actual Kotlin file still declared `package com.example.myapplication`, so that resolved activity class did not exist in the APK.

2. Scenario-start crash:
   - The Apollo binsource parser assumed every non-comment token in a data line was an octal word.
   - Real untouched Apollo binsource data includes `@` filler markers.
   - The parser should have skipped non-octal markers, but instead attempted `toInt(radix = 8)`.

## 4. What fix was applied

1. Launch crash:
   - Changed the manifest activity entry from the relative name to the real fully-qualified class:
     - from `.MainActivity`
     - to `com.example.myapplication.MainActivity`

2. Scenario-start crash:
   - Tightened `parseBinsourceToRopeBinary()` so it only treats real octal tokens as rope data.
   - Non-octal fillers such as `@` are now skipped.
   - The Apollo artifact itself was not changed.

- Both fixes were the smallest honest fixes because they corrected the adapter/parser behavior without rolling back recent emulator/runtime work.

## 5. Whether any Apollo-driven behavior had to be rolled back

- No.
- The recent Apollo-driven DSKY input/output work was preserved.
- No Apollo artifact imports were touched.
- No JNI contract changes were made.

## 6. Whether the app now launches successfully

- Yes.
- Verification performed on-device:
  - installed updated debug APK
  - launched `com.sallyw.apolloguidancedsky/com.example.myapplication.MainActivity`
  - confirmed live process via `adb shell pidof com.sallyw.apolloguidancedsky`
  - cleared logcat before launch and confirmed no fresh `AndroidRuntime`, `FATAL EXCEPTION`, `ClassNotFoundException`, `SIGSEGV`, or abort message afterward
- Scenario-start crash status:
  - the exact `NumberFormatException` in `ProgramAssetLoader` was fixed in code and local build/tests passed
  - I reinstalled and relaunched the updated APK successfully
  - I did not automate the exact on-device tap sequence for "Play", so final end-to-end user interaction should be rechecked on the tablet

## 7. Build/test status

- `.\\gradlew.bat test` — PASS
- `.\\gradlew.bat app:assembleDebug` — PASS

## Preserved recent gains

- Apollo-driven `PROG/VERB/NOUN` output
- Apollo-driven register-digit output
- Apollo-driven `COMP ACTY`, `KEY REL`, and `OPR ERR`
- Apollo-first DSKY key injection into Apollo-facing channels
- narrow Apollo-driven `P64` / `P66` adoption from displayed program output

## Biggest remaining blocker

- The app is now launching again, but the biggest remaining blocker is still incomplete Apollo-owned input/peripheral execution.
- DSKY output ownership improved, but Pinball/input-processing and full phase-transition ownership are still not fully Apollo-driven end-to-end.

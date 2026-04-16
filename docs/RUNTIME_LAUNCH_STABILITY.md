# Runtime Launch Stability

## Current result

- The app is **not currently crashing on launch**.
- The app is **not currently finishing itself on launch**.
- The app is **not currently being backgrounded on launch**.
- The latest instrumented run shows the app **launches, reaches the TITLE screen, stays alive, and remains foregrounded**.

## Exact evidence

### Successful cold start

Command:

- `adb shell am start -W -n com.sallyw.apolloguidancedsky/com.example.myapplication.MainActivity`

Result:

- `Status: ok`
- `LaunchState: COLD`
- `Activity: com.sallyw.apolloguidancedsky/com.example.myapplication.MainActivity`
- `TotalTime: 1812`

### App lifecycle / UI evidence

Clean launch logcat showed:

- `ActivityManager: Start proc 22995:com.sallyw.apolloguidancedsky/u0a114`
- `ApolloLaunch: screen=TITLE paused=false missionResult=false`
- `ActivityTaskManager: Displayed com.sallyw.apolloguidancedsky/com.example.myapplication.MainActivity`

That means the app process started, the activity drew a real window, and the app-side UI state reached the title screen.

### Foreground / PID evidence

Commands:

- `adb shell pidof com.sallyw.apolloguidancedsky`
- `adb shell dumpsys activity activities | rg "topResumedActivity|ResumedActivity|mLastFocusedRootTask=Task|com.sallyw.apolloguidancedsky|com.example.myapplication.MainActivity"`

Result:

- live PID present: `22995`
- `topResumedActivity=...com.sallyw.apolloguidancedsky/com.example.myapplication.MainActivity`
- visible focused task and focused window both belong to the app

## Why earlier results looked bad

The earlier "no PID / Launcher top-resumed" result was real at the time, but it was not evidence of a launch crash in the app itself.

The main misleading factors were:

- prior verification logs showed repeated `installPackageLI` kills:
  - `ActivityManager: Killing ... com.sallyw.apolloguidancedsky ... due to installPackageLI`
  - `Force removing ActivityRecord ... app died, no saved state`
- some adb probes were also polluted by sandbox/home-directory issues, which made the process/activity checks unreliable
- those older checks were mixed with reinstall cycles, so they captured package-replacement death rather than a clean post-launch steady state

## Exact cause

No app-side finish path or launch crash was confirmed.

The earlier disappearing-process evidence was caused by **verification during or around package reinstall**, plus some **bad adb probe conditions**, not by a confirmed in-app `finish()` path or a confirmed native/app crash on clean launch.

## Exact fix applied

Two changes were applied to make launch-state verification honest and repeatable:

1. Added minimal app-side instrumentation:
   - `MainActivity` lifecycle logs:
     - `onCreate`
     - `onStart`
     - `onResume`
     - `onPause`
     - `onStop`
     - `onDestroy`
   - screen-state logging for the top-level app screen
   - `ApolloSimViewModel` state-transition logging for screen/paused/variant/mission-result changes

2. Changed verification method:
   - cleared logcat before launch
   - used `am start -W` for synchronous launch confirmation
   - checked logcat, PID, and `dumpsys activity activities` against the same clean launch

## Whether the app now stays alive / foregrounded on device

- Yes, on the latest clean verified run.
- The app reached the title screen.
- A live PID was present.
- `MainActivity` was top-resumed and focused.

## Remaining caution

- This report covers the clean launch / initial foreground case.
- It does **not** prove there are no later runtime exits under deeper interaction paths.
- If a later disappearance is reported again, use the `ApolloLaunch` tag plus clean `am start -W` / PID / `dumpsys` checks before assuming a crash.

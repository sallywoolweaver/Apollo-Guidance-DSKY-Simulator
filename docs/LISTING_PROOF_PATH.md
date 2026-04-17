# Listing Proof Path

## Goal

Establish a trustworthy, reproducible local workflow for deriving exact Luminary 099 label/address relationships from the imported Apollo artifacts without modifying those artifacts.

This proof path is specifically needed for the Executive block around:

- `SUPDXCHZ`
- `DUMMYJOB`
- `ADVAN`
- `NUDIRECT`
- `CHANJOB`
- `ENDPRCHG`
- `NUCHANG2`
- `INTRSM`

## Local yaYUL/listing path attempted

Attempted local source-of-truth path:

1. use the imported upstream Virtual AGC `yaYUL` sources
2. copy them into a derived workspace
3. patch only the derived copy for Windows/MSVC-compatible macro syntax
4. build a derived local `yaYUL.exe`
5. run that derived helper against a copied Luminary 099 source tree
6. capture the listing output into a derived `.lst` file

Apollo artifacts were not modified during this process.

## Why the original local path failed

The checked-in Windows build path for `yaYUL` was not usable as-is in this workspace.

The original blocker was:

- the upstream `MSC_VS` macro definitions in `yaYUL.h` use GNU-style statement-expression syntax such as:
  - `#define VALID_ADDRESS ( { ... } )`
  - `#define REG(n) ( { ... } )`
  - `#define CONSTANT(n) ( { ... } )`
  - `#define FIXEDADD(n) ( { ... } )`

That syntax prevented a usable local Windows listing build.

## Fix implemented

A derived build helper now exists and does not modify Apollo artifacts:

- `tools/build_derived_yayul.ps1`

What it does:

1. copies `third_party/apollo/upstream/virtualagc/yaYUL/*` into:
   - `third_party/_derived_tools/yaYUL-compat-src/`
2. patches only the derived copy of `yaYUL.h` so the `MSC_VS` macros use C-compatible compound-literal syntax
3. imports the Visual Studio build environment with `VsDevCmd.bat`
4. compiles the derived sources with:
   - NDK `clang.exe`
   - `--target=x86_64-pc-windows-msvc`
5. links the derived object files with `clang -fuse-ld=link`
6. produces:
   - `third_party/_derived_tools/yaYUL.exe`

This fixed the source/build side of the local proof path.

## Replacement/fallback workflow attempted

A derived listing runner also exists:

- `tools/generate_luminary099_listing.ps1`

What it does:

1. copies the imported Luminary 099 source tree from:
   - `third_party/apollo/apollo11/lm/luminary099/`
   into:
   - `third_party/_derived_tools/luminary099_listing_work/`
2. runs the derived `third_party/_derived_tools/yaYUL.exe --force MAIN.agc`
3. writes the listing output to:
   - `third_party/_derived_tools/Luminary099.lst`
4. extracts Executive scheduler labels into:
   - `third_party/_derived_tools/luminary099_executive_listing_extract.txt`

This workflow remains honest because:

- Apollo artifacts stay read-only in `third_party/apollo/...`
- all generated files live under `third_party/_derived_tools/...`

## Exact commands used

Commands run in this workspace:

```powershell
powershell -ExecutionPolicy Bypass -File tools\build_derived_yayul.ps1
```

```powershell
powershell -ExecutionPolicy Bypass -File tools\generate_luminary099_listing.ps1
```

```powershell
python tools\prove_luminary099_executive_labels.py
```

```powershell
python tools\check_luminary099_executive_alignment.py
```

```powershell
Get-Content third_party\apollo\upstream\virtualagc\Luminary099\AP11ROPE.binsource | python third_party\apollo\upstream\virtualagc\Tools\disassemblerAGC\disassemblerAGC.py --dtest --dbank=01 --dstart=2000 --dend=3777 | Set-Content -LiteralPath third_party\_derived_tools\luminary099_bank01_full.dtest.txt
```

```powershell
Get-Content third_party\apollo\upstream\virtualagc\Luminary099\AP11ROPE.binsource | python third_party\apollo\upstream\virtualagc\Tools\disassemblerAGC\disassemblerAGC.py --dtest --dbank=02 --dstart=2000 --dend=3777 | Set-Content -LiteralPath third_party\_derived_tools\luminary099_bank02_full.dtest.txt
```

```powershell
Get-Content third_party\apollo\upstream\virtualagc\Luminary099\AP11ROPE.binsource | python third_party\apollo\upstream\virtualagc\Tools\disassemblerAGC\disassemblerAGC.py --dtest --dbank=03 --dstart=2000 --dend=2400 | Set-Content -LiteralPath third_party\_derived_tools\luminary099_bank03_interp_window.dtest.txt
```

Diagnostic commands used while fixing the build:

```powershell
& cmd /c '"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 >nul && where cl && where link'
```

```powershell
& cmd /c '"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 >nul && dumpbin /headers third_party\_derived_tools\yaYUL.exe'
```

## Apollo artifacts used as source of truth

- `third_party/apollo/apollo11/lm/luminary099/MAIN.agc`
- `third_party/apollo/apollo11/lm/luminary099/EXECUTIVE.agc`
- `third_party/apollo/apollo11/lm/luminary099/TAGS_FOR_RELATIVE_SETLOC_AND_BLANK_BANK_CARDS.agc`
- `third_party/apollo/apollo11/lm/luminary099/AP11ROPE.binsource`
- `third_party/apollo/upstream/virtualagc/yaYUL/*`

## Output files produced

Produced successfully:

- `third_party/_derived_tools/yaYUL-compat-src/`
- `third_party/_derived_tools/yaYUL.exe`
- `third_party/_derived_tools/luminary099_bank01_full.dtest.txt`
- `third_party/_derived_tools/luminary099_bank03_interp_window.dtest.txt`
- `third_party/_derived_tools/luminary099_executive_label_proof.json`
- `third_party/_derived_tools/luminary099_executive_label_proof.txt`
- `third_party/_derived_tools/luminary099_executive_alignment_check.json`
- `third_party/_derived_tools/luminary099_executive_alignment_check.txt`

Not produced successfully:

- `third_party/_derived_tools/Luminary099.lst`
- `third_party/_derived_tools/luminary099_executive_listing_extract.txt`

## Exact evidence for the remaining blocker

The derived local assembler now builds, but Windows still refuses to execute it from this workspace.

Evidence:

- `Test-Path third_party\_derived_tools\yaYUL.exe` returned `True`
- `dumpbin /headers third_party\_derived_tools\yaYUL.exe` confirmed a valid `PE32+` console executable image
- direct execution failed with:
  - `Program 'yaYUL.exe' failed to run: Access is denied`
- `cmd /c third_party\_derived_tools\yaYUL.exe --help` also failed with:
  - `Access is denied`

So the remaining blocker is no longer the source/build path.
It is local execution policy/host behavior against the derived helper.

## Trustworthiness of the current result

### What is now trustworthy

- the imported Luminary 099 artifacts used as source of truth
- the derived local `yaYUL` source copy and build workflow
- the fact that a valid local `yaYUL.exe` can now be produced without modifying Apollo artifacts
- the conclusion that the old naive source-line-count proof is not trustworthy
- the bank-context correction for the Executive block:
  - `CHANJOB`, `DUMMYJOB`, `ADVAN`, and `NUDIRECT` are in bank `01`
  - `SUPDXCHZ` is in bank `02`
- the current alternate exact proof path built from:
  - imported `EXECUTIVE.agc`
  - imported `AP11ROPE.binsource`
  - imported `disassemblerAGC.py`
  - derived bank-specific `.dtest` disassembly windows
  - `tools/check_luminary099_executive_alignment.py`

### Exact Executive labels now proven honestly

- `CHANJOB` -> bank `01`, address `01:2706`
- `ENDPRCHG` -> bank `01`, address `01:2765`
- `NUCHANG2` -> bank `01`, address `01:3011`
- `DUMMYJOB` -> bank `01`, address `01:3206`
- `ADVAN` -> bank `01`, address `01:3214`
- `NUDIRECT` -> bank `01`, address `01:3225`
- `SUPDXCHZ` -> bank `02`, address `02:3165`
- `INTRSM` -> bank `03`, address `03:2050`

These are now trustworthy enough for exact Executive scheduler/job-switch work in the current routed key-input slice because the source sequences and the derived disassembly sequences match exactly in the emitted alignment report:

- `third_party/_derived_tools/luminary099_executive_alignment_check.txt`

### What is still not trustworthy enough

- a full Luminary 099 `.lst` emitted locally from `yaYUL.exe`
- broader scheduler/job-switch labels beyond the currently proven set
- any scheduler/runtime claim that depends on labels not present in the alignment report

Reason:

- the derived listing was still not emitted because Windows denied executing the derived helper
- the current exact proof is therefore the bank-split disassembly workflow, not a completed local assembler-listing workflow

## Current conclusion

The local `yaYUL` build path has been fixed far enough to produce a derived assembler binary, but the exact local listing-emission path is still blocked by host execution denial for that helper.

That means:

- the old "bank-02 mismatch" blocker has been reduced
- the proof path is strong enough to support exact work on:
  - `CHANJOB`
  - `DUMMYJOB`
  - `ADVAN`
  - `NUDIRECT`
  - `SUPDXCHZ`
- the proof path is still not finished for broader Executive work because the local `.lst` path remains blocked

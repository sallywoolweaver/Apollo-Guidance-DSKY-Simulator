$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $repoRoot 'third_party\apollo\upstream\virtualagc\yaYUL'
$derivedDir = Join-Path $repoRoot 'third_party\_derived_tools\yaYUL-compat-src'
$outputExe = Join-Path $repoRoot 'third_party\_derived_tools\yaYUL.exe'
$clang = 'C:\Users\p00121261\AppData\Local\Android\Sdk\ndk\28.2.13676358\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Import-CmdEnvironment([string] $BatchFile, [string] $Arguments) {
    $setOutput = & cmd /c """$BatchFile"" $Arguments >nul && set"
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to initialize Visual Studio command environment."
    }

    foreach ($line in $setOutput) {
        $separator = $line.IndexOf('=')
        if ($separator -lt 1) {
            continue
        }
        $name = $line.Substring(0, $separator)
        $value = $line.Substring($separator + 1)
        Set-Item -Path "Env:$name" -Value $value
    }
}

if (Test-Path $derivedDir) {
    Remove-Item -LiteralPath $derivedDir -Recurse -Force
}
New-Item -ItemType Directory -Path $derivedDir | Out-Null
Copy-Item -Path (Join-Path $sourceDir '*') -Destination $derivedDir -Recurse -Force

$headerPath = Join-Path $derivedDir 'yaYUL.h'
$header = Get-Content -LiteralPath $headerPath -Raw
$header = $header.Replace('#define VALID_ADDRESS ( { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } )', '#define VALID_ADDRESS ((Address_t){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })')
$header = $header.Replace('#define REG(n) ( { 0, 0, 1, n, 1, 0, 1, 0, 0, 0, 0, 0, n, 0 })', '#define REG(n) ((Address_t){ 0, 0, 1, n, 1, 0, 1, 0, 0, 0, 0, 0, n, 0 })')
$header = $header.Replace('#define CONSTANT(n) ( { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, n, 0 })', '#define CONSTANT(n) ((Address_t){ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, n, 0 })')
$header = $header.Replace('#define FIXEDADD(n) ( { 0, 0, 1, n, 0, 1, 1, 0, 0, 0, 0, 0, n, 0 })', '#define FIXEDADD(n) ((Address_t){ 0, 0, 1, n, 0, 1, 1, 0, 0, 0, 0, 0, n, 0 })')
Set-Content -LiteralPath $headerPath -Value $header -NoNewline

Import-CmdEnvironment -BatchFile $vsDevCmd -Arguments '-host_arch=x64 -arch=x64'

if (Test-Path $outputExe) {
    Remove-Item -LiteralPath $outputExe -Force
}
Get-ChildItem -Path $derivedDir -File | Where-Object { $_.Extension -in '.obj', '.o' } | Remove-Item -Force

$sources = Get-ChildItem -Path $derivedDir -Filter '*.c' | Sort-Object Name
if ($sources.Count -eq 0) {
    throw "No yaYUL source files found in $derivedDir"
}

$compileArgs = @(
    '--target=x86_64-pc-windows-msvc',
    '-std=gnu89',
    '-DMSC_VS',
    '-D_CRT_SECURE_NO_WARNINGS',
    '-DNVER="derived-local"',
    '-O2',
    '-c'
) + ($sources | ForEach-Object { $_.FullName })

Push-Location $derivedDir
try {
    & $clang @compileArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Clang compilation failed."
    }

    $objects = Get-ChildItem -Path $derivedDir -File | Where-Object { $_.Extension -in '.obj', '.o' } | Sort-Object Name
    if ($objects.Count -ne $sources.Count) {
        throw "Expected $($sources.Count) object files, found $($objects.Count)."
    }

    $linkArgs = @(
        '--target=x86_64-pc-windows-msvc',
        '-fuse-ld=link',
        '-o',
        $outputExe
    ) + ($objects | ForEach-Object { $_.FullName })

    & $clang @linkArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Link failed."
    }
}
finally {
    Pop-Location
}

if (-not (Test-Path $outputExe)) {
    throw "Failed to produce $outputExe"
}

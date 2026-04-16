$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $repoRoot 'third_party\apollo\apollo11\lm\luminary099'
$workDir = Join-Path $repoRoot 'third_party\_derived_tools\luminary099_listing_work'
$outputDir = Join-Path $repoRoot 'third_party\_derived_tools'
$yaYulExe = Join-Path $outputDir 'yaYUL.exe'
$listingPath = Join-Path $outputDir 'Luminary099.lst'
$extractPath = Join-Path $outputDir 'luminary099_executive_listing_extract.txt'

if (-not (Test-Path $yaYulExe)) {
    throw "Missing $yaYulExe. Build the derived yaYUL helper first."
}

if (Test-Path $workDir) {
    Remove-Item -LiteralPath $workDir -Recurse -Force
}
New-Item -ItemType Directory -Path $workDir | Out-Null
Copy-Item -Path (Join-Path $sourceDir '*') -Destination $workDir -Recurse -Force

Push-Location $workDir
try {
    & $yaYulExe --force MAIN.agc | Set-Content -LiteralPath $listingPath
}
finally {
    Pop-Location
}

$extractPatterns = 'CHANJOB|ENDPRCHG|JOBSLP1|NUCHANG2|DUMMYJOB|ADVAN|SELFBANK|NUDIRECT|SUPDXCHZ'
rg -n $extractPatterns $listingPath | Set-Content -LiteralPath $extractPath

if (-not (Test-Path $listingPath)) {
    throw "Failed to produce $listingPath"
}

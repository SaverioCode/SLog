$ErrorActionPreference = "Stop"

$ValidationFilePath = Join-Path $PSScriptRoot "..\README.md"

$Branch = git branch --show-current
if ([string]::IsNullOrWhiteSpace($Branch)) {
    Write-Host "No branch detected."
    exit 0
}

$Content = Get-Content $ValidationFilePath -Raw

$NewContent = $Content -replace "(\?branch=)[^&]+", "`$1$Branch"

if ($Content -ne $NewContent) {
    Set-Content -Path $ValidationFilePath -Value $NewContent -NoNewline
    git add $ValidationFilePath
    Write-Host "UPDATED README badges to branch: $Branch"
}

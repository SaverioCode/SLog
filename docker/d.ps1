param (
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Tag
)

$BaseDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ParentDir = (Get-Item $BaseDir).Parent.FullName
$DockerSlogDir = "/home/dev/slog"

docker run -it --rm `
    -v "${ParentDir}/include:${DockerSlogDir}/include" `
    -v "${ParentDir}/src:${DockerSlogDir}/src" `
    -v "${ParentDir}/tests:${DockerSlogDir}/tests" `
    -v "${ParentDir}/example:${DockerSlogDir}/example" `
    -v "${ParentDir}/CMakeLists.txt:${DockerSlogDir}/CMakeLists.txt" `
    "slog:${Tag}" bash

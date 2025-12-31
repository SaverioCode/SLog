param (
    [Parameter(Mandatory=$true, Position=0)]
    [string]$Tag
)

$BaseDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ParentDir = (Get-Item $BaseDir).Parent.FullName

docker run -it --rm `
    -v "${ParentDir}/include:/home/dev/slogger/include" `
    -v "${ParentDir}/tests:/home/dev/slogger/tests" `
    "slogger:${Tag}" bash

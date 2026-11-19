<#
.SYNOPSIS
Dumps an S2 map, converts it to H1, builds the H1 zone into usermaps and starts h1-mod.

.DESCRIPTION
Run this script from any folder. Pass the H1 and S2 game folders. The script runs
these steps in the H1 folder: dump and convert, junction, CSV copy, build, launch.
It stops at the first failed step and prints the exit code.
The logs go to <H1 folder>\zonetool_logs\.

.EXAMPLE
.\s2_hub_to_h1.ps1 -H1Path "C:\Program Files (x86)\Steam\steamapps\common\Call of Duty Modern Warfare Remastered (1.15)" -S2Path "D:\SteamLibrary\steamapps\common\Call of Duty WWII"
#>
param(
	[Parameter(Mandatory = $true)]
	[string]$H1Path,

	[Parameter(Mandatory = $true)]
	[string]$S2Path,

	[string]$Zone = "mp_hub_allies",

	[switch]$SkipDump,

	[switch]$NoLaunch
)

$ErrorActionPreference = "Stop"

$zonetool = Join-Path $H1Path "zonetool.exe"
$dumpFolder = Join-Path $H1Path "dump\$Zone"
$zoneToolFolder = Join-Path $H1Path "zonetool\$Zone"
$csvSource = Join-Path $dumpFolder "$Zone.csv"
$csvTarget = Join-Path $H1Path "zone_source\$Zone.csv"
$logFolder = Join-Path $H1Path "zonetool_logs"

function Invoke-Step {
	param(
		[string]$Name,
		[string]$FileName,
		[scriptblock]$Action
	)

	$log = Join-Path $logFolder "$Zone`_$FileName.log"
	Write-Host "== $Name (log: $log)"
	& $Action *>&1 | Tee-Object -FilePath $log
	if ($LASTEXITCODE -ne 0)
	{
		Write-Host "FAILED: $Name returned exit code $LASTEXITCODE"
		exit $LASTEXITCODE
	}
}

if (-not (Test-Path -LiteralPath $H1Path -PathType Container))
{
	Write-Host "FAILED: H1 folder not found: $H1Path"
	exit 1
}

if (-not (Test-Path -LiteralPath $zonetool))
{
	Write-Host "FAILED: zonetool.exe is not in $H1Path"
	exit 1
}

if (-not (Test-Path -LiteralPath $S2Path -PathType Container))
{
	Write-Host "FAILED: S2 folder not found: $S2Path"
	exit 1
}

New-Item -ItemType Directory -Force -Path $logFolder | Out-Null

Push-Location $H1Path
try
{
	if (-not $SkipDump)
	{
		Invoke-Step -Name "Dump and convert" -FileName "dump" -Action {
			& $zonetool -game s2 -path $S2Path dumpzone $Zone --target h1 -target_path $H1Path
		}
	}

	if (-not (Test-Path -LiteralPath $csvSource))
	{
		Write-Host "FAILED: dump is missing $csvSource"
		exit 1
	}

	if (-not (Test-Path -LiteralPath $zoneToolFolder))
	{
		Write-Host "== Junction zonetool\$Zone"
		$junction = & cmd /c mklink /J "$zoneToolFolder" "$dumpFolder"
		$junction
		if ($LASTEXITCODE -ne 0)
		{
			Write-Host "FAILED: junction failed with exit code $LASTEXITCODE"
			exit $LASTEXITCODE
		}
	}

	Write-Host "== Copy $csvSource to $csvTarget"
	Copy-Item -LiteralPath $csvSource -Destination $csvTarget -Force

	Invoke-Step -Name "Build zone" -FileName "build" -Action {
		& $zonetool -game h1 buildzone $Zone -use_usermaps
	}

	if (-not $NoLaunch)
	{
		Write-Host "== Start h1-mod"
		Start-Process -FilePath (Join-Path $H1Path "h1-mod.exe") -WorkingDirectory $H1Path `
			-ArgumentList "-multiplayer", "+map", $Zone
	}
}
finally
{
	Pop-Location
}

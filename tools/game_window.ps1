param(
	[Parameter(Mandatory)][ValidateSet("shot", "click", "type", "key", "rect", "focus")][string]$action,
	[string]$arg1,
	[string]$arg2,
	[string]$process_name = "h1-mod"
)

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class game_window
{
	[StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
	[StructLayout(LayoutKind.Sequential)] public struct POINT { public int x, y; }
	[DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
	[DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
	[DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
	[DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
	[DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
	[DllImport("user32.dll")] public static extern void mouse_event(uint f, uint x, uint y, uint d, IntPtr e);
	[DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint f, IntPtr e);
	[DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
	[DllImport("user32.dll")] public static extern uint MapVirtualKey(uint code, uint type);
}
"@

$process = Get-Process $process_name -ErrorAction SilentlyContinue | Where-Object { $_.MainWindowHandle -ne 0 } | Select-Object -First 1
if (-not $process) { Write-Output "no $process_name window"; exit 1 }
$hwnd = $process.MainWindowHandle

function focus_game
{
	if ([game_window]::GetForegroundWindow() -ne $hwnd)
	{
		[game_window]::ShowWindow($hwnd, 9) | Out-Null
		[game_window]::SetForegroundWindow($hwnd) | Out-Null
		Start-Sleep -Milliseconds 400
	}
}

function client_rect
{
	$r = New-Object game_window+RECT
	[game_window]::GetClientRect($hwnd, [ref]$r) | Out-Null
	$p = New-Object game_window+POINT
	[game_window]::ClientToScreen($hwnd, [ref]$p) | Out-Null
	return @{ x = $p.x; y = $p.y; w = $r.right; h = $r.bottom }
}

function press([byte]$vk)
{
	$scan = [byte][game_window]::MapVirtualKey($vk, 0)
	[game_window]::keybd_event($vk, $scan, 0, [IntPtr]::Zero)
	Start-Sleep -Milliseconds 30
	[game_window]::keybd_event($vk, $scan, 2, [IntPtr]::Zero)
	Start-Sleep -Milliseconds 30
}

switch ($action)
{
	"rect" { client_rect | ConvertTo-Json -Compress }
	"focus" { focus_game }
	"shot"
	{
		focus_game
		$c = client_rect
		$bitmap = New-Object System.Drawing.Bitmap $c.w, $c.h
		$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
		$graphics.CopyFromScreen($c.x, $c.y, 0, 0, $bitmap.Size)
		$scale = if ($arg2) { [double]$arg2 } else { 0.5 }
		$small = New-Object System.Drawing.Bitmap ([int]($c.w * $scale)), ([int]($c.h * $scale))
		$g2 = [System.Drawing.Graphics]::FromImage($small)
		$g2.InterpolationMode = "HighQualityBicubic"
		$g2.DrawImage($bitmap, 0, 0, $small.Width, $small.Height)
		$small.Save($arg1, [System.Drawing.Imaging.ImageFormat]::Jpeg)
		Write-Output "$($c.w)x$($c.h) saved at scale $scale"
	}
	"click"
	{
		focus_game
		$c = client_rect
		[game_window]::SetCursorPos($c.x + [int]$arg1, $c.y + [int]$arg2) | Out-Null
		Start-Sleep -Milliseconds 150
		[game_window]::mouse_event(2, 0, 0, 0, [IntPtr]::Zero)
		Start-Sleep -Milliseconds 60
		[game_window]::mouse_event(4, 0, 0, 0, [IntPtr]::Zero)
	}
	"key"
	{
		focus_game
		foreach ($name in $arg1.Split(","))
		{
			$vk = switch ($name)
			{
				"enter" { 0x0D } "esc" { 0x1B } "tilde" { 0xC0 } "space" { 0x20 } "tab" { 0x09 } "back" { 0x08 }
				default { [int][char]$name.ToUpper() }
			}
			press ([byte]$vk)
		}
	}
	"type"
	{
		focus_game
		Add-Type -AssemblyName System.Windows.Forms
		$escaped = [regex]::Replace($arg1, '[+^%~(){}\[\]]', '{$0}')
		[System.Windows.Forms.SendKeys]::SendWait($escaped)
	}
}

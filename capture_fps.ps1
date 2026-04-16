Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
Start-Process '.\build\bin\DEBUG\FPSDemo.exe' -NoNewWindow
Start-Sleep -Seconds 3
$bmp = New-Object Drawing.Bitmap([Windows.Forms.Screen]::PrimaryScreen.Bounds.Width, [Windows.Forms.Screen]::PrimaryScreen.Bounds.Height)
$graphics = [Drawing.Graphics]::FromImage($bmp)
$graphics.CopyFromScreen(0, 0, 0, 0, $bmp.Size)
$bmp.Save("E:\Code\CPP\CaninoEngine\screenshot.jpg")
$graphics.Dispose()
$bmp.Dispose()
Stop-Process -Name 'FPSDemo' -Force

Add-Type -AssemblyName System.Drawing
$bmp = New-Object System.Drawing.Bitmap("E:\Code\CPP\CaninoEngine\assets\floor_texture.jpg")

$blackPixels = 0
$totalPixels = $bmp.Width * $bmp.Height

for ($x = 0; $x -lt $bmp.Width; $x += 50) {
    for ($y = 0; $y -lt $bmp.Height; $y += 50) {
        $color = $bmp.GetPixel($x, $y)
        if ($color.R -lt 10 -and $color.G -lt 10 -and $color.B -lt 10) {
            $blackPixels++
        }
    }
}

Write-Output "Image is $($bmp.Width) x $($bmp.Height)"
Write-Output "Black pixels sampled: $blackPixels"

$bmp.Dispose()

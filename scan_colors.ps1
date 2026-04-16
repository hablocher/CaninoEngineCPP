Add-Type -AssemblyName System.Drawing
$bmp = New-Object System.Drawing.Bitmap("E:\Code\CPP\CaninoEngine\screenshot.jpg")

$colorCounts = @{}

for ($x = 0; $x -lt $bmp.Width; $x += 10) {
    for ($y = 0; $y -lt $bmp.Height; $y += 10) {
        $color = $bmp.GetPixel($x, $y)
        $rgb = "$($color.R),$($color.G),$($color.B)"
        if ($colorCounts.ContainsKey($rgb)) {
            $colorCounts[$rgb]++
        } else {
            $colorCounts[$rgb] = 1
        }
    }
}

$colorCounts.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 15

$bmp.Dispose()

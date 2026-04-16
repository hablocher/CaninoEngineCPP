from PIL import Image
from collections import Counter

img = Image.open('screenshot.jpg')
pixels = list(img.getdata())

# Count colors, ignore very common dark colors (like pure black or desktop backgrounds)
counts = Counter(pixels)

print("Most common colors across the entire screen:")
for color, count in counts.most_common(15):
    print(f"RGB {color}: {count} pixels")

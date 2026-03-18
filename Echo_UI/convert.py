from PIL import Image

img = Image.open("input.bmp").convert("1")  # 1bit化
width, height = img.size

data = []
pixels = img.load()

for y in range(height):
    for x in range(0, width, 8):
        byte = 0
        for bit in range(8):
            if x + bit < width:
                # 白=0, 黒=1 にするならここ調整
                pixel = pixels[x + bit, y]
                if pixel == 0:  # 黒
                    byte |= (1 << (7 - bit))
        data.append(byte)

# C配列出力
print(f"static const uint8_t image_data[{len(data)}] = {{")
for i in range(0, len(data), 16):
    line = ", ".join(f"0x{b:02x}" for b in data[i:i+16])
    print("    " + line + ",")
print("};")

import json
import os
import struct
path = input("pls input song data folder\n>>>")
name = input("song name pls\n>>>")
difficulty = input("difficulty pls\n>>>")
chart_path = os.path.join(path, name+"-chart.json")
output = bytearray()
if os.path.exists(chart_path):
    with open(chart_path, "r") as f:
        chart = json.load(f)
    output += (chart["scrollSpeed"][difficulty].to_bytes(2, "little"))
    output += (len(chart["notes"][difficulty]).to_bytes(4, "little"))
    for notes in chart["notes"][difficulty]:
        output += (struct.pack('<f', notes["t"]))
        note_len = 0.0
        if "l" in notes:
            note_len = notes["l"]
        output += (struct.pack('<f', note_len))
        output += (notes["d"].to_bytes(1, "little"))
    with open("chart.bin", "wb") as f:
        f.write(output)
else:
    print("Error")
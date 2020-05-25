#! /usr/bin/python3

import sys, string
from PIL import ImageFont, ImageDraw, Image

def get_char_pixels(fontname, c, size):
    #image = Image.new('RGBA', (size, size), (255,255,255,0))
    image = Image.new('RGB', (size, size), (0,0,0))
    draw = ImageDraw.Draw(image)
    # use a truetype font
    font = ImageFont.truetype(fontname, int(size*1.5))
    draw.text((0, -5), c, font=font)

    image.show()
    return image.load();

def generate_header_head(size):
    return "#ifndef _FONT_H_ \n\
#define _FONT_H_ string.ascii_letters \n\
\n\
#define FONT_WIDTH {}\n\
#define FONT_HEIGHT {}\n\
\n\
".format(size, size)

def generate_header_tail():
    return "#endif //_FONT_H_ \n"


def generate_c_array(char, size, pixels):
    res = "{{ // {}\n".format(char)
    for y_row in range(0, int(size/8)):
        for x in range(0, int(size)):
            v = 0
            for y in range(0, 8):
                if pixels[x, y_row*8+y] != (0, 0, 0):
                    v |= (1<<y)
            res += "0x{:02x}, ".format(v)
        res += '\n'
    res += '},\n'
    return res

def generate_font_header(fontname, size, char_tables):
    result = generate_header_head(size)

    for tables in char_tables:
        result += "\nconst unsigned char font_table_{}[][FONT_WIDTH*FONT_HEIGHT/8] = {{\n".format(tables[0])
        for ch in tables[1]:
            result += generate_c_array(ch, size, get_char_pixels(fontname, ch, size))
        result += "};\n"
    result += generate_header_tail()
    return result
if len(sys.argv) < 2:
    print("no enought param, needed 2 - <file.ttf> <size>")
    exit(1)

#char_table =  [("digits", string.digits),
#               ("uppercase", string.ascii_uppercase),
#               ("lowercase", string.ascii_lowercase)]
#char_table =  [("digits", string.digits),
#               ("letters", string.ascii_uppercase)]
#char_table =  [("letters", ['A'])]
#char_table =  [("fractions", ['\u00BD', '\u00BC','\u215B'])]
#char_table =  [("fractions", ['\u00'])]


print(generate_font_header(sys.argv[1], int(sys.argv[2]), char_table))

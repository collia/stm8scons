#! /usr/bin/python3

import sys, string, unicodedata
from PIL import ImageFont, ImageDraw, Image

def get_char_pixels(fontname, c, size):
    #image = Image.new('RGBA', (size, size), (255,255,255,0))
    image = Image.new('RGB', (size, size), (0,0,0))
    draw = ImageDraw.Draw(image)
    # use a truetype font
    font = ImageFont.truetype(fontname, int(size*1.5))
    draw.text((0, -(int(size*0.3))), c, font=font)

    #image.show()
    return image.load();

def generate_header_head():
    return "#ifndef _FONT_H_ \n\
#define _FONT_H_ \n\
\n\
"

def generate_source_head():
    return "#include \"font.h\" \n\
\n\
"

def generate_header_tail():
    return "#endif //_FONT_H_ \n"

def generate_font_size_defines(name, size):
        return "\n#define {}_FONT_WIDTH {} \n\
#define {}_FONT_HEIGHT {} \n\
\n\
".format(name.upper(), size, name.upper(), size)

def get_char_name(ch):
    return unicodedata.name(ch).replace('-','_').replace(' ', '_')

def generate_c_array(char, size, pixels):
    res = " // {}\n".format(char)
    for y_row in range(0, int(size/8)):
        for x in range(0, int(size)):
            v = 0
            for y in range(0, 8):
                if pixels[x, y_row*8+y] != (0, 0, 0):
                    v |= (1<<y)
            res += "0x{:02x}, ".format(v)
        res += '\n'
    return res

def generate_font_files(fontname, symbols):
    result_header = generate_header_head()
    result_source = generate_source_head()

    fonts_enum = "typedef enum {\n"

    for table in symbols:
        name = table[0]
        size = table[1]
        chars = table[2]

        fonts_enum += "\t{}_FONT,\n".format(name.upper())

        char_table_enum = "typedef enum {\n"
        char_table_definition = ""
        char_table = "const unsigned char *{}_font_char_table[] = {{\n".format(name.lower())

        result_header += generate_font_size_defines(name, size)

        for ch in chars:
            char_table_definition += "\nstatic const unsigned char {}_font_{}[{}_FONT_WIDTH*{}_FONT_HEIGHT/8] = {{\n".format(
                 name.lower(),
                 get_char_name(ch),
                 name.upper(), name.upper())
            char_table_definition += generate_c_array(ch, size, get_char_pixels(fontname, ch, size))
            char_table_definition += "};\n"

            char_table_enum += "\t{}_FONT_{},\n".format(
                name.upper(),
                get_char_name(ch))
            char_table += "\t{}_font_{},\n".format(
                name.lower(), get_char_name(ch))
        char_table_enum += "\t{}_FONT_MAX,\n}} {}_chars;\n".format(
            name.upper(), name.lower())
        char_table += "};\n"

        result_header += char_table_enum
        result_header += "extern const unsigned char *{}_font_char_table[];\n".format(name.lower())
        result_source += char_table_definition
        result_source += char_table

    fonts_enum += "} fonts;\n"
    result_header += fonts_enum
    result_header += generate_header_tail()
    return (result_header, result_source)



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

big_symbol_table = ("big", 16,  "ACLEXPTIMU"+" ")
small_symbol_table = ("small", 8,  string.digits+string.ascii_uppercase+'-+_ ')
#big_symbol_table = ("big", 16, "A")
#small_symbol_table = ("big", 8, "a")

files = generate_font_files(sys.argv[1], [big_symbol_table, small_symbol_table])

for f in files:
    print(f)

with open("font.h", 'w') as f:
    f.write(files[0])
with open("font.c", 'w') as f:
    f.write(files[1])

#!python

import os
env = Environment(ENV = os.environ)
 
env['CC'] = 'sdcc'
env['CXX'] = 'sdcc'
env['LINK'] = 'sdcc'
env['OBJSUFFIX'] = '.rel'
env['PROGSUFFIX'] = '.ihx'

stm_device='STM8S003'

# include locations
env.Append(CPPPATH = [
    '#inc', 
    '#inc/stm8s/inc',
    '#font',
    ])

# compiler flags
env.Append(CCFLAGS = [
    '-mstm8',
    '-lstm8',
    '--opt-code-size',
    '--std-c99',
    ])


env.Append(LINKFLAGS = [
    '-mstm8',
    '-lstm8',
    '--out-fmt-ihx',
    ])

# defines
env.Append(CPPDEFINES = [
    stm_device,
    "SSD1306_128_64",
    #"DEBUG"
])


env.VariantDir('build/stm8/', 'src/stm8s/src', duplicate=0)
env.VariantDir('build/src/', 'src', duplicate=0)
env.VariantDir('build/font/', 'font', duplicate=0)


#print(env.Dump())
# build everything
prg = env.Program(
    target = 'main',
    source = [
        'build/src/main.c',
        'build/src/terminal.c',
        'build/src/exponometer.c',
        'build/src/buttons.c',
        'build/src/message.c',
        'build/src/time.c',
        'build/src/debug.c',
        'build/src/i2c.c',
        'build/src/ssd1306.c',
        'build/src/max44009.c',
        'build/src/stm8s_it.c',
        'build/stm8/stm8s_gpio.c',
        'build/stm8/stm8s_i2c.c',
        'build/stm8/stm8s_clk.c',
        'build/stm8/stm8s_tim4.c',
        'build/stm8/stm8s_exti.c',
        'build/font/font.c'
    ]
)


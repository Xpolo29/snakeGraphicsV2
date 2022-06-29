# snakeGraphicsV2
Always winning snake using a Hamiltonian cycle generator

Cycle generation only works on even_number x even_number map.
Some sizes don't work, either change start position or map size in HMap.h file.

Uses windows dll so might not work on some VM.

Compiling with -Ofast is recommended as cycle generation takes ~3-10 mins on 200x100 map with ryzen 5 5600x.

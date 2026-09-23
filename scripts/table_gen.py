import math
import random

pi = 3.1415926                 # pie 
fidelity = 24                  # set fidelity 
table_size = 256               # set table size
amplitude = (2 ** (fidelity)) - 1  # 2^fidelity - 1

print("SINE WAVE\n\n\n")

print("{", end="")
for k in range(table_size):
    print(str(round(amplitude/2 * math.sin(2*pi * (k / table_size)))), end=",")
print("};",end="")

print("\n\n\nSQUARE WAVE\n\n\n")

print("{", end="")
for k in range(table_size):
    if(k < table_size / 2):
        print(str(round(amplitude/2)-1), end=",")
    else:
        print(str(round(-amplitude/2)+1), end=",")
print("};", end="")

print("\n\n\nNOISE\n\n\n")

print("{", end="")
for k in range(table_size):
    print(str(round(amplitude * random.random())), end=",")
print("};", end="")
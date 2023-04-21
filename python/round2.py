import sys

def round_pow_2(n):
    x = n
    x -= 1
    x |= x >> 1
    x |= x >> 2
    x |= x >> 4
    x |= x >> 8
    x |= x >> 16
    x |= x >> 32
    x += 1
    return x

print(round_pow_2(int(sys.argv[1])))

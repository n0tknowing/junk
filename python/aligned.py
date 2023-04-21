# align must be power of two
def align_to(n, align):
    if n < 0 or align < 0:
        raise ValueError("both n and align must be positive number")
    elif align > 0 and (align & (align - 1)) != 0:
        raise ValueError("align must be power of two")
    _align_to = align - 1
    return (n + _align_to) & ~_align_to

import sys

print(align_to(11, 2))
print(align_to(11, 8))
print(align_to(7823, 4096))

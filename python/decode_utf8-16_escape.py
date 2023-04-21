import sys

def decode_utf8_escape(s):
    if type(s) != bytes:
        raise TypeError("bytes expected")
    if len(s) != 5:
        raise ValueError("5 bytes expected")
    if s[0] != 0x75:
        raise ValueError("escaped string must be started by 'u'")
    value = 0
    for i in range(1, 5):
        c = s[i]
        value <<= 4
        if c >= 0x30 and c <= 0x39:
            value += c - 0x30
        elif c >= 0x61 and c <= 0x66:
            value += c - 0x61
        elif c >= 0x41 and c <= 0x46:
            value += c - 0x41
        else:
            return -1
    return value

if len(sys.argv) < 2:
    exit(1)

print(hex(decode_utf8_escape(bytes(sys.argv[1], "ascii"))))

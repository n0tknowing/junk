import sys
from base64 import b64encode as b64e_py

b64tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

def b64e_me(s):
    sb = bytes(s, "utf-8")
    sl = len(sb)
    out = bytearray()
    i = 0
    while sl >= 3:
        out += bytes(b64tab[sb[i] >> 0x02], "utf-8")
        out += bytes(b64tab[((sb[i] & 0x03) << 0x04) | (sb[i+1] >> 0x04)], "utf-8")
        out += bytes(b64tab[((sb[i+1] & 0x0F) << 0x02) | ((sb[i+2] & 0xC0) >> 0x06)], "utf-8")
        out += bytes(b64tab[sb[i+2] & 0x3F], "utf-8")
        sl -= 3
        i += 3
    if sl > 0:
        out += bytes(b64tab[sb[i] >> 0x02], "utf-8")
        if sl == 1:
            out += bytes(b64tab[(sb[i] & 0x03) << 0x04], "utf-8")
            out += b"="
            out += b"="
        else:
            out += bytes(b64tab[((sb[i] & 0x03) << 0x04) | ((sb[i+1] & 0xF0) >> 0x04)], "utf-8")
            out += bytes(b64tab[(sb[i+1] & 0x0F) << 0x02], "utf-8")
            out += b"="
    return bytes(out)

if len(sys.argv) < 2:
    exit(1)

print("b64e_me =", b64e_me(sys.argv[1]))
print("b64e_py =", b64e_py(bytes(sys.argv[1], "utf-8")))

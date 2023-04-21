class BitPack:
    def __init__(self):
        self.acc = 0

    def __repr__(self):
        return "{:#018x}".format(self.acc)

    def pack32(self, v0, v1, v2, v3):
        assert v0 >= 0 and v0 <= 255
        assert v1 >= 0 and v1 <= 255
        assert v2 >= 0 and v2 <= 255
        assert v3 >= 0 and v3 <= 255
        self.acc = v0
        self.acc = (self.acc << 8) | v1
        self.acc = (self.acc << 8) | v2
        self.acc = (self.acc << 8) | v3
        return self.acc

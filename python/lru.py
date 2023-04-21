class LRU:
    def __init__(self, max_lru=16):
        self._list = []
        self._max_lru = max_lru

    def __repr__(self):
        return repr(self._list)

    def clear(self):
        return self._list.clear()

    def lookup(self, v):
        if v in self._list:
            if self._list[-1] != v:
                self._list.remove(v)
                self._list.append(v)
            return (True, v)
        return (False, None)

    def insert(self, v):
        exist, _ = self.lookup(v)
        if not exist:
            if len(self._list) == self._max_lru:
                self._list.pop(0)
            self._list.append(v)

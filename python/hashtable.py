class _KV:
    def __init__(self, key, value, keyhash):
        self.key = key
        self.keyhash = keyhash
        self.value = value
        self.tombstone = False

class HashTable:
    def __init__(self):
        self._table = [ None ] * 8
        self._count = 0
        self._capacity = 8
        self._loadfactor = int(self._capacity * 0.75)

    def __len__(self):
        return self._count

    def __setitem__(self, key, value):
        self.set(key, value)

    def __getitem__(self, key):
        return self.get(key)

    def __delitem__(self, key):
        self.delete(key)

    def _resize(self):
        new_capacity = self._capacity * 2
        new_loadfactor = int(new_capacity * 0.75)
        new_table = [ None ] * new_capacity
        for i in range(self._capacity):
            if self._table[i] == None or self._table[i].tombstone:
                continue
            j = self._table[i].keyhash % new_capacity
            for _ in range(new_capacity):
                if new_table[j] == None:
                    new_table[j] = self._table[i]
                    break
                j += 1
                j %= new_capacity
        self._capacity = new_capacity
        self._loadfactor = new_loadfactor
        self._table = new_table

    def get(self, key):
        keyhash = hash(key)
        idx = keyhash % self._capacity
        for _ in range(self._capacity):
            kv = self._table[idx]
            if kv == None:
                raise KeyError("{}".format(key))
            elif not kv.tombstone and kv.keyhash == keyhash and kv.key == key:
                return kv.value
            idx += 1
            idx %= self._capacity
        raise KeyError("{}".format(key))

    def set(self, key, value):
        if self._count == self._loadfactor:
            self._resize()
        keyhash = hash(key)
        kv = _KV(key, value, keyhash)
        idx = keyhash % self._capacity
        for _ in range(self._capacity):
            if self._table[idx] == None or self._table[idx].tombstone:
                self._table[idx] = kv
                self._count += 1
                return
            elif self._table[idx].keyhash == keyhash and self._table[idx].key == key:
                self._table[idx].value = value
                return
            idx += 1
            idx %= self._capacity

    def delete(self, key):
        if self._count == 0:
            raise KeyError("{}".format(key))
        keyhash = hash(key)
        idx = keyhash % self._capacity
        for _ in range(self._capacity):
            if self._table[idx] == None:
                raise KeyError("{}".format(key))
            elif not self._table[idx].tombstone and self._table[idx].keyhash == keyhash and self._table[idx].key == key:
                self._table[idx].tombstone = True
                self._count -= 1
                return
            idx += 1
            idx %= self._capacity

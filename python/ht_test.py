import hashtable

d = hashtable.HashTable()
for i in range(50):
    d.set("key" + str(i), "Hello" + str(i))

print(d.len())

for i in range(50):
    print(d.get("key" + str(i)))

for i in range(45,50):
    d.delete("key" + str(i))

print(d.len())
for i in range(45):
    print(d.get("key" + str(i)))

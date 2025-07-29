

word = [0xffffe001ffff, 0x3fffc0004000,0xffffffffdfbf,0xbfffffffff07,]
cleared = [0xfffee000d7ff, 0xdffc0004000, 0xffffffffd7af, 0xa77fffffff07]
allocated = [0] * 4
tags = []

def bits_invert(n: int) -> int:
    return n ^ ((1 << 64) - 1)
size = len(word)
base = 0
total = 0
for i in range(0, size):
    w = word[i]
    c = cleared[i]
    a = w & bits_invert(c)
    allocated[i] = a

    j = 0
    for j in range(0, 64):
        if  (a >> j) & 1:
            tags.append(j+base)
            total += 1

    base += 64

print([hex(i) for i in allocated])
print("total allocated requests: %d, and their tags are: "%(total))
print([i for i in tags])

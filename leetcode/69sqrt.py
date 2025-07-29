def mySqrt(x: int) -> int:
    l = 0
    r = x
    rv =0
    while l <= r:
        m = l + (r-l) // 2
        mv= m*m
        #print((m, mv, x,l, r))
        if mv <= x:
            l = m + 1
            rv = m
        else:
            r = m -1

    print((m, mv, x,rv, l, r))
    return rv

for i in range(0, 120):
    mySqrt(i)

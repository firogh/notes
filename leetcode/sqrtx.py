#!/bin/env python3

class Solution:
    def mySqrtw(self, x: int) -> int:
        low = 0 
        high = x 
        mid = (low + high) // 2
        while low <= high:
            print(low, mid, high)
            mid = (low + high) // 2
            if mid * mid < x:  
                low = mid + 1 
            elif mid * mid > x:
                high = mid - 1 
            else:
                break

        print(mid)
        return mid

    def mySqrt1(self, x: int) -> int:
        low = 0 
        high = x
        rv = 0
        if x == 1:
            return 1
        while low <= high:
            mid = (low + high) // 2
            power = mid * mid
            rv = mid
            if power < x:
                low = mid + 1
            elif power > x:
                high = mid - 1
                rv = high
            else:
                break
        return rv

    def mySqrt2(self, x: int) -> int:
        low = 0 
        high = x  
        while True:
            mid = (low + high) // 2
            power = mid * mid
            if power == x:
                return mid
            elif power < x:  
                low = mid  + 1
                if (low * low)>x:
                    return mid
            elif power > x:
                high = mid - 1 
                if (high *high) <x:
                    return high
          
        return -1
    def mySqrtx(self, x: int) -> int:
        l = 0 
        h = x
        while l <= h:
            m = (l+h)//2
            if (m*m>x):
                h=m-1
            elif (m*m<x):
                l=m+1
            else:
                return m
        return h

    def mySqrtq(self, x: int) -> int:
        l = 0 
        h = x +1
        while l < h:
            m=(l+h)//2
            if (m*m< x):
                l=m+1
            elif (m*m>x):
                h=m
            else:
                return m
        return h-1

    def mySqrt(self, x: int) -> int:
        l = 0 
        h = x
        while l <= h:
            m=(l+h)//2
            print(l, m, h)
            if (m*m <= x):
                l=m+1
            else:
                h=m-1
        return h
s = Solution()
s.mySqrt(1)
print("======\n")
s.mySqrt(8)

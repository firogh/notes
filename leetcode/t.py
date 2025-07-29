def rotate1(s:int, e:int, l:int, nums, k: int) -> None:
    """
    Do not return anything, modify nums in-place instead.
    """
    j = s
    n = nums[j]
    for i in range(0, e):
        cur = n
        print((j, cur))
        j = (j + k) % l
        n = nums[j]
        nums[j] = cur

    return nums

nums = [-1,-100,3,99]
print(nums)
l = len(nums)
k = 2
rotate1(0, int(l/2), l, nums, k)
rotate1(1, int(l/2), l, nums, k)

print(nums)

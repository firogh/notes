j = 1
nums = [-1,-100,3,99]
k =2
n = nums[j]
l = len(nums)
print(nums)
for i in range(0, l):
    cur = n
    print(j)
    j = (j + k) % l
    n = nums[j]
    nums[j] = cur
    print(nums)
print(nums)

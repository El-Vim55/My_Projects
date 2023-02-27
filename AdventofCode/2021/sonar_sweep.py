# --- PART ONE ---
nums = []
cnt = 0
increase_counter = 0

with open('file.txt', 'r') as file:
    for data in file:
        nums.append(int(data))
    
    for i in range(len(nums)):
        nums1 = nums[0+cnt]
        nums2 = nums[1+cnt]
        
        if nums1 > nums2:
            cnt += 1
        elif nums2 > nums1:
            cnt += 1
            increase_counter += 1
            print(increase_counter)

#---------------------------------------


# --- PART TWO ---
def windows_technique(arr=[], k=3):
    cnt = 0
    increase_counter = 0

    with open('file.txt', 'r') as file:
        for data in file:
            arr.append(int(data))

    n = len(arr)
    if n < k:
        return 0

    # move forward linearly, we will start with the first 3 nums in the index
    # from here on, we need to "add" the next number after the 2nd index and negate the first, 0th
    for _ in range(n-2):
        max_sum1 = sum(arr[cnt:k+cnt])
        max_sum2 = sum(arr[cnt+1:k+cnt+1])
        cnt += 1

        if max_sum2 > max_sum1:
            increase_counter += 1

    print(increase_counter)

windows_technique()

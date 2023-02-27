f = open('2021/no.2/positions.txt', 'r').read().split('\n')

def main(pos=0, depth=0):
    for text in f:
        num = text[-1]
        if text == f'forward {num}':
            pos += int(num)
        if text == f'down {num}':
            depth += int(num)
        if text == f'up {num}':
            depth -= int(num)

    return pos * depth

print(main())

# ---------------------------

# // PART 2
# down increases, up decreases
def main(aim=0, pos=0, depth=0):
    for text in f:
        num = int(text[-1])
        if text == f'down {num}':
            aim += num
        if text == f'up {num}':
            aim -= num
        if text == f'forward {num}':
            pos += num

            if aim != 0 and num != 0:
                depth += aim * num
    print(pos * depth)
main()

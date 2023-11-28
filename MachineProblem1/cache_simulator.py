import math

# Function to find the frame that will not be used
# recently in future after given index in pg[0..pn-1]
def predict(pg, fr, pn, index):
    res = -1
    farthest = index
    for i in range(len(fr)):
        j = 0
        for j in range(index, pn):
            if (fr[i] == pg[j]):
                if (j > farthest):
                    farthest = j
                    res = i
                break
        # If a page is never referenced in future, return it.
        if (j == pn):
            return i
    # If all of the frames were not in future, return any of them, we return 0. Otherwise we return res.
    return 0 if (res == -1) else res


def optimalPage(pg, fn):

    # Create an array for given number of frames and initialize it as empty.
    fr = []

    # Traverse through page reference array and check for miss and hit.
    hit = 0
    for idx, c in enumerate(pg):
        if c == ' ':
            continue

        # Page found in a frame : HIT
        if c in fr:
            print("h", end='')
        else:
            # Page not found in a frame : MISS
            # If there is space available in frames.
            if len(fr) < fn:
                fr.append(c)
                print("m", end='')

            # Find the page to be replaced.
            else:
                j = predict(pg, fr, i + 1)
                # fr[j] =
                # print(fr[j], end='')


def fifo(s):
    queue = []
    for c in s:
        if c == ' ':
            continue
        if c in queue:
            print("h", end='')
        else:
            if len(queue) == 4:
                # replace the first one
                print(queue[0], end='')
                queue.pop(0)
                queue.append(c)
            else:
                queue.append(c)
                print('m', end='')


def inverse_MRU(s):
    queue = []
    index = 0
    for c in s:
        if c == ' ':
            continue
        # hit, update index
        if c in queue:
            print('h', end='')
            for i in range(len(queue)):
                if queue[i] == c:
                    index = i
                    break
        else:
            if len(queue) == 4:
                # replace the inverse
                print(queue[3 - index], end='')
                queue[3 - index] = c
                index = 3 - index
            # fill queue
            else:
                queue.append(c)
                print('m', end='')


def pesudo_LRU(s):
    b0 = 0
    b1 = 0
    b2 = 0
    cache = []
    for c in s:
        if c == ' ':
            continue
        # update bits, 0 is left, 1 is right
        if c in cache:
            print('h', end='')
            for index in range(len(cache)):
                if cache[index] == c:
                    if index == 0:
                        b0 = 0
                        b1 = 0
                    elif index == 1:
                        b0 = 0
                        b1 = 1
                    elif index == 2:
                        b0 = 1
                        b2 = 0
                    elif index == 3:
                        b0 = 1
                        b2 = 1
        # not in cache
        else:
            # update, if 0 go right, if 1 go left
            if len(cache) == 4:
                # go right
                if b0 == 0:
                    b0 = 1
                    if b2 == 0:
                        print(cache[3], end='')
                        cache[3] = c
                        b2 = 1
                    else:
                        print(cache[2], end='')
                        cache[2] = c
                        b2 = 0
                # go left
                elif b0 == 1:
                    b0 = 0
                    if b1 == 1:
                        print(cache[0], end='')
                        cache[0] = c
                        b1 = 0
                    else:
                        print(cache[1], end='')
                        cache[1] = c
                        b1 = 1
            # fill cache
            else:
                cache.append(c)
                print('m', end='')

def LRU(s):
    order = 0
    queue = []
    used = [0, 0, 0, 0]
    for idx, c in enumerate(s):
        if c == ' ':
            continue
        # hit
        if c in queue:
            print('h', end='')
            for i in range(len(queue)):
                if queue[i] == c:
                    used[i] = order
                    order += 1
                    break
        # miss
        else:
            if idx > 4:
                lowest = math.inf
                index = 0
                for idx, v in enumerate(used):
                    if v < lowest:
                        lowest = v
                        index = idx
                print(queue[index], end='')
                queue[index] = c
                used[index] = order
                order += 1

            else:
                print('m', end='')
                queue.append(c)
                used[len(queue) - 1] = order
                order += 1

# str2 = "ANNT BT AT BNN ATT BT AN"
def global_global(m, n, s):
    '''
    m: global history with m bit registers
    n: counter table with n bit counter
    s: string of sequence
    '''
    global_history = 0
    counter_table = [1] * (2 ** m)
    branch = 'A'

    for idx, actual in enumerate(s):
        if actual == ' ':
            continue

        if actual == 'A':
            branch = 'A'
        elif actual == 'B':
            branch = 'B'
        
        if actual == 'A' or actual == 'B':
            continue

        if branch == 'A':
            index = (global_history ^ 0xA) % (2 ** m)
        elif branch == 'B':
            index = (global_history ^ 0xB) % (2 ** m)
        
        predict = counter_table[index] >= 2 ** (n - 1)
        # 1. print predict
        if predict:
            print('T', end=', ')
        else:
            print('N', end=', ')

        # update counter
        if actual == 'T':
            if counter_table[index] != 2 ** n - 1:
                counter_table[index] += 1
        else:
            if counter_table[index] != 0:
                counter_table[index] -= 1

        # print counter
        print(counter_table[index], end=', ')

        # update global history
        global_history = global_history << 1
        if actual == 'T':
            global_history += 1
        global_history = global_history % (2 ** m)

        # print last m bits of global history
        print(bin(global_history)[2:].zfill(m), end=', ')

        
        


        if predict != (actual == 'T'):
            print('mispredicted: y', end=', ')
        else:
            print('mispredicted: n', end=', ')
        print('')



def main():

    #str1 = "ABCD AEGH FFDC DEGH EA"

    str2 = "ANNT BT AT BNN ATT BT AN"

    global_global(3, 2, str2)

if __name__ == '__main__':
    main()

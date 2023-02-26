#!/usr/bin/env python3

# number of keys - 1 arg
# file name      - 2 arg

from io import TextIOWrapper
from typing import List
import random, sys

class NumNReq:
    num:int  = 0
    req = 'k'

def put_unique_key(list: List[int]):
    return list.pop(random.randint(0, len(list) - 1))

list: List[NumNReq] = []
num_of_keys = int(sys.argv[1])
abs_max_key = int(num_of_keys / 2)
first_req_num = int(num_of_keys / 10)
num_of_ins = 0

for i in range(2, len(sys.argv)):
    print(i - 1)
    list_unique: List[int] = []
    
    for j in range(-abs_max_key, abs_max_key):
        list_unique.append(j)

    for _ in range(first_req_num):
        list.append((put_unique_key(list_unique), 'k'))
        num_of_ins += 1

    for _ in range(first_req_num, num_of_keys):
        request = random.randint(0, 2)
        if request == 0:
            list.append((put_unique_key(list_unique), 'k'))
            num_of_ins += 1
        elif  request == 1:
            list.append((random.randint(1, num_of_ins), 'm'))
        else:
            list.append((random.randint(-abs_max_key, abs_max_key), 'n'))
    
    file = open(sys.argv[i], 'w')
    for i in range(len(list)):
        num, req = list[i]
        file.write(req + ' ' + str(num) + ' ')
    file.write('\n')
    num_of_ins = 0
    list.clear()
     


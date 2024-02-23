"""
 * REFERENCE IMPLEMENTATION OF block searching programe.
 *
 * Filename: blockfind.py
 *
 * created: 07/12/23
 * updated: 05/01/24
 *
 * by Hiren
 * Research Scholar
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 * This script finds the consecutive PNBs
 * running command: python3/python blockfind.py
"""


listBLOCKs =[] # list form

# Open the file in read mode
with open('europnb.txt', 'r') as file:
    # Read the content and split it into a list of strings
    lines = file.readlines()

    # Create a list to store the numbers
    list = []

    # Iterate through each line
    for index, line in enumerate(lines):
        # Split each line into individual numbers
        line_numbers = line.split()

        # Ignore the last element of the second row
        if index == 0:
            line_numbers = line_numbers[:-1]

        # Convert each string number to an integer and append to the list
        list.extend(map(int, line_numbers))

# list = [3, 4, 5, 6, 47, 48, 49, 67, 68, 69, 70, 71, 72, 73, 74, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 91, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 124, 125, 126, 127, 156, 157, 158, 159, 169, 191, 192, 193, 194, 199, 200, 208, 212, 220, 221, 222, 223, 224, 225, 226, 227, 245, 246, 247] # PNBs

block_len = 4 # how many consecutive elements to find

if block_len > len(list):
    print("Block length is too high")

else:
    list.sort()
    i = 0   
    while i < len(list):
        counter = 1
        block = [list[i]]
        i += 1
        
        while i < len(list) and list[i] - list[i-1] == 1 and counter < block_len:
            block.append(list[i])
            counter+=1
            i += 1
            
        if (counter == block_len):
            for j in range(block_len):
                listBLOCKs.append(block[j])

rest = []
for i in list:
    if i not in listBLOCKs:
        rest.append(i)
         
print(*listBLOCKs, sep=" ", file = open('pnbblock.txt','w')) 

with open('pnbblock.txt', 'a') as file:
    for i in rest:
        file.write(f"{i} ")
    file.write(f"{ len(listBLOCKs)} ")
    file.write(f"{len(rest)} ")
    

if len(rest)+len(listBLOCKs)== len(list):
    print("Ok")

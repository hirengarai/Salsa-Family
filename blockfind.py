"""
 * REFERENCE IMPLEMENTATION OF block searching programe.
 *
 * Filename: blockfind.py
 *
 * created: 07/12/23
 * updated: 20/12/23
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


list = [3, 4, 5, 6, 47, 48, 49, 67, 68, 69, 70, 71, 72, 73, 74, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 91, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 115, 124, 125, 126, 127, 156, 157, 158, 159, 169, 191, 192, 193, 194, 199, 200, 208, 212, 220, 221, 222, 223, 224, 225, 226, 227, 245, 246, 247] # PNBs

# print(f"The initial length of the list: {len(list)}")

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
            # print(block)
            counter+=1
            i += 1
            
        if (counter == block_len):
            # BLOCKs.append(block[:block_len])
            for j in range(block_len):
                listBLOCKs.append(block[j])
# print(BLOCKs)
# print( len(listBLOCKs)," elements arranged in consecutive", block_len, " block size: ")
# print(listBLOCKs)

rest = []
for i in list:
    if i not in listBLOCKs:
        rest.append(i)
        
# print("The rest of the ", len(rest)," elements are: ")
# print(rest)  
print(*listBLOCKs, sep=" ,", file = open('%dsyncopnbblock.txt' %block_len,'w')) 

with open('europnbblock.txt', 'a') as file:
    for i in rest:
        file.write(f"{i} ")
    file.write(f"{ len(listBLOCKs)} ")
    file.write(f"{len(rest)} ")
    

if len(rest)+len(listBLOCKs)== len(list):
    print("Ok")

# print("The last elements of the block are: {", end ="")

# for i in lastelement:
#     print(f"{i}, ", end ="")
# print("}")

# counter1=0
# print("The first", block_len-1, "elements of the block are: {", end ="")
# for elements in rest_cons_element:
#     for i in elements:
#         print(f"{i}, ", end ="")
#         counter1+=1
# print("}")

# counter = 0
# print("The rest of the elements are: {", end ="")
# for i in range(256):
#     if i not in BLOCK:
#             counter +=1
#             print(f"{i}, ", end ="")
# print("}")

# print(f"Total elements = {len(lastelement)+counter1+counter}")
# print(f"\n# of blocks: {len(BLOCK)}") 
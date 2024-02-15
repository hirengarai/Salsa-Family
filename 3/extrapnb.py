# preferably the larger set
list1 = [3, 6, 15, 16, 31, 35, 67, 68, 71, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 103, 104, 127, 136, 191, 223, 224, 225, 248, 249, 250, 251, 252, 253, 254, 255]
extra = []

list2 = [3, 16, 68, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 104, 136, 191, 224, 225, 248, 249, 250, 251, 252, 253, 254, 255]


for i in list1:
    if i not in list2:
        extra.append(i)

if len(extra)+len(list2) == len(list1):
    print("ok")
with open('extrapnb.txt','w') as file:
    print(*extra, sep=", ", file = file)
    file.write("Total extra element %d" %len(extra))
# print (extra)
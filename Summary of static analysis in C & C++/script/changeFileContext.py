import os

for file in os.listdir('.'):    #os.listdir('.')遍历文件夹内的每个文件名，并返回一个包含文件名的list
    print(file)
    data = []
    if file[-2: ] == 'py':
        continue   #过滤掉改名的.py文件    
    
    with open(file, 'r+') as f:
        for line in f.readlines():
            left, right = line.split(",")
            line = right
            data.append(line)

    with open(file, 'w+') as f:
        for eachline in data:
            f.writelines(eachline)
            #print(eachline)
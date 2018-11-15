import os

for file in os.listdir('.'):    #os.listdir('.')遍历文件夹内的每个文件名，并返回一个包含文件名的list
    if file[-2: ] == 'py':
        continue   #过滤掉改名的.py文件    
    new_name = file.replace("_commits", "")   #选择名字中需要保留的部分
    print(new_name)
    fi, se = new_name.split("_" , 1)
    print(fi + '/' + se)

    curWorkingPath = os.getcwd()
    os.chdir('/mnt/c/wcventure/cppcheck/out')
    os.system('mkdir ' + fi)
    os.chdir(fi)
    os.system('cp /mnt/c/wcventure/cppcheck/tag_commits/'+file + " " + se)
    
    folderName=se.replace(".txt",'')
    os.chdir('/mnt/c/wcventure/cppcheck/cppcheck_output')
    os.system('mkdir ' + folderName)
    
    os.chdir(curWorkingPath)
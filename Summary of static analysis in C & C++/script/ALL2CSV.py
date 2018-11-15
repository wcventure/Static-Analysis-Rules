import os

for file in os.walk('.'):    #os.listdir('.')遍历文件夹内的每个文件名，并返回一个包含文件名的list
    for eachXML in file[2]:
        if eachXML[-3: ] == '.py' or eachXML[-4: ] == '.csv':
            continue   #过滤掉改名的.py文件
        XMLFILE = os.path.abspath(file[0] + '/' + eachXML)
        ProjectNameIndex = file[0].rfind('/')
        ProjectName = file[0][ProjectNameIndex+1:]
        # print(ProjectName,XMLFILE)
        os.system('python3 CppcheckXML2CSV.py -i ' + XMLFILE + ' -p ' + ProjectName)
        
        

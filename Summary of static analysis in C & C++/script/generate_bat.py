import os

#获取某文件夹下各个文件名的列表
path = 'C:/Users/admin/Documents/study/Bitbucket Git/staticanalysis/Summary of static analysis in C & C++/overflow_vuln_func/function_data/'
outputpath = 'C:/Users/admin/Documents/study/Bitbucket Git/staticanalysis/Summary of static analysis in C & C++/overflow_vuln_func/result_from_cppcheckonlyerror/'

with open('cppcheck_xmlonlyerror.bat', 'w') as f:
    for i in os.walk(path):
        for each in i[2]:
            f.write('cppcheck ' + '"' + path + each + '"' + ' 2>' + '"' + outputpath + each[0:-2] + '.txt' + '"\n')
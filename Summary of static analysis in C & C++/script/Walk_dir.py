import os

dirlist = []
newset = set()
CommandList = []

print('step1:')
x = 0
for dirpath,dirname,filename in os.walk(r'./git-project'):
    x = x + 1
    print('x =', str(x))
    for i in filename:
        dirlist.append(os.path.join(dirpath,i))
print('step2:')
y = 0
for each in dirlist:
    y = y + 1
    print('y =', str(y))
    tmpList = each.split('/',4)
    newset.add('/home/jingkai/Documents' + tmpList[0][1:] + '/' + tmpList[1] + '/' + tmpList[2] + '/' + tmpList[3])
print('step3:')
z = 0
for each in newset:
    z = z + 1
    print('z =', str(z))
    if not each[-4:] == '.txt':
        CommandList.append('python3 cppcheck_auto.py -p ' + each + ' -c ' + each + '.txt' + ' -o ./cppcheck_output/' + os.path.split(each)[1])
        print('python3 cppcheck_auto.py -p ' + each + ' -c ' + each + '.txt' + ' -o ./cppcheck_output/' + os.path.split(each)[1])
print('step4:')
if not CommandList == []:
    f = open('./Clog.txt', 'w')

    for each in CommandList:
        f.write(each + '\n')

    f.close()
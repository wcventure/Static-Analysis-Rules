#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import getopt
import os
import time
import datetime

def main(argv):
    #通过 getopt模块 来识别参数demo

    projectPath = ""
    commitFile = ""
    outputPath = ""

    try:
        """
            options, args = getopt.getopt(args, shortopts, longopts=[])
            参数args：一般是sys.argv[1:]。过滤掉sys.argv[0]，它是执行脚本的名字，不算做命令行参数。
            参数shortopts：短格式分析串。例如："hp:i:"，h后面没有冒号，表示后面不带参数；p和i后面带有冒号，表示后面带参数。
            参数longopts：长格式分析串列表。例如：["help", "ip=", "port="]，help后面没有等号，表示后面不带参数；ip和port后面带冒号，表示后面带参数。
            返回值options是以元组为元素的列表，每个元组的形式为：(选项串, 附加参数)，如：('-i', '192.168.0.1')
            返回值args是个列表，其中的元素是那些不含'-'或'--'的参数。
        """
        opts, args = getopt.getopt(argv, "hp:c:o:", ["help", "project_path=", "commit_file=", "output_path="])

    except getopt.GetoptError:
        print('Error: cppcheck_auto.py -p <project_path> -c <commit_file> -o <output_path>')
        print('   or: cppcheck_auto.py --project=<project_path> --commit=<commit_file> --output=<output_path>')
        sys.exit(2)

    # 处理 返回值options是以元组为元素的列表。
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print('cppcheck_auto.py -n <project_path> -c <commit_file> -o <output_path>')
            print('or: cppcheck_auto.py --project=<project_path> --commit=<commit_file> --output=<output_path>')
            sys.exit()
        elif opt in ("-p", "--project"):
            projectPath = arg
        elif opt in ("-c", "--commit"):
            commitFile = arg
        elif opt in ("-o", "--output"):
            outputPath = arg

    if not projectPath[-1] == '/':
        projectPath = projectPath + '/'
    if not outputPath[-1] == '/':
        outputPath = outputPath + '/'

    print('Project path = ', projectPath)
    print('Commit file =', commitFile)
    print('Output path =', outputPath)
    print('')

    # 打印 返回值args列表，即其中的元素是那些不含'-'或'--'的参数。
    for i in range(0, len(args)):
        print('参数 %s 为：%s' % (i + 1, args[i]))

    if projectPath == "":
        print('Error: project Path is empty')
        sys.exit(2)
    if commitFile == "":
        print('Error: commit File is empty')
        sys.exit(2)
    if outputPath == "":
        print('Error: output Pah is empty')
        sys.exit(2)

    # start
    commitList = GetCommitList(commitFile)
    
    if not commitList == []:
        commitNum = 0
        for commitID in commitList:
            print('---------------------------------------------------------------')
            print("Running cppcheck for", commitNum,"/", len(commitList), 'in', projectPath)
            print('---------------------------------------------------------------')
            curCommitID = GitCheckOut(projectPath, commitID)
            RunCppcheck(projectPath, curCommitID, outputPath)
            commitNum = commitNum + 1
            print('')

    print("\n\n@@@@@ End task for", commitNum,"/", len(commitList), 'in', projectPath, ' @@@@@')
    # end

def GetCommitList(commitFile):
    commitList = []
    file = open(commitFile)

    commitList = file.readlines()
    if not commitList == []:
        for i in range(0, len(commitList)):
            commitList[i] = commitList[i].strip('\n')
    return commitList


def GitCheckOut(projectPath, commitID):
    curWorkingPath = os.getcwd()
    os.chdir(projectPath)
    os.system("git checkout " + commitID + " -f" )
    curCommitID = os.popen("git rev-parse HEAD")
    
    os.chdir(curWorkingPath)
    return(curCommitID.read().strip('\n'))

def RunCppcheck(projectPath, commitID, outputPath):
    os.system("cppcheck --enable=all --xml-version=2 " + projectPath + " 2> " + outputPath + commitID + '.xml')
    #print("cppcheck --enable=all --xml-version=2 " + projectPath + " 2> " + outputPath + commitID + '.xml')
    

if __name__ == "__main__":
    # sys.argv[1:]为要处理的参数列表，sys.argv[0]为脚本名，所以用sys.argv[1:]过滤掉脚本名。
    starttime = datetime.datetime.now()
    print ("\nstart time: ", starttime)

    main(sys.argv[1:])
    
    endtime = datetime.datetime.now()
    print ("start time: ", starttime)
    print ("end time: ", endtime)
    
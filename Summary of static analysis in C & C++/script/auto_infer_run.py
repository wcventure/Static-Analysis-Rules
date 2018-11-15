# workflow

'''
git checkout -f
./autogen.sh
./configure
infer run -- make
mv infer-out infer_output
mv infer-out/infer_output ${commitID}
make clean
'''


#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import getopt
import os
import time
import datetime

COMMIT_FILE = 'commits.txt'


def GetCommitList(commitFile):
    commitList = []
    file = open(commitFile)

    commitList = file.readlines()
    if not commitList == []:
        for i in range(0, len(commitList)):
            commitList[i] = commitList[i].strip('\n')
    return commitList

def GitCheckOut(commitID):
    os.system("git checkout " + commitID + " -f" )
    curCommitID = os.popen("git rev-parse HEAD")
    return(curCommitID.read().strip('\n'))


def RunInfer(commitID):
    
    os.system("./autogen.sh")
    os.system("./configure")
    os.system("infer run -- make --keep-going")
    os.system("mv infer-out/ infer_output/")
    os.system("mv infer_output/infer-out/ infer_output/" + commitID)
    os.system("make clean")

# main

commitList = GetCommitList(COMMIT_FILE)


os.system("mkdir infer_output")

if not commitList == []:
    commitNum = 0
    for commitID in commitList:
        print('-------------------------------------------------')
        print("Running Infer for", commitNum,"/", len(commitList))
        print('-------------------------------------------------')
        curCommitID = GitCheckOut(commitID)
        RunInfer(curCommitID)
        commitNum = commitNum + 1
        print('')

    print("\n\n@@@@@ End task for", commitNum,"/", len(commitList), 'in', projectPath, ' @@@@@')
    # end


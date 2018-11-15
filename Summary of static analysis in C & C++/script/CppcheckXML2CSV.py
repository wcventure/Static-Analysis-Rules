#!/usr/bin/python
# -*- coding: UTF-8 -*-
# 注意：commitsID 默认为文件的名字
# Project Name 手动输入

import getopt
import time
import datetime
import os
import sys
import glob
import pandas as pd
import xml.etree.ElementTree as ET

def main(argv):
    
    #通过 getopt模块 来识别参数demo
    
    FILE = ''
    ProjectName = ''
    Tools = 'CPPCheck'

    try:

        opts, args = getopt.getopt(argv, "hi:p:", ["help", "CPPCHECK_XML_FILE=", "Project_Name="])

    except getopt.GetoptError:
        print('Error: CppcheckXML2CSV.py -i <CPPCHECK_XML_FILE> -p <Project_Name>')
        print('   or: CppcheckXML2CSV.py --xml=<CPPCHECK_XML_FILE> --project <Project_Name>')
        sys.exit(2)

    # 处理 返回值options是以元组为元素的列表。
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print('CppcheckXML2CSV.py -i <CPPCHECK_XML_FILE> -p <Project_Name>')
            print('or: CppcheckXML2CSV.py --xml=<CPPCHECK_XML_FILE> --project <Project_Name>')
            sys.exit()
        elif opt in ("-i", "--xml"):
            FILE = arg.strip()
        elif opt in ("-p", "--project"):
            ProjectName = arg.strip()
        
    if FILE == "":
        print('Error: XML file path is empty')
        print('Tips: Using -h to view help')
        sys.exit(2)
        
    if ProjectName == "":
        print('Error: Project name is empty')
        print('Tips: Using -h to view help')
        sys.exit(2)
        
    if FILE[-4:] != '.xml':
        sys.exit("Sorry, the file isn't a .xml file. goodbye! Please type -h to see the Guide")
    
    # print('XML_FILE = ', FILE)    


    # 打印 返回值args列表，即其中的元素是那些不含'-'或'--'的参数。
    for i in range(0, len(args)):
        print('参数 %s 为：%s' % (i + 1, args[i]))


    # start
    PathEndIndex = FILE.rfind('/')
    CurFileName = FILE[PathEndIndex+1:].replace('.xml','')
    PATH = FILE[:PathEndIndex]
    PATH = os.path.abspath(PATH)
    FILE = os.path.abspath(FILE)
    os.chdir(PATH)



    xml_list = []
    tree = ET.parse(FILE)
    root = tree.getroot()
    for mem in root.findall('errors'):
        for member in mem.findall('error'):
            file = 'N.A.'
            line = 'N.A.'
            try:
                file = member[0].attrib['file']
                line = member[0].attrib['line']
            except:
                pass
            value = (ProjectName,
                    CurFileName,
                    Tools,
                    member.attrib['severity'],
                    member.attrib['id'],
                    file,
                    line,
                    member.attrib['msg'],
                    member.attrib['verbose']
                    )
            xml_list.append(value)

    column_name = ['project_name', 'commit_id', 'tool', 'severity', 'type', 'location', 'line_number', 'details', 'remarks']
    xml_df = pd.DataFrame(xml_list, columns=column_name)
    xml_df.to_csv(CurFileName + '.csv', index=None)


    print('Successfully converted xml to csv:', CurFileName)

    # end

	
if __name__ == "__main__":
    # sys.argv[1:]为要处理的参数列表，sys.argv[0]为脚本名，所以用sys.argv[1:]过滤掉脚本名。
    starttime = datetime.datetime.now()

    # print ("\nstart time: ", starttime)

    main(sys.argv[1:])
    
    endtime = datetime.datetime.now()
    
    # print ("start time: ", starttime)
    # print ("end time: ", endtime)

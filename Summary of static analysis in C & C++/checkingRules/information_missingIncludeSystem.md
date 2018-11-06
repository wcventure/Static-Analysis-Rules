# <center> missingIncludeSystem

cppcheck找不到included的头文件

### Type

id = "missingIncludeSystem"  
severity = "information"

    <error id="missingIncludeSystem" severity="information" msg="Include file: &lt;&gt; not found. Please note: Cppcheck does not need standard library headers to get proper results." verbose="Include file: &lt;&gt; not found. Please note: Cppcheck does not need standard library headers to get proper results."/>

### Description

Cppcheck cannot find all the include files. Cppcheck can check the code without the include files found. But the results will probably be more accurate if all the include files are found. Please check your project include directories and add all of them as include directories for Cppcheck. To see what files Cppcheck cannot find use --check-config.


### Example cpp file

	include "XX1.h"
	include <XX2.h>

	int main()
	{
		;
	}



### Massage output in cppcheck

	(information) Cppcheck cannot find all the include files (use --check-config for details)



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="missingIncludeSystem" severity="information" msg="Cppcheck cannot find all the include files (use --check-config for details)" verbose="Cppcheck cannot find all the include files. Cppcheck can check the code without the include files found. But the results will probably be more accurate if all the include files are found. Please check your project&apos;s include directories and add all of them as include directories for Cppcheck. To see what files Cppcheck cannot find use --check-config."/>
	    </errors>
	</results>




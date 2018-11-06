# <center> missingIncludeSystem

cppcheck找不到included的头文件

### Type

id = "missingInclude"  
severity = "information"

	<error id="missingInclude" severity="information" msg="Include file: &quot;&quot; not found." verbose="Include file: &quot;&quot; not found."/>

### Description


### Example cpp file


### Massage output in cppcheck

	(information) Cppcheck cannot find all the include files (use --check-config for details)



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="missingInclude" severity="information" msg="Cppcheck cannot find all the include files (use --check-config for details)" verbose="Cppcheck cannot find all the include files. Cppcheck can check the code without the include files found. But the results will probably be more accurate if all the include files are found. Please check your project&apos;s include directories and add all of them as include directories for Cppcheck. To see what files Cppcheck cannot find use --check-config."/>
	    </errors>
	</results>
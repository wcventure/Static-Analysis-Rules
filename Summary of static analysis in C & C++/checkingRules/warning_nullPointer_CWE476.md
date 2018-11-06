# <center> nullPointer - CWE 476

Null指针被使用

### Type

id = "nullPointer"  
severity = "warning"

cwe = "CWE-476: NULL Pointer Dereference"  
cwe-type = "Base"

    <error id="nullPointer" severity="error" msg="Null pointer dereference" verbose="Null pointer dereference" cwe="476"/>



### Description

A NULL pointer dereference occurs when the application dereferences a pointer that it expects to be valid, but is NULL, typically causing a crash or exit.

NULL pointer dereference issues can occur through a number of flaws, including race conditions, and simple programming omissions.

More Detail：https://cwe.mitre.org/data/definitions/476.html  



### Example cpp file

	void manipulate_string(char * string){
		char buf[24];
		strcpy(buf, string);
		//...
	}
	
	void main(){
		char *s = NULL;
		manipulate_string(s);
	}



### Massage output in cppcheck

	[test\a\a.cpp:3]: (warning) Possible null pointer dereference: string



### XML output cppcheck
		
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="nullPointer" severity="warning" msg="Possible null pointer dereference: string" verbose="Possible null pointer dereference: string" cwe="476">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="3" info="Null pointer dereference"/>
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="9" info="Calling function &apos;manipulate_string&apos;, 1st argument &apos;string&apos; value is 0"/>
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="8" info="Assignment &apos;s=NULL&apos;, assigned value is 0"/>
	        </error>
	    </errors>
	</results>







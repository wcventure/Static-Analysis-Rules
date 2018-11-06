# <center> selfAssignment - CWE 398

自己赋值给自己是多余的

### Type

id = "selfAssignment"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="selfAssignment" severity="warning" msg="Redundant assignment of &apos;varname&apos; to itself." verbose="Redundant assignment of &apos;varname&apos; to itself." cwe="398"/>



### Description

自己赋值给自己是多余的



### Example cpp file

	void main(){
		a=a;
	}

### Massage output in cppcheck

	[test.cpp:2]: (warning) Redundant assignment of 'a' to itself.


### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="selfAssignment" severity="warning" msg="Redundant assignment of &apos;a&apos; to itself." verbose="Redundant assignment of &apos;a&apos; to itself." cwe="398">
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>

# <center> memsetFloat - CWE 668

memset的第二个参数为浮点数

### Type

id = "memsetFloat"  
severity = "portability"

cwe = "CWE-668: Exposure of Resource to Wrong Sphere"  
cwe-type = "Class"

    <error id="memsetFloat" severity="portability" msg="The 2nd memset() argument &apos;varname&apos; is a float, its representation is implementation defined." verbose="The 2nd memset() argument &apos;varname&apos; is a float, its representation is implementation defined. memset() is used to set each byte of a block of memory to a specific value and the actual representation of a floating-point value is implementation defined." cwe="688"/>

### Description

The product exposes a resource to the wrong control sphere, providing unintended actors with inappropriate access to the resource.

Resources such as files and directories may be inadvertently exposed through mechanisms such as insecure permissions, or when a program accidentally operates on the wrong object. For example, a program may intend that private files can only be provided to a specific user. This effectively defines a control sphere that is intended to prevent attackers from accessing these private files. If the file permissions are insecure, then parties other than the user will be able to access those files.

A separate control sphere might effectively require that the user can only access the private files, but not any other files on the system. If the program does not ensure that the user is only requesting private files, then the user might be able to access other files on the system.

In either case, the end result is that a resource has been exposed to the wrong party.

More Detail：https://cwe.mitre.org/data/definitions/668.html  



### Example cpp file

	void main()
	{
		float a;
		memset(&a, 1.2, sizeof(a))
	}


### Massage output in cppcheck

	[test.cpp:4]: (portability) The 2nd memset() argument '1.2' is a float, its representation is implementation defined.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memsetFloat" severity="portability" msg="The 2nd memset() argument &apos;1.2&apos; is a float, its representation is implementation defined." verbose="The 2nd memset() argument &apos;1.2&apos; is a float, its representation is implementation defined. memset() is used to set each byte of a block of memory to a specific value and the actual representation of a floating-point value is implementation defined." cwe="688">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

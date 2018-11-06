# <center> shiftTooManyBits - CWE 758

32位系统下移位超过31位后运行结果出错

### Type

id = "shiftTooManyBits"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

	<error id="shiftTooManyBits" severity="error" msg="Shifting 32-bit value by 40 bits is undefined behaviour" verbose="Shifting 32-bit value by 40 bits is undefined behaviour" cwe="758"/>



### Description

Shifting signed 32-bit value by 31 bits is undefined behaviour.

More Detail:https://bbs.csdn.net/topics/390982149

### Example cpp file

32位系统下移位超过31位后运行结果出错

	void main()
	{
		int a = 0;
		a = a>>40;
	}

More Detail:https://bbs.csdn.net/topics/390982149

### Massage output in cppcheck

	[test.cpp:4]: (error) Shifting 32-bit value by 40 bits is undefined behaviour

### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="shiftTooManyBits" severity="error" msg="Shifting 32-bit value by 40 bits is undefined behaviour" verbose="Shifting 32-bit value by 40 bits is undefined behaviour" cwe="758">
	            <location file="test.cpp" line="4" info="Shift"/>
	        </error>
	    </errors>
	</results>
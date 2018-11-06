# <center> shiftTooManyBitsSigned - CWE 758

31位移位操作作用于有符号的32位值会导致未定义的行为。

### Type

id = "shiftTooManyBitsSigned"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

	<error id="shiftTooManyBitsSigned" severity="error" msg="Shifting signed 32-bit value by 31 bits is undefined behaviour" verbose="Shifting signed 32-bit value by 31 bits is undefined behaviour" cwe="758"/>


### Description

Shifting signed 32-bit value by 31 bits is undefined behaviour.

More Detail:https://bbs.csdn.net/topics/390982149

### Example cpp file

31位移位操作作用于有符号的32位值会导致未定义的行为。

	void main()
	{
		int a = 0;
		a = a>>31;
	}

### Massage output in cppcheck

	[test.cpp:4]: (error) Shifting signed 32-bit value by 31 bits is undefined behaviour

### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="shiftTooManyBitsSigned" severity="error" msg="Shifting signed 32-bit value by 31 bits is undefined behaviour" verbose="Shifting signed 32-bit value by 31 bits is undefined behaviour" cwe="758">
	            <location file="test.cpp" line="4" info="Shift"/>
	        </error>
	    </errors>
	</results>

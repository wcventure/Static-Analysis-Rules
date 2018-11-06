# <center> arrayIndexOutOfBounds - CWE 119

数组越界

### Type

id = "arrayIndexOutOfBounds"  
severity = "error"

cwe = "CWE-119: Improper Restriction of Operations within the Bounds of a Memory Buffer"  
cwe-type = "class"

    <error id="arrayIndexOutOfBoundsCond" severity="warning" msg="Array &apos;x[10]&apos; accessed at index 20, which is out of bounds. Otherwise condition &apos;y==20&apos; is redundant." verbose="Array &apos;x[10]&apos; accessed at index 20, which is out of bounds. Otherwise condition &apos;y==20&apos; is redundant." cwe="119"/>



### Description

The software performs operations on a memory buffer, but it can read from or write to a memory location that is outside of the intended boundary of the buffer.

Certain languages allow direct addressing of memory locations and do not automatically ensure that these locations are valid for the memory buffer that is being referenced. This can cause read or write operations to be performed on memory locations that may be associated with other variables, data structures, or internal program data.

As a result, an attacker may be able to execute arbitrary code, alter the intended control flow, read sensitive information, or cause the system to crash.  

More Detail：https://cwe.mitre.org/data/definitions/119.html  



### Example cpp file

    void main()
    {
    	char a[10];
    	a[10] = '0';
    }



### Massage output in cppcheck

	[test\a.c:4]: (error) Array 'a[10]' accessed at index 10, which is out of bounds.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
    	<cppcheck version="1.83"/>
    	<errors>
    	    <error id="arrayIndexOutOfBounds" severity="error" msg="Array &apos;a[10]&apos; accessed at index 10, which is out of bounds." verbose="Array &apos;a[10]&apos; accessed at index 10, which is out of bounds." cwe="119">
    	        <location file0="test\a\a.c" file="test\a\a.c" line="4" info="Array index out of bounds"/>
    	    </error>
    	</errors>
	</results>

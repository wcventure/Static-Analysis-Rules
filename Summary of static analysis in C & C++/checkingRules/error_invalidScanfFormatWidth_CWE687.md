# <center> invalidScanfFormatWidth - CWE 687

scanf()字符串格式化长度不匹配，溢出

### Type

id = "invalidScanfFormatWidth"  
severity = "error"

cwe = "CWE-416: Use After Free"  
cwe-type = "Base"

    <error id="invalidScanfFormatWidth" severity="error" msg="Width 5 given in format string (no. 10) is larger than destination buffer &apos;[0]&apos;, use %-1s to prevent overflowing it." verbose="Width 5 given in format string (no. 10) is larger than destination buffer &apos;[0]&apos;, use %-1s to prevent overflowing it." cwe="687"/>


### Description

The software calls a function, procedure, or routine, but the caller specifies an argument that contains the wrong value, which may lead to resultant weaknesses.

More Detail：https://cwe.mitre.org/data/definitions/687.html  
https://blog.csdn.net/u010154760/article/details/44262029

### Example cpp file

	void main()
	{
		char a[10];
		scanf("%10s",a);
		printf("%s",a);
	}

### Massage output in cppcheck
	
	[test.cpp:6]: (error) Memory leak: str



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="invalidScanfFormatWidth" severity="error" msg="Width 10 given in format string (no. 1) is larger than destination buffer &apos;a[10]&apos;, use %9s to prevent overflowing it." verbose="Width 10 given in format string (no. 1) is larger than destination buffer &apos;a[10]&apos;, use %9s to prevent overflowing it." cwe="687">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

# <center> duplicateExpression - CWE 398

在操作符的两侧发现相同的表达式是可疑的，并且可能指示剪切和粘贴或逻辑错误。请仔细检查此代码，以确定它是否正确。

### Type

id = "uselessAssignmentArg"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="uselessAssignmentArg" severity="style" msg="Assignment of function parameter has no effect outside the function." verbose="Assignment of function parameter has no effect outside the function." cwe="398"/>



### Description

在操作符的两侧发现相同的表达式是可疑的，并且可能指示剪切和粘贴或逻辑错误。请仔细检查此代码，以确定它是否正确。


### Example cpp file

	void main(){
		(a+b)<(a+b);
	}



### Massage output in cppcheck

	[test.cpp:2]: (warning) Redundant assignment of 'a' to itself.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="duplicateExpression" severity="style" msg="Same expression on both sides of &apos;&lt;&apos;." verbose="Finding the same expression on both sides of an operator is suspicious and might indicate a cut and paste or logic error. Please examine this code carefully to determine if it is correct." cwe="398">
	            <location file="test.cpp" line="2"/>
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>

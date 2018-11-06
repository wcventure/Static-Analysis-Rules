# <center> unreachableCode - CWE 561

无法抵达的代码段

### Type

id = "unreachableCode"  
severity = "style"

cwe = "CWE-561: Dead Code"  
cwe-type = "Variant"

    <error id="unreachableCode" severity="style" msg="Statements following return, break, continue, goto or throw will never be executed." verbose="Statements following return, break, continue, goto or throw will never be executed." cwe="561"/>



### Description

The software contains dead code, which can never be executed.

Dead code is source code that can never be executed in a running program. The surrounding code makes it impossible for a section of code to ever be executed.

More Detail：https://cwe.mitre.org/data/definitions/561.html  



### Example cpp file

Statements following "goto L1" will never be executed.

	void main()
	{  
		if (x!=0) {
			goto L1;
			if (y!=0) {
				L1:   
			} 
			else {
			} 
		} 
		else {
		}
	}



### Massage output in cppcheck

	[test.cpp:6]: (style) Statements following return, break, continue, goto or throw will never be executed.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unreachableCode" severity="style" msg="Statements following return, break, continue, goto or throw will never be executed." verbose="Statements following return, break, continue, goto or throw will never be executed." cwe="561">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>
# <center> syntaxError

### Type

id = "syntaxError"  
severity = "error"

cwe = "UK"  
cwe-type = "UK"

    

### Description

Invalid number of character "{" when no macros are defined.


### Example cpp file

	int main()
	{
		ifdef A
	}
		endif



### Massage output in cppcheck

	[test\syntaxError\bad.c:2]: (error) Invalid number of character '{' when no macros are defined.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="syntaxError" severity="error" msg="Invalid number of character &apos;{&apos; when no macros are defined." verbose="Invalid number of character &apos;{&apos; when no macros are defined.">
	            <location file0="test\syntaxError\bad.c" file="test\syntaxError\bad.c" line="2"/>
	        </error>
	    </errors>
	</results>




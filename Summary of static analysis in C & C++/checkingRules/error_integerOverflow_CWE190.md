# <center> integerOverflow - CWE 190

整数溢出

### Type

id = "integerOverflow"  
severity = "error"

cwe = "CWE-190: Integer Overflow or Wraparound"  
cwe-type = "base"

    <error id="integerOverflow" severity="error" msg="Signed integer overflow for expression &apos;&apos;." verbose="Signed integer overflow for expression &apos;&apos;." cwe="190"/>


### Description

The software performs a calculation that can produce an integer overflow or wraparound, when the logic assumes that the resulting value will always be larger than the original value. This can introduce other weaknesses when the calculation is used for resource management or execution control.

An integer overflow or wraparound occurs when an integer value is incremented to a value that is too large to store in the associated representation. When this occurs, the value may wrap to become a very small or negative number. While this may be intended behavior in circumstances that rely on wrapping, it can have security consequences if the wrap is unexpected. This is especially the case if the integer overflow can be triggered using user-supplied inputs. This becomes security-critical when the result is used to control looping, make a security decision, or determine the offset or size in behaviors such as memory allocation, copying, concatenation, etc. 

More Detail：https://cwe.mitre.org/data/definitions/190.html  


### Example cpp file

	void main()
	{
		int a = 7000000000;
		a = a*2;
		cout<<a<<endl;
	}  


### Massage output in cppcheck

	[test.cpp:4]: (error) Signed integer overflow for expression 'a*2'.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="integerOverflow" severity="error" msg="Signed integer overflow for expression &apos;a*2&apos;." verbose="Signed integer overflow for expression &apos;a*2&apos;." cwe="190">
	            <location file="test.cpp" line="4" info="Integer overflow"/>
	            <location file="test.cpp" line="3" info="Assignment &apos;a=7000000000&apos;, assigned value is 7000000000"/>
	        </error>
	    </errors>
	</results>

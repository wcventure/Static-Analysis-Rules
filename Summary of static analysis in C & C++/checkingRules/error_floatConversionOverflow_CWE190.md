# <center> floatConversionOverflow - CWE 190

浮点数转换成整型时溢出

### Type

id = "floatConversionOverflow"  
severity = "error"

cwe = "CWE-190: Integer Overflow or Wraparound"  
cwe-type = "base"

    <error id="floatConversionOverflow" severity="error" msg="Undefined behaviour: float (1e+100) to integer conversion overflow." verbose="Undefined behaviour: float (1e+100) to integer conversion overflow." cwe="190"/>


### Description

The software performs a calculation that can produce an integer overflow or wraparound, when the logic assumes that the resulting value will always be larger than the original value. This can introduce other weaknesses when the calculation is used for resource management or execution control.

An integer overflow or wraparound occurs when an integer value is incremented to a value that is too large to store in the associated representation. When this occurs, the value may wrap to become a very small or negative number. While this may be intended behavior in circumstances that rely on wrapping, it can have security consequences if the wrap is unexpected. This is especially the case if the integer overflow can be triggered using user-supplied inputs. This becomes security-critical when the result is used to control looping, make a security decision, or determine the offset or size in behaviors such as memory allocation, copying, concatenation, etc. 

More Detail：https://cwe.mitre.org/data/definitions/190.html  


### Example cpp file

浮点数转换成整型时溢出

	void main()
	{
		float b = 7000000000.0;
		int a = b*2;
		cout<<a<<endl;
	}  


### Massage output in cppcheck

	[test.cpp:4]: (error) Undefined behaviour: float (1.4e+10) to integer conversion overflow.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="floatConversionOverflow" severity="error" msg="Undefined behaviour: float (1.4e+10) to integer conversion overflow." verbose="Undefined behaviour: float (1.4e+10) to integer conversion overflow." cwe="190">
	            <location file="test.cpp" line="4" info="float to integer conversion"/>
	            <location file="test.cpp" line="3" info="Assignment &apos;b=7000000000.0&apos;, assigned value is 7000000000.0"/>
	        </error>
	    </errors>
	</results>
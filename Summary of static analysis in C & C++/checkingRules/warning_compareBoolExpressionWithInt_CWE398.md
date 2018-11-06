# <center> compareBoolExpressionWithInt - CWE 398

布尔表达式与整形比较	

### Type

id = "compareBoolExpressionWithInt"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="compareBoolExpressionWithInt" severity="warning" msg="Comparison of a boolean expression with an integer other than 0 or 1." verbose="Comparison of a boolean expression with an integer other than 0 or 1." cwe="398"/>



### Description

布尔表达式与整形比较	



### Example cpp file

	void main(){
		bool a=true;
		int i=128;
		if (i>a)
			cout<<c[i];
	}

### Massage output in cppcheck

	[test.cpp:5]: (warning) Comparison of a boolean expression with an integer.
	[test.cpp:5]: (style) Condition 'i>a' is always true



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="compareBoolExpressionWithInt" severity="warning" msg="Comparison of a boolean expression with an integer." verbose="Comparison of a boolean expression with an integer." cwe="398">
	            <location file="test.cpp" line="5"/>
	        </error>
	        <error id="knownConditionTrueFalse" severity="style" msg="Condition &apos;i&gt;a&apos; is always true" verbose="Condition &apos;i&gt;a&apos; is always true" cwe="571">
	            <location file="test.cpp" line="5" info="Condition &apos;i&gt;a&apos; is always true"/>
	            <location file="test.cpp" line="4" info="Assignment &apos;i=128&apos;, assigned value is 128"/>
	        </error>
	    </errors>
	</results>



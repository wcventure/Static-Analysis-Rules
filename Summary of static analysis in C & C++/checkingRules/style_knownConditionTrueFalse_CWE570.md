# <center> knownConditionTrueFalse - CWE 570

bool表达式的值一直都是false

### Type

id = "knownConditionTrueFalse"  
severity = "style"

cwe = "CWE-570: Expression is Always False"  
cwe-type = " Variant"

    <error id="knownConditionTrueFalse" severity="style" msg="Condition &apos;x&apos; is always false" verbose="Condition &apos;x&apos; is always false" cwe="570"/>



### Description

The software contains an expression that will always evaluate to false.

More Detail：https://cwe.mitre.org/data/definitions/570.html  



### Example cpp file

	void updateInventory(){
		bool isProductAvailable = false; 
		bool isDelayed = false; 
	
		if(isProductAvailable){
			;
		} 
		else if(isDelayed){
			;
		}
	}



### Massage output in cppcheck

	[test.cpp:5]: (style) Condition 'isProductAvailable' is always false
	[test.cpp:8]: (style) Condition 'isDelayed' is always false



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="knownConditionTrueFalse" severity="style" msg="Condition &apos;isProductAvailable&apos; is always false" verbose="Condition &apos;isProductAvailable&apos; is always false" cwe="570">
	            <location file="test.cpp" line="6" info="Condition &apos;isProductAvailable&apos; is always false"/>
	            <location file="test.cpp" line="2" info="Assignment &apos;isProductAvailable=false&apos;, assigned value is 0"/>
	        </error>
	        <error id="knownConditionTrueFalse" severity="style" msg="Condition &apos;isDelayed&apos; is always false" verbose="Condition &apos;isDelayed&apos; is always false" cwe="570">
	            <location file="test.cpp" line="9" info="Condition &apos;isDelayed&apos; is always false"/>
	            <location file="test.cpp" line="3" info="Assignment &apos;isDelayed=false&apos;, assigned value is 0"/>
	        </error>
	    </errors>
	</results>
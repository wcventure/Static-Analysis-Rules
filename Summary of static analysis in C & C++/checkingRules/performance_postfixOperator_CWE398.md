# <center> postfixOperator - CWE 398

对于非基本类型来说，使用前缀++ / --更好，后缀是低效率的

### Type

id = "postfixOperator"  
severity = "performance"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="postfixOperator" severity="performance" msg="Prefer prefix ++/-- operators for non-primitive types." verbose="Prefix ++/-- operators should be preferred for non-primitive types. Pre-increment/decrement can be more efficient than post-increment/decrement. Post-increment/decrement usually involves keeping a copy of the previous value around and adds a little extra code." cwe="398"/>



### Description

Prefix ++/-- operators should be preferred for non-primitive types. Pre-increment/decrement can be more efficient than post-increment/decrement. Post-increment/decrement usually involves keeping a copy of the previous value around and adds a little extra code.


### Example cpp file

	void main(){  
	for(vector<int>::iterator iter=vector_database.begin(); vector_database!=veci.end(); iter++)
		if( *iter == 10)
			vector_database.erase(iter);

}


### Massage output in cppcheck

	[test.cpp:2]: (performance) Prefer prefix ++/-- operators for non-primitive types.




### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="postfixOperator" severity="performance" msg="Prefer prefix ++/-- operators for non-primitive types." verbose="Prefix ++/-- operators should be preferred for non-primitive types. Pre-increment/decrement can be more efficient than post-increment/decrement. Post-increment/decrement usually involves keeping a copy of the previous value around and adds a little extra code." cwe="398">
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>


# <center> zerodiv - CWE 369

除数为0

### Type

id = "zerodiv"  
severity = "error"

cwe = "CWE-369: Divide By Zero"  
cwe-type = "Base"

    <error id="zerodiv" severity="error" msg="Division by zero." verbose="Division by zero." cwe="369"/>



### Description

The product divides a value by zero.

This weakness typically occurs when an unexpected value is provided to the product, or if an error occurs that is not properly detected. It frequently occurs in calculations involving physical dimensions such as size, length, width, and height. 

More Detail：https://cwe.mitre.org/data/definitions/369.html  



### Example cpp file

	void main(){
		int a = 1;
		a = a/0;
	}
	
	\\除数为0

Bad version

	double divide(double x, double y){
		return x/y;
	}

Good version
	
	const int DivideByZero = 10;
	double divide(double x, double y){
		if ( 0 == y ){
			throw DivideByZero;
		}
		return x/y;
	}
	//...
	try{
		divide(10, 0);
	}
	catch( int i ){
		if(i==DivideByZero) {
		cerr<<"Divide by zero error";
		}
	}

### Massage output in cppcheck

	[test.cpp:3]: (error) Division by zero.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="zerodiv" severity="error" msg="Division by zero." verbose="Division by zero." cwe="369">
	            <location file="test.cpp" line="3" info="Division by zero"/>
	        </error>
	    </errors>
	</results>
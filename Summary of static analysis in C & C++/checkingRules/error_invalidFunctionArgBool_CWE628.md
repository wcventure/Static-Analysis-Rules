# <center> invalidFunctionArgBool - CWE 628

函数接收的参数类型不对

### Type

id = "invalidFunctionArgBool"  
severity = "error"

cwe = "CWE-628: Function Call with Incorrectly Specified Arguments"  
cwe-type = "Base"

     <error id="invalidFunctionArgBool" severity="error" msg="Invalid func_name() argument nr 1. A non-boolean value is required." verbose="Invalid func_name() argument nr 1. A non-boolean value is required." cwe="628"/>



### Description

The product calls a function, procedure, or routine with arguments that are not correctly specified, leading to always-incorrect behavior and resultant weaknesses.

The product calls a function, procedure, or routine with arguments that are not correctly specified, leading to always-incorrect behavior and resultant weaknesses.

More Detail：https://cwe.mitre.org/data/definitions/628.html  



### Example cpp file

Invalid f() argument nr 1. A non-boolean value is required.

	int f(int c){
		return c;
	}

	void main(){
		f(true);
	}

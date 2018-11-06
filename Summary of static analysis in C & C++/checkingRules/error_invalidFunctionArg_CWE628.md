# <center> invalidFunctionArg - CWE 628

函数接收的参数类型不对

### Type

id = "invalidFunctionArg"  
severity = "error"

cwe = "CWE-628: Function Call with Incorrectly Specified Arguments"  
cwe-type = "Base"

     <error id="invalidFunctionArg" severity="error" msg="Invalid func_name() argument nr 1. The value is 0 or 1 (boolean) but the valid values are &apos;1:4&apos;." verbose="Invalid func_name() argument nr 1. The value is 0 or 1 (boolean) but the valid values are &apos;1:4&apos;." cwe="628"/>



### Description

The product calls a function, procedure, or routine with arguments that are not correctly specified, leading to always-incorrect behavior and resultant weaknesses.

The product calls a function, procedure, or routine with arguments that are not correctly specified, leading to always-incorrect behavior and resultant weaknesses.

More Detail：https://cwe.mitre.org/data/definitions/628.html  



### Example cpp file

Invalid f() argument nr 1. The value is 0 or 1 (boolean) but the valid values are int.

	int f(int c){
		return c;
	}

	void main(){
		f(false);
	}

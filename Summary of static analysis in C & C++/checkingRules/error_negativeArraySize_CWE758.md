# <center> negativeArraySize - CWE 758

创建一个长度为负数的数组

### Type

id = "negativeArraySize"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

     <error id="negativeArraySize" severity="error" msg="Declaration of array &apos;&apos; with negative size is undefined behaviour" verbose="Declaration of array &apos;&apos; with negative size is undefined behaviour" cwe="758"/>



### Description

The software uses an API function, data structure, or other entity in a way that relies on properties that are not always guaranteed to hold for that entity.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://cwe.mitre.org/data/definitions/758.html  



### Example cpp file

create an array of negative length

	void main (){

		int n = -35;
	
		int testArray[n];
	
		for(int i = 0; i < 10; i++)
		    testArray[i]=i+1;
	}

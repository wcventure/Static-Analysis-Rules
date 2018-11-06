# <center> pointerArithBool - CWE 571

将指针运算结果转换为布尔运算。

### Type

id = "pointerArithBool"  
severity = "error"

cwe = "CWE-571: Expression is Always True"  
cwe-type = "class"

    <error id="pointerArithBool" severity="error" msg="Converting pointer arithmetic result to bool. The bool is always true unless there is undefined behaviour." verbose="Converting pointer arithmetic result to bool. The boolean result is always true unless there is pointer arithmetic overflow, and overflow is undefined behaviour. Probably a dereference is forgotten." cwe="571"/>


### Description

Converting pointer arithmetic result to bool. The boolean result is always true unless there is pointer arithmetic overflow, and overflow is undefined behaviour. Probably a dereference is forgotten.

More Detail：https://cwe.mitre.org/data/definitions/119.html  

https://stackoverflow.com/questions/3002338/determining-true-false

### Example cpp file

	int main(){
	    char greeting[50] = "goodmorning everyone";
	    char *s1 = greeting;
	    char *s2 = &greeting[7];
	
	    bool test = s2-s1;
	
	    cout << "s1 is: " << s1 << endl;
	    cout << "s2 is: " << s2 << endl;
	    if (test == true ){
	        cout << "test is true and is: " << test << endl;
	    }
	
	    if (test == false){
	        cout<< "test is false and is: " << test << endl;
	    }
	
	    return 0;
	}

outputs:

	s1 is: goodmorning everyone
	s2 is: ning everyone
	test is true and is: 1


If the result of the subtraction is zero, test will be false; otherwise it will be true.

Any value that is of a numeric type, enumeration type, or is a pointer can be converted to a boolean; if the value is zero (or null, for pointers), the result is false; otherwise the result is true.

Converting pointer arithmetic result to bool. The boolean result is always true unless there is pointer arithmetic overflow, and overflow is undefined behaviour. Probably a dereference is forgotten.
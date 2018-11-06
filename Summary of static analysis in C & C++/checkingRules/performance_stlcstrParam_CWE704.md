# <center> stlcstrParam - CWE 704

c_str()的结果传递给一个函数,采用string作为参数。是缓慢和冗余。

### Type

id = "stlcstrParam"  
severity = "performance"

cwe = "CWE-704: Incorrect Type Conversion or Cast"  
cwe-type = "Class"

    <error id="stlcstrParam" severity="performance" msg="Passing the result of c_str() to a function that takes std::string as argument no. 0 is slow and redundant." verbose="The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly passing the string." cwe="704"/>



### Description

The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly passing the string.


### Example cpp file

c_str()的结果传递给一个函数,采用string作为参数。是缓慢和冗余。

	char* func(char* s)
	{
	    return(s.c_str()); 
	}
	
	void main(){
		string s = "1234";  
		func(s.c_str());
	}
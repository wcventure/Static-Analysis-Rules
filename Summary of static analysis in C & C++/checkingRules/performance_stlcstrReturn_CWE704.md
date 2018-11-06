# <center> stlcstrReturn - CWE 704

返回的结果c_str()函数,它返回string是缓慢和冗余。

### Type

id = "stlcstrReturn"  
severity = "performance"

cwe = "CWE-704: Incorrect Type Conversion or Cast"  
cwe-type = "Class"

    <error id="stlcstrReturn" severity="performance" msg="Returning the result of c_str() in a function that returns std::string is slow and redundant." verbose="The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly returning the string." cwe="704"/>



### Description

The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly returning the string.


### Example cpp file

返回的结果c_str()函数,它返回string是缓慢和冗余。

	char* func(char* s)
	{
	    return(s.c_str()); 
	}
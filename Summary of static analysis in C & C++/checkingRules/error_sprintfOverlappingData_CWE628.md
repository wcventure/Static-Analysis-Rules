# <center> sprintfOverlappingData - CWE 628

重叠数据,snprintf中来源和目的地是一样的

### Type

id = "sprintfOverlappingData"  
severity = "error"

cwe = "CWE-628: Function Call with Incorrectly Specified Arguments"  
cwe-type = "Base"

    <error id="sprintfOverlappingData" severity="error" msg="Undefined behavior: Variable &apos;varname&apos; is used as parameter and destination in s[n]printf()." verbose="The variable &apos;varname&apos; is used both as a parameter and as destination in s[n]printf(). The origin and destination buffers overlap. Quote from glibc (C-library) documentation (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Formatted-Output-Functions): &quot;If copying takes place between objects that overlap as a result of a call to sprintf() or snprintf(), the results are undefined.&quot;" cwe="628"/>


### Description

Undefined behavior: Variable is used as parameter and destination in snprintf().

The product calls a function, procedure, or routine with arguments that are not correctly specified, leading to always-incorrect behavior and resultant weaknesses.
+ Extended Description
There are multiple ways in which this weakness can be introduced, including:  
- the wrong variable or reference;  
- an incorrect number of arguments;  
- incorrect order of arguments;  
- wrong type of arguments; or  
- wrong value.  

More Detail：https://cwe.mitre.org/data/definitions/628.html
[snprintf()函数探讨](https://blog.csdn.net/czxyhll/article/details/7950247)  
[snprintf()函数使用方法](https://blog.csdn.net/sky1203850702/article/details/52778831)


### Example cpp file

	int main()
	{
		char a='a';
		char str[20];
		snprintf(a,2,"%02x",a);
		return 0;
	}


### Massage output in cppcheck

	test.cpp:5]: (error) Undefined behavior: Variable 'a' is used as parameter and destination in s[n]printf().


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="sprintfOverlappingData" severity="error" msg="Undefined behavior: Variable &apos;a&apos; is used as parameter and destination in s[n]printf()." verbose="The variable &apos;a&apos; is used both as a parameter and as destination in s[n]printf(). The origin and destination buffers overlap. Quote from glibc (C-library) documentation (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Formatted-Output-Functions): &quot;If copying takes place between objects that overlap as a result of a call to sprintf() or snprintf(), the results are undefined.&quot;" cwe="628">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>
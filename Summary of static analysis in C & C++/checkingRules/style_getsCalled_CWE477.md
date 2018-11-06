# <center> getsCalled - CWE 477

过时的函数被调用。推荐使用fgets()函数	

### Type

id = "getsCalled"  
severity = "style"

cwe = "CWE-477: Use of Obsolete Function"  
cwe-type = "Class"

    <error id="getsCalled" severity="style" msg="Obsolete function &apos;gets&apos; called. It is recommended to use &apos;fgets&apos; or &apos;gets_s&apos; instead." verbose="The obsolete function &apos;gets&apos; is called. With &apos;gets&apos; you&apos;ll get a buffer overrun if the input data exceeds the size of the buffer. It is recommended to use the functions &apos;fgets&apos; or &apos;gets_s&apos; instead." cwe="477">

### Description

The code uses deprecated or obsolete functions, which suggests that the code has not been actively reviewed or maintained.

As programming languages evolve, functions occasionally become obsolete due to:  
- Advances in the language
- Improved understanding of how operations should be performed effectively and securely
- Changes in the conventions that govern certain operations

Functions that are removed are usually replaced by newer counterparts that perform the same task in some different and hopefully improved way.

More Detail：https://cwe.mitre.org/data/definitions/704.html  


### Example cpp file

gets()函数存在一个严重的缺陷，这个缺陷就是：它不会检查数组是否能够装得下输入行。
过时的函数被调用。推荐使用fgets()函数	

	int main()
	{
	    char words[STLEN];
	
	    puts("Enter a string, please.");
	    gets(words);  // typical use
	    printf("Your string twice:\n");
	    printf("%s\n", words);
	    puts(words);
	    puts("Done.");
	
	    return 0;
	}

### Massage output in cppcheck

	[test.cpp:6]: (warning) Obsolete function 'gets' called. It is recommended to use 'fgets' or 'gets_s' instead.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="getsCalled" severity="warning" msg="Obsolete function &apos;gets&apos; called. It is recommended to use &apos;fgets&apos; or &apos;gets_s&apos; instead." verbose="The obsolete function &apos;gets&apos; is called. With &apos;gets&apos; you&apos;ll get a buffer overrun if the input data exceeds the size of the buffer. It is recommended to use the functions &apos;fgets&apos; or &apos;gets_s&apos; instead." cwe="477">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>

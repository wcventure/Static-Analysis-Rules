# <center> nullPointerRedundantCheck - CWE 476

使用指针之前检查指针是否为NULL

### Type

id = "nullPointerRedundantCheck"  
severity = "warning"

cwe = "CWE-476: NULL Pointer Dereference"  
cwe-type = "Base"

    <error id="nullPointerRedundantCheck" severity="warning" msg="Either the condition is redundant or there is possible null pointer dereference: pointer." verbose="Either the condition is redundant or there is possible null pointer dereference: pointer." cwe="476"/>



### Description

A NULL pointer dereference occurs when the application dereferences a pointer that it expects to be valid, but is NULL, typically causing a crash or exit.

NULL pointer dereference issues can occur through a number of flaws, including race conditions, and simple programming omissions.

More Detail：https://cwe.mitre.org/data/definitions/476.html  



### Example cpp file

bad：

	int main()
	{
	   	FILE *fp = fopen("good.c", "r");
	
		if (fp != NULL)
		{
			;//...
			
		}
	
		fclose(fp);
		fp=NULL;
	   	return 0;
	}

right：

	int main()
	{
		FILE *fp = fopen("good.c", "r");
		
		if (NULL == fp)
		{
	        	return -1;
		}
		
		//...
		
		fclose(fp);
		fp=NULL;
		return 0;
	}

### Massage output in cppcheck

	[test\a\a.cpp:5] -> [test\a\a.cpp:11]: (warning) Either the condition 'fp!=NULL' is redundant or there is possible null pointer dereference: fp.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="nullPointerRedundantCheck" severity="warning" msg="Either the condition &apos;fp!=NULL&apos; is redundant or there is possible null pointer dereference: fp." verbose="Either the condition &apos;fp!=NULL&apos; is redundant or there is possible null pointer dereference: fp." cwe="476">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="11" info="Null pointer dereference"/>
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="5" info="Assuming that condition &apos;fp!=NULL&apos; is not redundant"/>
	        </error>
	    </errors>
	</results>










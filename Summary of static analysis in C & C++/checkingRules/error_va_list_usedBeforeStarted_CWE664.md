# <center> va_list_usedBeforeStarted - CWE 664

va_list在va_start()调用之前就被使用了

### Type

id = "va_list_usedBeforeStarted"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

	<error id="va_list_usedBeforeStarted" severity="error" msg="va_list &apos;vl&apos; used before va_start() was called." verbose="va_list &apos;vl&apos; used before va_start() was called." cwe="664"/>


### Description

va_list 'ap' used before va_start() was called.

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

	void main(){
		va_list ap;//初始化指向可变参数列表的指针
		va_list *bp=&ap;

		va_start(*bp,fmt);//将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始
	}


### Massage output in cppcheck

	[test.cpp:3]: (error) va_list 'ap' used before va_start() was called.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="va_list_usedBeforeStarted" severity="error" msg="va_list &apos;ap&apos; used before va_start() was called." verbose="va_list &apos;ap&apos; used before va_start() was called." cwe="664">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>




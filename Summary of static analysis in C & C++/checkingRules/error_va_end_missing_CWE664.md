# <center> va_end_missing - CWE 664

va_list没有使用到va_end()

### Type

id = "va_end_missing"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

	<error id="va_end_missing" severity="error" msg="va_list &apos;vl&apos; was opened but not closed by va_end()." verbose="va_list &apos;vl&apos; was opened but not closed by va_end()." cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

va_list没有使用到va_end()

	void main(){
		va_list ap;//初始化指向可变参数列表的指针
		va_start(ap,fmt);//将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始
	
	}



### Massage output in cppcheck

	[test.cpp:5]: (error) va_list 'ap' was opened but not closed by va_end().



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="va_end_missing" severity="error" msg="va_list &apos;ap&apos; was opened but not closed by va_end()." verbose="va_list &apos;ap&apos; was opened but not closed by va_end()." cwe="664">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>
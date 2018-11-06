# <center> va_start_subsequentCalls - CWE 664

va_start()和va_copy()之间没有调用va_end()

### Type

id = "va_start_subsequentCalls"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

	<error id="va_start_subsequentCalls" severity="error" msg="va_start() or va_copy() called subsequently on &apos;vl&apos; without va_end() in between." verbose="va_start() or va_copy() called subsequently on &apos;vl&apos; without va_end() in between." cwe="664"/>


### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

va_start()和va_copy()之间没有调用va_end()

	void main(){
		va_list ap,bp;//初始化指向可变参数列表的指针
		va_start(ap,fmt);
		va_start(bp,fmt);
		va_copy(ap,bp);
		va_end(ap);
		va_end(bp);
	}



### Massage output in cppcheck

	[test.cpp:5]: (error) va_start() or va_copy() called subsequently on 'ap' without va_end() in between.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="va_start_subsequentCalls" severity="error" msg="va_start() or va_copy() called subsequently on &apos;ap&apos; without va_end() in between." verbose="va_start() or va_copy() called subsequently on &apos;ap&apos; without va_end() in between." cwe="664">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>




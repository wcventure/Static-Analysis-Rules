# <center> eraseDereference - CWE 664

元素被擦除后还继续被使用

### Type

id = "eraseDereference"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="eraseDereference" severity="error" msg="Invalid iterator &apos;iter&apos; used." verbose="The iterator &apos;iter&apos; is invalid before being assigned. Dereferencing or comparing it with another iterator is invalid operation." cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

	include <vector>
	
	int main()
	{
	    std::vector<int> items;
	    items.push_back(1);
	    items.push_back(2);
	    items.push_back(3);
	    std::vector<int>::iterator iter;
	    for (iter = items.begin(); iter != items.end(); ++iter) {
	        if (*iter == 2) {
	            items.erase(iter);
	        }
	    }
	}



### Massage output in cppcheck

	[test\erase\bad.cpp:9] -> [test\erase\bad.cpp:11]: (error) Iterator 'iter' used after element has been erased.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="eraseDereference" severity="error" msg="Iterator &apos;iter&apos; used after element has been erased." verbose="The iterator &apos;iter&apos; is invalid after the element it pointed to has been erased. Dereferencing or comparing it with another iterator is invalid operation." cwe="664">
	            <location file0="test\erase\bad.cpp" file="test\erase\bad.cpp" line="11"/>
	            <location file0="test\erase\bad.cpp" file="test\erase\bad.cpp" line="9"/>
	        </error>
	    </errors>
	</results>




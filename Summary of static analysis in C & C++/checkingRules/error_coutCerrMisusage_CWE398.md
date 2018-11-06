# <center> coutCerrMisusage - CWE 398

输出流(std::cout)的不正确使用

### Type

id = "coutCerrMisusage"  
severity = "error"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="coutCerrMisusage" severity="error" msg="No &apos;return&apos; statement in non-void function causes undefined behavior." verbose="No &apos;return&apos; statement in non-void function causes undefined behavior." cwe="398"/>



### Description

invalid usage of output stream. For example: std::cout << std::cout;'


### Example cpp file

	void main(){
		std::cout << std::cout;
	}

### Massage output in cppcheck

	[test.cpp:2]: (error) Invalid usage of output stream: '<< std::cout'.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="coutCerrMisusage" severity="error" msg="Invalid usage of output stream: &apos;&lt;&lt; std::cout&apos;." verbose="Invalid usage of output stream: &apos;&lt;&lt; std::cout&apos;." cwe="398">
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>
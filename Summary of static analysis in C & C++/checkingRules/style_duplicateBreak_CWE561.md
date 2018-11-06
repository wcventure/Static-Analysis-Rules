# <center> duplicateBreak - CWE 561

连续的 return, break, continue, goto or throw statements 是没有必要的.第二个语句无法执行,因此应该被删除。

### Type

id = "duplicateBreak"  
severity = "style"

cwe = "CWE-561: Dead Code"  
cwe-type = "Variant"

    <error id="duplicateBreak" severity="style" msg="Consecutive return, break, continue, goto or throw statements are unnecessary." verbose="Consecutive return, break, continue, goto or throw statements are unnecessary. The second statement can never be executed, and so should be removed." cwe="561"/>


### Description

Consecutive return, break, continue, goto or throw statements are unnecessary. The second statement can never be executed, and so should be removed.

More Detail：https://cwe.mitre.org/data/definitions/561.html  



### Example cpp file

	void main(){
	
	  return;
	  return;
	}



### Massage output in cppcheck

	[test.cpp:6]: (style) Statements following return, break, continue, goto or throw will never be executed.



### XML output cppcheck

<?xml version="1.0" encoding="UTF-8"?>
<results version="2">
    <cppcheck version="1.83"/>
    <errors>
        <error id="duplicateBreak" severity="style" msg="Consecutive return, break, continue, goto or throw statements are unnecessary." verbose="Consecutive return, break, continue, goto or throw statements are unnecessary. The second statement can never be executed, and so should be removed." cwe="561">
            <location file="test.cpp" line="4"/>
        </error>
    </errors>
</results>

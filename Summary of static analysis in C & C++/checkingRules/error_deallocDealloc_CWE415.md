# <center> deallocDealloc - CWE 415

回收一个分配指针

### Type

id = "deallocDealloc"  
severity = "error"

cwe = "CWE-415: Double Free"  
cwe-type = "Variant"

    <error id="deallocDealloc" severity="error" msg="Deallocating a deallocated pointer: varname" verbose="Deallocating a deallocated pointer: varname" cwe="415"/>


### Description

The product calls free() twice on the same memory address, potentially leading to modification of unexpected memory locations.

When a program calls free() twice with the same argument, the program's memory management data structures become corrupted. This corruption can cause the program to crash or, in some circumstances, cause two later calls to malloc() to return the same pointer. If malloc() returns the same value twice and the program later gives the attacker control over the data that is written into this doubly-allocated memory, the program becomes vulnerable to a buffer overflow attack.

More Detail：https://cwe.mitre.org/data/definitions/415.html  



### Example cpp file

free(p)使用了两次

	int main(){
		int * p = (int *)malloc(sizeof(int));
		free(p);
		free(p);
		return 0;
	}

### Massage output in cppcheck
	
	[test.cpp:4]: (error) Memory pointed to by 'p' is freed twice.
	[test.cpp:4]: (error) Deallocating a deallocated pointer: p



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="doubleFree" severity="error" msg="Memory pointed to by &apos;p&apos; is freed twice." verbose="Memory pointed to by &apos;p&apos; is freed twice." cwe="415">
	            <location file="test.cpp" line="4"/>
	        </error>
	        <error id="deallocDealloc" severity="error" msg="Deallocating a deallocated pointer: p" verbose="Deallocating a deallocated pointer: p" cwe="415">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

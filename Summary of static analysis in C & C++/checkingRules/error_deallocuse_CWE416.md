# <center> deallocuse - CWE 416

指针p释放后，又继续使用*p

### Type

id = "deallocuse"  
severity = "error"

cwe = "CWE-416: Use After Free"  
cwe-type = "Base"

    <error id="deallocuse" severity="error" msg="Dereferencing &apos;varname&apos; after it is deallocated / released" verbose="Dereferencing &apos;varname&apos; after it is deallocated / released" cwe="416"/>



### Description

Referencing memory after it has been freed can cause a program to crash, use unexpected values, or execute code.

The use of previously-freed memory can have any number of adverse consequences, ranging from the corruption of valid data to the execution of arbitrary code, depending on the instantiation and timing of the flaw. The simplest way data corruption may occur involves the system's reuse of the freed memory. Use-after-free errors have two common and sometimes overlapping causes:  

- Error conditions and other exceptional circumstances.
- Confusion over which part of the program is responsible for freeing the memory.
  
In this scenario, the memory in question is allocated to another pointer validly at some point after it has been freed. The original pointer to the freed memory is used again and points to somewhere within the new allocation. As the data is changed, it corrupts the validly used memory; this induces undefined behavior in the process.

If the newly allocated data chances to hold a class, in C++ for example, various function pointers may be scattered within the heap data. If one of these function pointers is overwritten with an address to valid shellcode, execution of arbitrary code can be achieved.

More Detail：https://cwe.mitre.org/data/definitions/416.html  


### Example cpp file

	void main()
	{
		int* p=new int[1]
		p[0]=0;
		delete[] p;
		cout<<p[0];
	}


### Massage output in cppcheck
	
	[test.cpp:6]: (error) Dereferencing 'p' after it is deallocated / released



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="deallocuse" severity="error" msg="Dereferencing &apos;p&apos; after it is deallocated / released" verbose="Dereferencing &apos;p&apos; after it is deallocated / released" cwe="416">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>
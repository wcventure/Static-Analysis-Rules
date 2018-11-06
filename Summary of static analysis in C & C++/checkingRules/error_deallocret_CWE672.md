# <center> deallocret - CWE 672

资源释放后又作为返回值返回

### Type

id = "deallocret"  
severity = "error"

cwe = "CWE-672: Operation on a Resource after Expiration or Release"  
cwe-type = "Base"

    <error id="deallocret" severity="error" msg="Returning/dereferencing &apos;p&apos; after it is deallocated / released" verbose="Returning/dereferencing &apos;p&apos; after it is deallocated / released" cwe="672"/>


### Description

The software uses, accesses, or otherwise operates on a resource after that resource has been expired, released, or revoked.

More Detail：https://cwe.mitre.org/data/definitions/415.html  



### Example cpp file

资源释放后又作为返回值返回

	int *f(){
		int * p = (int *)malloc(sizeof(int));
		free(p);
		return p;
	}


### Massage output in cppcheck
	
	[test.cpp:4]: (error) Returning/dereferencing 'p' after it is deallocated / released



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="deallocret" severity="error" msg="Returning/dereferencing &apos;p&apos; after it is deallocated / released" verbose="Returning/dereferencing &apos;p&apos; after it is deallocated / released" cwe="672">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

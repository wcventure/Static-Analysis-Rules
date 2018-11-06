# <center> noCopyConstructor - CWE 398

类没有定义拷贝构造函数

### Type

id = "noCopyConstructor"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="noCopyConstructor" severity="style" msg="class &apos;class&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." verbose="class &apos;class&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." cwe="398"/>


### Description

class does not have a copy constructor which is recommended since the class contains a pointer to allocated memory.

More Detail：https://blog.csdn.net/fox64194167/article/details/21174575

### Example cpp file

	class base{
	private:
	   int i;
	   char *p;
	   char *q;
	public :
	    int j;
	
	   base();
	   ~base();
	   void memalloc();
	   void memdealloc();
	
	}
	base::base(){
	   p= malloc(12);
	}
	void base::memalloc(){
	  q = malloc(12);
	  free(p);
	}


### Massage output in cppcheck

	[test.cpp:1]: (style) class 'base' does not have a copy constructor which is recommended since the class contains a pointer to allocated memory.
	[test.cpp:19]: (warning) Possible leak in public function. The pointer 'q' is not deallocated before it is allocated.




### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="noCopyConstructor" severity="style" msg="class &apos;base&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." verbose="class &apos;base&apos; does not have a copy constructor which is recommended since the class contains a pointer to allocated memory." cwe="398">
	            <location file="test.cpp" line="1"/>
	        </error>
	        <error id="publicAllocationError" severity="warning" msg="Possible leak in public function. The pointer &apos;q&apos; is not deallocated before it is allocated." verbose="Possible leak in public function. The pointer &apos;q&apos; is not deallocated before it is allocated." cwe="398">
	            <location file="test.cpp" line="19"/>
	        </error>
	    </errors>
	</results>

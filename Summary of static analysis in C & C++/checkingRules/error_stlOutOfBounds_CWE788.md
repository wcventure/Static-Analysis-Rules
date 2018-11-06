# <center> stlOutOfBounds - CWE 788

SLT Vector 越界

### Type

id = "stlOutOfBounds"  
severity = "error"

cwe = "CWE-788: Access of Memory Location After End of Buffer"  
cwe-type = "Base"

    <error id="stlOutOfBounds" severity="error" msg="When i==foo.size(), foo[i] is out of bounds." verbose="When i==foo.size(), foo[i] is out of bounds." cwe="788"/>


### Description

The software reads or writes to a buffer using an index or pointer that references a memory location after the end of the buffer.

This typically occurs when a pointer or its index is decremented to a position before the buffer; when pointer arithmetic results in a position before the buffer; or when a negative index is used, which generates a position before the buffer.

More Detail：https://cwe.mitre.org/data/definitions/788.html  


### Example cpp file

out_of_range 
相当于数组的越界了。vector会自动增大容量，但是如果索引超出了当前的size，就会引发异常。

	include<iostream>
	include<vector>
	using namespace std;
	int main()
	{
	    vector<int>v(4);
	    std::cout << v[0] << std::endl;
	    std::cout << v[1] << std::endl;
	    std::cout << v[2] << std::endl;
	    std::cout << v[3] << std::endl;
	    std::cout << v[4] << std::endl;//越界
	    return 0;
	}

除了使用索引外，还有使用vector.at()时发生越界：

	include <iostream>       // std::cerr
	include <stdexcept>      // std::out_of_range
	include <vector>         // std::vector
	
	int main (void) {
	  std::vector<int> myvector(10);
	  try {
	    myvector.at(20)=100;      // vector::at throws an out-of-range
	  }
	  catch (const std::out_of_range& oor) {
	    std::cerr << "Out of Range error: " << oor.what() << '\n';
	  }
	  return 0;
	}

std::length_error 
在使用vector的时候，很少会引发std::length_error异常，但是如果疏忽大意写这样的代码：

	include <iostream>       // std::cerr
	include <stdexcept>      // std::length_error
	include <vector>         // std::vector
	
	int main (void) {
	  try {
	    // vector throws a length_error if resized above max_size
	    std::vector<int> myvector;
	    myvector.resize(myvector.max_size()+1);
	  }
	  catch (const std::length_error& le) {
	      std::cerr << "Length error: " << le.what() << '\n';
	  }
	  return 0;
	}

vector* pData;

https://blog.csdn.net/wangshubo1989/article/details/50374381
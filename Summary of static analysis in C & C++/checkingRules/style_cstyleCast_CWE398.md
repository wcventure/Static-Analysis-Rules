# <center> cstyleCast - CWE 398

用C语言的函数操作C++的类

### Type

id = "cstyleCast"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="cstyleCast" severity="style" msg="C-style pointer casting" verbose="C-style pointer casting detected. C++ offers four different kinds of casts as replacements: static_cast, const_cast, dynamic_cast and reinterpret_cast. A C-style cast could evaluate to any of those automatically, thus it is considered safer if the programmer explicitly states which kind of cast is expected. See also: https://www.securecoding.cert.org/confluence/display/cplusplus/EXP05-CPP.+Do+not+use+C-style+casts." cwe="398"/>



### Description

Unused private function



### Example cpp file

	include <stdio.h>  
	include <string.h>  
	include <malloc.h>  
	  
	class USER  
	{  
	public:  
	    USER()  
	    {  
	        printf("construct user\n");  
	    }  
	    ~USER()  
	    {  
	        printf("destory user\n");  
	    }  
	    int i;  
	};  
	  
	  
	int main(int argc, char* argv[])  
	{  
	    //用malloc分配,构造函数没有执行  
	    USER *u1=(USER*)malloc(sizeof(USER));  
	    free(u1); //没有调用析构函数  
	//  delete u1; //调用了一次析构函数  
	//  delete [] u1; //用delete[]释放没有用new[]创建的对象，将会出错  
	  
	  
	    //用new分配，构造函数得到执行  
	//  USER *u2=new USER;  
	//  free(u2); //没有调用析构函数  
	//  delete u2; //调用了一次析构函数  
	//  delete [] u2; //用delete[]释放没有用new[]创建的对象，将会出错  
	  
	  
	  
	//  //用new[]分配，[]中有多少个就执行多少次构造函数  
	//  USER *u3=new USER[3];  
	//  u3[0].i=1;  
	//  u3[1].i=2;  
	//  u3[2].i=3;  
	  
	  
	  
	//  free(u3-1); //释放出错  
	//  delete u3; //释放出错  
	//  delete [] u3; //用delete[]释放没有用new[]创建的对象，将会出错  
	  
	    return 0;  
	}  



### Massage output in cppcheck

	[test.cpp:19]: (style) C-style pointer casting



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="cstyleCast" severity="style" msg="C-style pointer casting" verbose="C-style pointer casting detected. C++ offers four different kinds of casts as replacements: static_cast, const_cast, dynamic_cast and reinterpret_cast. A C-style cast could evaluate to any of those automatically, thus it is considered safer if the programmer explicitly states which kind of cast is expected. See also: https://www.securecoding.cert.org/confluence/display/cplusplus/EXP05-CPP.+Do+not+use+C-style+casts." cwe="398">
	            <location file="test.cpp" line="19"/>
	        </error>
	    </errors>
	</results>
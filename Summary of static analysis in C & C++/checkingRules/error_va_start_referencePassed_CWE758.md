# <center> va_start_referencePassed - CWE 758

va_start()第一个参数不能是引用

### Type

id = "va_start_referencePassed"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

	<error id="va_start_referencePassed" severity="error" msg="Using reference &apos;arg1&apos; as parameter for va_start() results in undefined behaviour." verbose="Using reference &apos;arg1&apos; as parameter for va_start() results in undefined behaviour." cwe="758"/>


### Description

VA_LIST的用法： 
（1）首先在函数里定义一具VA_LIST型的变量，这个变量是指向参数的指针；   
（2）然后用VA_START宏初始化变量刚定义的VA_LIST变量；  
（3）然后用VA_ARG返回可变的参数，VA_ARG的第二个参数是你要返回的参数的类型（如果函数有多个可变参数的，依次调用VA_ARG获取各个参数）；  
（4）最后用VA_END宏结束可变参数的获取。  

使用VA_LIST应该注意的问题：   
（1）因为va_start, va_arg, va_end等定义成宏,所以它显得很愚蠢,可变参数的类型和个数完全在该函数中由程序代码控制,它并不能智能地识别不同参数的个数和类型. 也就是说,你想实现智能识别可变参数的话是要通过在自己的程序里作判断来实现的.   
（2）另外有一个问题,因为编译器对可变参数的函数的原型检查不够严格,对编程查错不利.不利于我们写出高质量的代码。   
（3）由于参数的地址用于VA_START宏，所以参数不能声明为寄存器变量，或作为函数或数组类型。  

More Detail: https://cwe.mitre.org/data/definitions/758.html  
[va_list原理及用法](https://blog.csdn.net/dengzhilong_cpp/article/details/54944676)

### Example cpp file

	void fun(char& ftm, ...)
	{
		va_list va;
		char* s1 = va_start(va,ftm);
		va_end(va);
	}

### Massage output in cppcheck

	[test.cpp:4]: (error) Using reference 'ftm' as parameter for va_start() results in undefined behaviour.

### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>	    
	        <error id="va_start_referencePassed" severity="error" msg="Using reference &apos;ftm&apos; as parameter for va_start() results in undefined behaviour." verbose="Using reference &apos;ftm&apos; as parameter for va_start() results in undefined behaviour." cwe="758">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

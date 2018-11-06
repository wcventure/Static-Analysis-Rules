# <center> mallocOnClassError - CWE 665

当malloc和string相遇时， 容易出错

### Type

id = "mallocOnClassError"  
severity = "error"

cwe = "CWE-665: Improper Initialization"  
cwe-type = "Class"

    <error id="mallocOnClassError" severity="error" msg="Memory for class instance allocated with malloc(), but class contains a std::string." verbose="Memory for class instance allocated with malloc(), but class a std::string. This is unsafe, since no constructor is called and class members remain uninitialized. Consider using &apos;new&apos; instead." cwe="665"/>



### Description

The software does not initialize or incorrectly initializes a resource, which might leave the resource in an unexpected state when it is accessed or used.

This can have security implications when the associated resource is expected to have certain properties or values, such as a variable that determines whether a user has been authenticated or not.

More Detail：https://cwe.mitre.org/data/definitions/665.html  



### Example cpp file

在学习C语言的时候， 我们学了malloc, 后来学习C++的时候， 又学了new, 那么malloc和new有什么区别呢？

首先, malloc是一个库函数， 返回值是void *形式的， 而new是一个运算符， 返回值类型与new的对象/变量的指针相同。

其次, new和delete的实现实际上是调用了malloc/free的。

最后， 介绍最重要的， 对于非内部类型来说， malloc是不能满足要求的， 因为malloc只是分配堆内存（不会调用构造函数）， 而new是分配且内存且在此创建一个对象（会调用构造函数）。

先看代码：
	include <iostream>  
	include <string>  
	using namespace std;  
	  
	class A            
	{  
	public:  
	    string str;  
	  
	    A()  
	    {  
	        cout << "A" << endl;  
	    }  
	};  
	  
	int main()   
	{  
	    A *q = (A *)malloc(sizeof(A));  
	    q->str = "hello"; // 运行error. 没有构造函数被调用， 所以， str根本就没有被初始化（string的构造函数没有被调用）， 所以不能赋值  
	  
	    return 0;  
	}  

调用malloc只是分配空间而已， 下面程序可以说明这点， 确实没有产生任何对象

[参考](https://blog.csdn.net/stpeace/article/details/41663381)

# <center> memsetClass - CWE 762

使用memset类包含一个虚拟方法是不安全的,因为构造函数、析构函数和拷贝操作符调用omitted。这些都是必要的对于这个non-POD确保创建一个有效的对象类型。

### Type

id = "memsetClass"  
severity = "error"

cwe = "CWE-762: Mismatched Memory Management Routines"  
cwe-type = "Variant"

    <error id="memsetClass" severity="error" msg="Using &apos;memfunc&apos; on class that contains a classname." verbose="Using &apos;memfunc&apos; on class that contains a classname is unsafe, because constructor, destructor and copy operator calls are omitted. These are necessary for this non-POD type to ensure that a valid object is created." cwe="762"/>



### Description

The application attempts to return a memory resource to the system, but it calls a release function that is not compatible with the function that was originally used to allocate that resource.

This weakness can be generally described as mismatching memory management routines, such as:

- The memory was allocated on the stack (automatically), but it was deallocated using the memory management routine free() (CWE-590), which is intended for explicitly allocated heap memory.  
- The memory was allocated explicitly using one set of memory management functions, and deallocated using a different set. For example, memory might be allocated with malloc() in C++ instead of the new operator, and then deallocated with the delete operator.  

When the memory management functions are mismatched, the consequences may be as severe as code execution, memory corruption, or program crash. Consequences and ease of exploit will vary depending on the implementation of the routines and the object being managed.

More Detail：https://cwe.mitre.org/data/definitions/762.html  



### Example cpp file

memset 某个结构（或其它的数据类型）在C语言中是很常见的代码，其目的是对结构（或其它的数据类型）进行初始化，通常都是将变量置为NULL或者0。在C++ 中，针对类对象除了用构造函数初始化对象外，也可以使用memset来进行初始化操作（确实有这种情况，不得已而为之，请大家不要把鸡蛋砸过来！）。但是 一定要注意以下所说的这种情况：如果类包含虚拟函数，则不能用 memset 来初始化类对象。下面是一个例子：

	class GraphicsObject{
	protected:
		char *m_pcName;
		int    m_iId;
		//etc
	public:
		virtual void Draw() {}
		virtual int Area() {}
		char* Name() { return m_pcName;}
	};
	
	class Circle: public GraphicsObject{
		void Draw() { /*draw something*/ }
		int Area() { /*calculate Area*/ }
	};
	
	void main()
	{
		GraphicsObject *obj = new Circle; // 创建对象
		memset((void *)obj,NULL,sizeof(Circle)); // memset to 0
		obj->Name(); // 非虚函数调用没问题
		obj->Draw(); // 在这里死得很难看
		deleted obj；
	}

结 果我就不说了。因为每个包含虚函数的类对象都有一个指针指向虚函数表（vtbl）。这个指针被用于解决运行时以及动态类型强制转换时虚函数的调用问题。该 指针是被隐藏的，对程序员来说，这个指针也是不可存取的。当进行memset操作时，这个指针的值也要被overwrite，这样一来，只要一调用虚函 数，程序便崩溃。这在很多由C转向C++的程序员来说，很容易犯这个错误，而且这个错误很难查。
为了避免这种情况，记住对于有虚拟函数的类对象，决不能使用 memset 来进行初始化操作。而是要用缺省的构造函数或其它的 init 例程来初始化成员变量。

### Massage output in cppcheck

[test.cpp:20]: (error) Using 'memset' on class that contains a virtual method.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memsetClass" severity="error" msg="Using &apos;memset&apos; on class that contains a virtual method." verbose="Using &apos;memset&apos; on class that contains a virtual method is unsafe, because constructor, destructor and copy operator calls are omitted. These are necessary for this non-POD type to ensure that a valid object is created." cwe="762">
	            <location file="test.cpp" line="20"/>
	        </error>
	    </errors>
	</results>


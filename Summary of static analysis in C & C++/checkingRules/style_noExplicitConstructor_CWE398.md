# <center> noExplicitConstructor - CWE 398

没有显式构造函数

### Type

id = "noExplicitConstructor"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="noExplicitConstructor" severity="style" msg="Class &apos;classname&apos; has a constructor with 1 argument that is not explicit." verbose="Class &apos;classname&apos; has a constructor with 1 argument that is not explicit. Such constructors should in general be explicit for type safety reasons. Using the explicit keyword in the constructor means some mistakes when using the class can be avoided." cwe="398"/>



### Description

Class has a constructor with 1 argument that is not explicit. Such constructors should in general be explicit for type safety reasons. Using the explicit keyword in the constructor means some mistakes when using the class can be avoided.

C++ 为类(Class)提供了许多默认函数。如果自己没有申明，编译器会为我们提供一个copy构造函数、一个copy assignment操作符和一个析构函数。此外，如果没有申明任何构造函数，编译器会为我们申明一个default构造函数。很像下面的Empty类：

	1 class Empty{
	2     public:
	3         Empty();
	4         Empty(const Empty &rhs);
	5         ~Empty();
	6         Empty& operator=(const Empty &rhs);
	7 };

就像Effective C++说的，如果不想使用编译器自动生成的函数，就应该明确拒绝。

1.C++中的隐式构造函数

　　如果c++类的其中一个构造函数有一个参数，那么在编译的时候就会有一个缺省的转换操作：将该构造函数对应数据类型的数据转换为该类对象。

2.Explicit Constructors显式构造函数

　　为了避免上面提到的只有一个参数的构造函数采用的缺省转换操作，在构造函数前，使用Explicit 关键字修饰即可。

3.如下面的例子：

	 1 #include <iostream>
	 2 using namespace std;
	 3 class B{
	 4     public:
	 5         int data;
	 6         B(int _data):data(_data){}
	 7         //explicit B(int _data):data(_data){}
	 8 };
	 9 
	10 int main(){
	11     B temp=5;
	12     cout<<temp.data<<endl;
	13     return 0;
	14 }

程序第11行，将int转换为B类型的对象，就是使用了隐式构造函数。因为B中存在一个只有一个参数的构造函数，且参数类型也是int。

如果在构造函数前加explicit ，表示为显示构造函数，则第11行编译不能通过。因为这时，就没有隐式构造函数啦。

### Example cpp file

	class A
	{
	public:
		A(int i){a=i;}
		~A(){}
	private:
		int a;
	};



### Massage output in cppcheck

	[test.cpp:4]: (style) Class 'A' has a constructor with 1 argument that is not explicit.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="noExplicitConstructor" severity="style" msg="Class &apos;A&apos; has a constructor with 1 argument that is not explicit." verbose="Class &apos;A&apos; has a constructor with 1 argument that is not explicit. Such constructors should in general be explicit for type safety reasons. Using the explicit keyword in the constructor means some mistakes when using the class can be avoided." cwe="398">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>
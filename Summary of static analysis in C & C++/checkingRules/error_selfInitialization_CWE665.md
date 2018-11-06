# <center> selfInitialization - CWE 665

初始化列表初始化异常

### Type

id = "selfInitialization"  
severity = "error"

cwe = "CWE-665: Improper Initialization"  
cwe-type = "Class"

    <error id="selfInitialization" severity="error" msg="Member variable &apos;var&apos; is initialized by itself." verbose="Member variable &apos;var&apos; is initialized by itself." cwe="665"/>



### Description

More Detail：http://www.jb51.net/article/100761.htm  

除了性能问题之外，有些时场合初始化列表是不可或缺的，以下几种情况时必须使用初始化列表

常量成员，因为常量只能初始化不能赋值，所以必须放在初始化列表里面
引用类型，引用必须在定义的时候初始化，并且不能重新赋值，所以也要写在初始化列表里面
没有默认构造函数的类类型，因为使用初始化列表可以不必调用默认构造函数来初始化，而是直接调用拷贝构造函数初始化。

### Example


成员变量的初始化顺序

成员是按照他们在类中出现的顺序进行初始化的，而不是按照他们在初始化列表出现的顺序初始化的，看代码。

	struct foo
	{
	  int i ;
	  int j ;
	  foo(int x):i(x), j(i){}; // ok, 先初始化i，后初始化j
	};

再看下面的代码

	struct foo
	{
	  int i ;
	  int j ;
	  foo(int x):j(x), i(j){} // i值未定义
	};

这里i的值是未定义的，虽然j在初始化列表里面出现在i前面，但是i先于j定义，所以先初始化i，但i由j初始化，此时j尚未初始化，所以导致i的值未定义。所以，一个好的习惯是，按照成员定义的顺序进行初始化。
	
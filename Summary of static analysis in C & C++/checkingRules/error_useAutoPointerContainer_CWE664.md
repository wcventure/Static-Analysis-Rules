# <center> useAutoPointerContainer - CWE 664

auto_ptr智能指针不能作为STL标准容器的元素

### Type

id = "useAutoPointerContainer"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="useAutoPointerContainer" severity="error" msg="You can randomly lose access to pointers if you store &apos;auto_ptr&apos; pointers in an STL container." verbose="An element of container must be able to be copied but &apos;auto_ptr&apos; does not fulfill this requirement. You should consider to use &apos;shared_ptr&apos; or &apos;unique_ptr&apos;. It is suitable for use in containers, because they no longer copy their values, they move them." cwe="664"/>


### Description

An element of container must be able to be copied but auto_ptr does not fulfill this requirement. You should consider to use shared_ptr or unique_ptr. It is suitable for use in containers, because they no longer copy their values, they move them.

You can randomly lose access to pointers if you store pointers in an STL container.

More Detail：
[为什么auto_ptr智能指针不能作为STL标准容器的元素](https://blog.csdn.net/yanglingwell/article/details/56011576)  
[STL中的智能指针(Smart Pointer)及其源码剖析： std::auto_ptr](https://my.oschina.net/costaxu/blog/105101)  


### Example cpp file

为什么auto_ptr不可以作为STL标准容器的元素，而shared_ptr可以? 

	int costa_foo()
	{
	    vector< auto_ptr<int> > v(10);
	    int i=0;
	    for(;i<10;i++)
	    {   
	        v[i]=auto_ptr<int>(new int(i));
	    }   
	}

错误出在这一行:

	vector< auto_ptr<int> > v(10);

这个错误是什么含义呢，我们看stl_construct.h的74行所在的函数：

	64   /**
	65    * Constructs an object in existing memory by invoking an allocated
	66    * object's constructor with an initializer.
	67    */
	68   template<typename _T1, typename _T2>
	69     inline void
	70     _Construct(_T1* __p, const _T2& __value)
	71     {
	72       // _GLIBCXX_RESOLVE_LIB_DEFECTS
	73       // 402. wrong new expression in [some_]allocator::construct
	74       ::new(static_cast<void*>(__p)) _T1(__value);
	75     }

我来直接说这个函数的作用：把第二个参数__T2& value拷贝构造一份，然后复制到T1这个指针所指向的位置。它是如何做到的呢？

看第第74行, 这里使用new的方法和我们平常所见到的似乎略有不同。 这是一个placement new。 placement new的语法是：

	new(p) T(value)

placement new并不会去堆上申请一块内存，而是直接使用指针p指向的内存，将value对象拷贝一份放到p指向的内存上去。

看到这里我就知道为什么编译器在编译本文开头的那段代码时会报这段错误” /usr/include/c++/4.4/bits/stl_construct.h:74: error: passing ‘const std::auto_ptr<int>’ as ‘this’ argument of ‘std::auto_ptr<_Tp>::operator std::auto_ptr_ref<_Tp1>() [with _Tp1 = int, _Tp = int]’ discards qualifiers" 了。通过查看auto_ptr的源代码（位置在 c++头文件目录的 backward/auto_ptr.h)可以发现， auto_ptr并没有一个参数为const auto_ptr& 的拷贝构造函数。换而言之， auto_ptr进行拷贝构造的时候，必需要修改作为参数传进来的那个auto_ptr。

auto_ptr的拷贝构造函数是这样写的:

	auto_ptr(auto_ptr& __a) throw() : _M_ptr(__a.release()) { }

可以看出来， 在拷贝构造一个auto_tr的时候， 必需要把参数的那个auto_ptr给release掉，然后在把参数的_M_ptr指针赋值给自己的_M_ptr指针。补充说明一下， _M_ptr是auto_ptr的一个成员，指向auto_ptr管理的那块内存，也就是在auto_ptr生命周期之外被释放掉的那块内存。

当看到这里的时候，整个问题已经能解释通了。STL容器在分配内存的时候，必须要能够拷贝构造容器的元素。而且拷贝构造的时候，不能修改原来元素的值。而auto_ptr在拷贝构造的时候，一定会修改元素的值。所以STL元素不能使用auto_ptr。

不过，还有一个很重要的问题没有解释。那就是为什么在设计auto_ptr的时候，拷贝构造要修改参数的值呢？

其实这问题很简单，不看代码也可以解释清楚。auto_ptr内部有一个指针成员_M_ptr，指向它所管理的那块内存。而拷贝构造的时候，首先把参数的_M_ptr的值赋值给自己的_M_ptr，然后把参数的_M_ptr指针设成NULL，。如果不这样设计，而是直接把参数的_M_ptr指针赋值给自己的， 那么两个auto_ptr的_M_ptr指向同一块内存，在析构auto_ptr的时候就会出问题： 假如两个auto_ptr的_M_ptr指针指向了同一块内存，那么第一个析构的那个auto_ptr指针就把_M_ptr指向的内存释放掉了，造成后一个auto_ptr在析构时释要放一块已经被释放掉的内存，这明显不科学，会产生程序的段错误而crash掉。

而shared_ptr则不存在这个问题， 在拷贝构造和赋值操作的时候，只会引起公用的引用计数的+1，不存在拷贝构造和赋值操作的参数不能是const的问题。

总结：

1 auto_ptr不能作为STL标准容器的元素。

2 auto_ptr在拷贝复制和赋值操作时，都会改变参数。这是因为两个auto_ptr不能管理同一块内存。

### Massage output in cppcheck

	[test.cpp:3]: (error) You can randomly lose access to pointers if you store 'auto_ptr' pointers in an STL container.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="useAutoPointerContainer" severity="error" msg="You can randomly lose access to pointers if you store &apos;auto_ptr&apos; pointers in an STL container." verbose="An element of container must be able to be copied but &apos;auto_ptr&apos; does not fulfill this requirement. You should consider to use &apos;shared_ptr&apos; or &apos;unique_ptr&apos;. It is suitable for use in containers, because they no longer copy their values, they move them." cwe="664">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>

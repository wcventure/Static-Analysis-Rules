# <center> useAutoPointerCopy - CWE 398

因为一个auto_ptr被拷贝或被赋值后，　其已经失去对原对象的所有权，这个时候，对这个auto_ptr的提领(dereference)操作是不安全的。

### Type

id = "useAutoPointerCopy"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="useAutoPointerCopy" severity="style" msg="Copying &apos;auto_ptr&apos; pointer to another does not create two equal objects since one has lost its ownership of the pointer." verbose="&apos;std::auto_ptr&apos; has semantics of strict ownership, meaning that the &apos;auto_ptr&apos; instance is the sole entity responsible for the object&apos;s lifetime. If an &apos;auto_ptr&apos; is copied, the source looses the reference." cwe="398"/>


### Description



More Detail：https://baike.baidu.com/item/auto_ptr/147859

### Example cpp file

因为一个auto_ptr被拷贝或被赋值后，　其已经失去对原对象的所有权，这个时候，对这个auto_ptr的提领(dereference)操作是不安全的。如下:

	void main()
	{
		int*p=new int(0);
		auto_ptr<int>ap1(p);
		auto_ptr<int>ap2=ap1;
		cout<<*ap1;//错误，此时ap1只剩一个null指针在手了
	}

这种情况较为隐蔽的情形出现在将auto_ptr作为函数参数按值传递，因为在函数调用过程中在函数的作用域中会产生一个局部对象来接收传入的auto_ptr(拷贝构造)，这样，传入的实参auto_ptr就失去了其对原对象的所有权，而该对象会在函数退出时被局部auto_ptr删除。如下：

	void f(auto_ptr<int>ap)
	{
	    cout<<*ap;
	}
	 
	auto_ptr<int>ap1(new int(0));
	f(ap1);
	cout<<*ap1;//错误，经过f(ap1)函数调用，ap1已经不再拥有任何对象了。

因为这种情况太隐蔽，太容易出错了，　所以auto_ptr作为函数参数按值传递是一定要避免的。或许大家会想到用auto_ptr的指针或引用作为函数参数或许可以，但是仔细想想，我们并不知道在函数中对传入的auto_ptr做了什么，　如果当中某些操作使其失去了对对象的所有权，　那么这还是可能会导致致命的执行期错误。　也许，用const reference的形式来传递auto_ptr会是一个不错的选择。


### Massage output in cppcheck

	[test.cpp:5]: (style) Copying 'auto_ptr' pointer to another does not create two equal objects since one has lost its ownership of the pointer.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="useAutoPointerCopy" severity="style" msg="Copying &apos;auto_ptr&apos; pointer to another does not create two equal objects since one has lost its ownership of the pointer." verbose="&apos;std::auto_ptr&apos; has semantics of strict ownership, meaning that the &apos;auto_ptr&apos; instance is the sole entity responsible for the object&apos;s lifetime. If an &apos;auto_ptr&apos; is copied, the source looses the reference." cwe="398">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>


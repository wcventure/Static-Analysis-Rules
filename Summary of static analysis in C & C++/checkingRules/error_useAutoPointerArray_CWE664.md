# <center> useAutoPointerArray - CWE 664

因为auto_ptr的析构函数中删除指针用的是delete,而不是delete [],所以我们不应该用auto_ptr来管理一个数组指针。

### Type

id = "useAutoPointerArray"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="useAutoPointerArray" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new[]&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes arrays, which are allocated by operator &apos;new[]&apos; and must be deallocated by operator &apos;delete[]&apos;." cwe="664"/>


### Description

Object pointed by an auto_ptr is destroyed using operator delete. This means that you should only use auto_ptr for pointers obtained with operator “new”. This excludes arrays, which are allocated by operator new[]; and must be deallocated by operator delete[].

More Detail：
[auto_ptr百度百科](https://baike.baidu.com/item/auto_ptr/147859)  

### Example cpp file

因为auto_ptr的析构函数中删除指针用的是delete,而不是delete [],所以我们不应该用auto_ptr来管理一个数组指针。

	void main()
	{
		int*pa=new int[10];
		auto_ptr<int>ap(pa);
	} 


### Massage output in cppcheck

	[test.cpp:4]: (error) Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. You should not use 'auto_ptr' for pointers obtained with operator 'new[]'.




### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="useAutoPointerArray" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new[]&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes arrays, which are allocated by operator &apos;new[]&apos; and must be deallocated by operator &apos;delete[]&apos;." cwe="664">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

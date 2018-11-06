# <center> useAutoPointerMalloc - CWE 762

因为auto_ptr的析构函数中删除指针用的是delete,而不是delete [],所以我们不应该用auto_ptr来管理一个数组指针。

### Type

id = "useAutoPointerMalloc"  
severity = "error"

cwe = "CWE-762: Mismatched Memory Management Routines"  
cwe-type = "Variant"

    <error id="useAutoPointerMalloc" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes use C library allocation functions (for example &apos;malloc&apos;), which must be deallocated by the appropriate C library function." cwe="762"/>


### Description

Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. You should not use 'auto_ptr' for pointers obtained with function 'malloc'.

More Detail：
[auto_ptr百度百科](https://baike.baidu.com/item/auto_ptr/147859)  

### Example cpp file

因为auto_ptr的析构函数中删除指针用的是delete,所以我们不应该用malloc来分配指针的空间。

	void main()
	{
		int *pa = malloc(sizeof(int)*10);
		auto_ptr<int>ap(pa);
	} 


### Massage output in cppcheck

	[test.cpp:4]: (error) Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. You should not use 'auto_ptr' for pointers obtained with function 'malloc'.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="useAutoPointerMalloc" severity="error" msg="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;." verbose="Object pointed by an &apos;auto_ptr&apos; is destroyed using operator &apos;delete&apos;. You should not use &apos;auto_ptr&apos; for pointers obtained with function &apos;malloc&apos;. This means that you should only use &apos;auto_ptr&apos; for pointers obtained with operator &apos;new&apos;. This excludes use C library allocation functions (for example &apos;malloc&apos;), which must be deallocated by the appropriate C library function." cwe="762">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

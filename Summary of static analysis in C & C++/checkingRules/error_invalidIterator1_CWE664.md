# <center> invalidIterator1 - CWE 664

删除，添加等操作可能会使当前迭代器失效

### Type

id = "invalidIterator1"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

	<error id="invalidIterator1" severity="error" msg="Invalid iterator: iterator" verbose="Invalid iterator: iterator" cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：

[CWE-664: Improper Control of a Resource Through its Lifetime:https://cwe.mitre.org/data/definitions/664.html](https://cwe.mitre.org/data/definitions/664.html)
[STL源码剖析---迭代器失效小结:https://blog.csdn.net/Hackbuteer1/article/details/7734382](https://blog.csdn.net/Hackbuteer1/article/details/7734382)    
[迭代器失效的几种情况总结:https://blog.csdn.net/lujiandong1/article/details/49872763](https://blog.csdn.net/lujiandong1/article/details/49872763)
[C++注意事项-----------删除，添加等操作可能会使当前迭代器失效:https://blog.csdn.net/xiaoyaohuqijun/article/details/49105903](https://blog.csdn.net/xiaoyaohuqijun/article/details/49105903)  
[容器大小的改变以及容器操作可能使迭代器失效、vector对象的容量变化:https://www.cnblogs.com/wuchanming/p/3915573.html](https://www.cnblogs.com/wuchanming/p/3915573.html)  

**stl可能使迭代器失效的操作**

1、vector、string

插入元素时，若引起空间重配，则迭代器、引用、指针都失效。若未引起空间重配，则插入位置之前的迭代器、引用、指针仍有效，但指向插入位置之后的迭代器、引用、指针将会失效。

删除元素时，删除位置之前的迭代器、引用、指针仍有效，但删除位置之后的迭代器、引用、指针一律无效。


2、deque

插入元素时，若插入到除首尾位置之外的任何位置，都会引起迭代器、引用、指针失效。如果在首尾位置插入元素，则迭代器失效，引用和指针不会失效。

删除元素时，若删除的不是首尾元素，则迭代器、引用、指针都会失效。若删除的是首元素，则除指向首元素的迭代器、引用、指针收到影响，指向其他元素的迭代器、引用、指针仍有效。删除尾元素也是一样，只有被删除的元素受到影响，其他不会受到影响。


3、list，forward_list

插入和删除元素时，其他迭代器、指针、引用都仍然有效！

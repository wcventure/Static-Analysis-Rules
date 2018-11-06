# <center> stlBoundaries - CWE 664

使用迭代器上的操作符进行危险比较

### Type

id = "stlBoundaries"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

	<error id="stlBoundaries" severity="error" msg="Dangerous comparison using operator&lt; on iterator." verbose="Iterator compared with operator&lt;. This is dangerous since the order of items in the container is not guaranteed. One should use operator!= instead to compare iterators." cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html


### Example cpp file

为什么两个不同的vector（类型一样），他们的迭代器一比较就出错呢？比如下面的代码

	//include<iostream>
	//include<vector>
	//using namespace std;
	 
	int main()
	{
	    int a[2]={1,2};
	    vector<int>::iterator it1,it2;
	    vector<int> ivec1(a,a+2);
	    vector<int> ivec2(a,a+2);
	    it1=ivec1.begin();
	    it2=ivec2.begin();
	    bool b=it1<it2;
	    system("pause");
	    return 0;
	}

迭代器差不多是一个对象！ 只不过是STL重载了++ --等运算符， 迭代器对象直接比较是危险操作
# <center> stlcstr - CWE 664

删除，添加等操作可能会使当前迭代器失效

### Type

id = "stlcstr"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="stlcstr" severity="error" msg="Dangerous usage of c_str(). The value returned by c_str() is invalid after this call." verbose="Dangerous usage of c_str(). The c_str() return value is only valid until its string is deleted." cwe="664"/>



### Description

Dangerous usage of c_str(). The c_str() return value is only valid until its string is deleted.

	//标准库的string类提供了三个成员函数来从一个string得到c类型的字符数组  
	//主要介绍c_str  
	//c_str()：生成一个const char*指针，指向以空字符终止的数组。  
	//这个数组应该是string类内部的数组  
	//include <iostream>  
	//需要包含cstring的字符串  
	//include <cstring>  
	//using namespace std;  
	  
	int main()  
	{  
	    //string-->char*  
	    //c_str()函数返回一个指向正规C字符串的指针, 内容与本string串相同  
	  
	    //这个数组的数据是临时的，当有一个改变这些数据的成员函数被调用后，其中的数据就会失效。  
	    //因此要么现用先转换，要么把它的数据复制到用户自己可以管理的内存中  
	    const char *c;  
	    string s = "1234";  
	    c = s.c_str();  
	    cout<<c<<endl;  
	    s = "abcde";  
	    cout<<c<<endl;  
	} 

输出结果是：
	1234
	abcde

上面如果继续用c指针的话，导致的错误将是不可想象的。就如：1234变为abcd

其实上面的c = s.c_str(); 不是一个好习惯。既然c指针指向的内容容易失效，我们就应该按照上面的方法，那怎么把数据复制出来呢？这就要用到strcpy等函数（推荐）。

更好的处理为：
	
	int main()  
	{  
	    //更好的方法是将string数组中的内容复制出来 所以会用到strcpy()这个函数  
	    char *c = new char[20];  
	    string s = "1234";  
	    // c_str()返回一个客户程序可读不可改的指向字符数组的指针，不需要手动释放或删除这个指针。  
	    strcpy(c,s.c_str());  
	    cout<<c<<endl;  
	    s = "abcd";  
	    cout<<c<<endl;  
	}   
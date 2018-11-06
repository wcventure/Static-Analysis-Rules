# <center> memleakOnRealloc - CWE 401



### Type

id = "memleakOnRealloc"  
severity = "error"

cwe = "CWE-401: Improper Release of Memory Before Removing Last Reference ('Memory Leak')"  
cwe-type = "Base"

    <error id="memleakOnRealloc" severity="error" msg="Common realloc mistake: &apos;varname&apos; nulled but not freed upon failure" verbose="Common realloc mistake: &apos;varname&apos; nulled but not freed upon failure" cwe="401"/>



### Description

The software does not sufficiently track and release allocated memory after it has been used, which slowly consumes remaining memory.

This is often triggered by improper handling of malformed data or unexpectedly interrupted sessions.

CppCheck进行源码安全性扫描时出现的error警告，会在realloc函数失败时导致原有申请的内存引用丢失而原有申请的内存并未被释放而导致内存泄漏，因此需要先保留realloc函数之前已有的内存指针，在realloc后判断是否成功，成功再进行赋值，这样失败以后保留了原申请内存的引用不会导致内存泄漏。

错误范例：

	d_data = (char *)realloc (d_data, size*sizeof(char));

解决办法：

	char *new_data = (char *)realloc (d_data, size*sizeof(char));
	 if (new_data == NULL)
	 {
	   // could not realloc, but orig still valid
	   cout<<”realloc failed”<<endl;
	 }
	 else
	 {
	     d_data = new_data;
	 }

More Detail：https://cwe.mitre.org/data/definitions/401.html   
https://blog.csdn.net/a472770699/article/details/46233949


### Example cpp file

详细说明及注意要点：  

1、如果有足够空间用于扩大mem_address指向的内存块，则分配额外内存，并返回mem_address  
这里说的是“扩大”，我们知道，realloc是从堆上分配内存的，当扩大一块内存空间时， realloc()试图直接从堆上现存的数据后面的那些字节中获得附加的字节，如果能够满足，自然天下太平。也就是说，如果原先的内存大小后面还有足够的空闲空间用来分配，加上原来的空间大小＝ newsize。那么就ok。得到的是一块连续的内存。   

2、如果原先的内存大小后面没有足够的空闲空间用来分配，那么从堆中另外找一块newsize大小的内存。  
并把原来大小内存空间中的内容复制到newsize中。返回新的mem_address指针。（数据被移动了）。  
老块被放回堆上。  

例如： 
	void main(){
		char *p;
		char *q; 
		p = (char * )malloc (10); 
		q = p; 
		p = (char *) realloc (p,20); 
	}

这段程序也许在编译器中没有办法通过，因为编译器可能会为我们消除一些隐患！在这里我们只是增加了一个记录原来内存地址的指针q，然后记录了原来的内存地址p，如果不幸的话，数据发生了移动，那么所记录的原来的内存地址q所指向的内存空间实际上已经放回到堆上了!这样一来，我们应该终于意识到问题的所在和可怕了吧！  
 
3、返回情况 
返回的是一个void类型的指针，调用成功。（这就再你需要的时候进行强制类型转换）  
返回NULL，当需要扩展的大小（第二个参数）为0并且第一个参数不为NULL，此时原内存变成了“freed（游离）”的了。   
返回NULL，当没有足够的空间可供扩展的时候，此时，原内存空间的大小维持不变。  
 
4、特殊情况 
如果mem_address为null，则realloc()和malloc()类似。分配一个newsize的内存块，返回一个指向该内存块的指针。
如果newsize大小为0，那么释放mem_address指向的内存，并返回null。 
如果没有足够可用的内存用来完成重新分配（扩大原来的内存块或者分配新的内存块），则返回null.而原来的内存块保持不变。

### Massage output in cppcheck

	[test.cpp:6]: (error) Common realloc mistake: 'p' nulled but not freed upon failure



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memleakOnRealloc" severity="error" msg="Common realloc mistake: &apos;p&apos; nulled but not freed upon failure" verbose="Common realloc mistake: &apos;p&apos; nulled but not freed upon failure" cwe="401">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>

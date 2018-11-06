# <center> sctimeCalled - CWE 477

过时的函数asctime调用。推荐使用strftime函数	

### Type

id = "assignBoolToFloat"  
severity = "style"

cwe = "CWE-477: Use of Obsolete Function"  
cwe-type = "Class"

    <error id="sctimeCalled" severity="style" msg="Obsolete function &apos;asctime&apos; called. It is recommended to use &apos;strftime&apos; instead." verbose="Obsolete function &apos;asctime&apos; called. It is recommended to use &apos;strftime&apos; instead." cwe="477">


### Description

The code uses deprecated or obsolete functions, which suggests that the code has not been actively reviewed or maintained.

As programming languages evolve, functions occasionally become obsolete due to:  
- Advances in the language
- Improved understanding of how operations should be performed effectively and securely
- Changes in the conventions that govern certain operations

Functions that are removed are usually replaced by newer counterparts that perform the same task in some different and hopefully improved way.

More Detail：https://cwe.mitre.org/data/definitions/704.html  


### Example cpp file

函数原型:char *asctime(const struct tm *tblock);

功能说明: 将tm结构类型时间日期转换为ASCII码。

头文件:

参数说明:语法:asctime[必要参数]

 必要参数的具体说明如下:

 tblock:待转换的时间指针，tblock为一tm结构型指针。

   返回值:返回由tm结构中的日期和时间转换成的字符串的地址，该字符串的形式定义如下:

DDD MMM dd hh:mm:ss YYYY

   各字符的意义:

   DDD   一星期的某一天,如Mon

MMM  月份,如Jan

dd     月中一天（1,2,……,31）

hh     小时（1,2,……,24）

mm    分钟（1,2,……,59）

ss      秒数（1,2,……,59）

YYYY  年

   范例: 转换日期和时间为ASCII码


	void main()
	
	{
	
	    time_t t;
	
	    struct tm *p;
	
	    t=time(NULL);
	
	    p=localtime(&t);
	
		printf("%s\n", asctime(p));
	}

类似的函数有
函数原型:char *ctime(const time_t *time);

功能说明:将time_t类型日期和时间转换为字符串。

头文件:

参数说明:语法:ctime[必要参数]

必要参数的具体说明如下: time:指向time_t结构的指针。


### Massage output in cppcheck

	[test.cpp:13]: (style) Obsolete function 'asctime' called. It is recommended to use 'strftime' instead.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="asctimeCalled" severity="style" msg="Obsolete function &apos;asctime&apos; called. It is recommended to use &apos;strftime&apos; instead." verbose="Obsolete function &apos;asctime&apos; called. It is recommended to use &apos;strftime&apos; instead." cwe="477">
	            <location file="test.cpp" line="13"/>
	        </error>
	    </errors>
	</results>


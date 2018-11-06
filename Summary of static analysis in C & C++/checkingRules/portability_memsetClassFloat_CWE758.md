# <center> memsetClassFloat - CWE 758

memset是以字节为单位进行赋值,用们memset对非字符型数组赋初值不可取

### Type

id = "memsetClassFloat"  
severity = "portability"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

    <error id="memsetClassFloat" severity="portability" msg="Using memset() on class which contains a floating point number." verbose="Using memset() on class which contains a floating point number. This is not portable because memset() sets each byte of a block of memory to a specific value and the actual representation of a floating-point value is implementation defined. Note: In case of an IEEE754-1985 compatible implementation setting all bits to zero results in the value 0.0." cwe="758"/>


### Description

The software uses an API function, data structure, or other entity in a way that relies on properties that are not always guaranteed to hold for that entity.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://cwe.mitre.org/data/definitions/758.html  



### Example cpp file
程序示例：

	\\1.
	unsigned char (*p)[40]=new unsigned char [100][40];
	memset(p,1,100*40);
	
	\\2.
	float (*p)[40]=new float[100][40];
	memset(p,1,100*40);
	
	\\3.
	float (*p)[40]=new float[100][40];
	memset(p,0,100*40);

程序1可以达到想要的目标，但是2和3都达不到想要的目标。这是为什么呢？

因为memset是以字节为单位进行赋值，每个都用ASCII为1的字符去填充，转为二进制后，1就是00000001,占一个字节。一个float元素是4字节，合一起就是00000001000000010000000100000001，转换成十进制明显不是我们想要的结果。而3是没有把所有的内存全部清空。

所以用memset对非字符型数组赋初值是不可取的！清空内存却可以做到，正确的做法如4：

	\\4。
	float (*p)[40]=new float[100][40];
	memset(p,0,100*40*sizeof(float));
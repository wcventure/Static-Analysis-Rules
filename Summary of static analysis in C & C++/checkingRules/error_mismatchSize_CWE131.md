# <center> mismatchSize - CWE 131

在分配缓冲区时，软件没有正确计算要使用的大小，这可能导致缓冲区溢出。

### Type

id = "mismatchSize"  
severity = "error"

cwe = "CWE-131: Incorrect Calculation of Buffer Size"  
cwe-type = "Base"

    <error id="mismatchSize" severity="error" msg="The allocated size sz is not a multiple of the underlying type&apos;s size." verbose="The allocated size sz is not a multiple of the underlying type&apos;s size." cwe="131"/>



### Description

The software does not correctly calculate the size to be used when allocating a buffer, which could lead to a buffer overflow.

More Detail：https://cwe.mitre.org/data/definitions/131.html  



### Example cpp file

The following code attempts to save three different identification numbers into an array. The array is allocated from memory using a call to malloc().

	void main(){
		/* Allocate space for an array of three ids. */ 	
		int *id_sequence;
	
		id_sequence = (int*) malloc(3);
		if (id_sequence == NULL) exit(1);
		/* Populate the id array. */ 
		id_sequence[0] = 13579;
		id_sequence[1] = 24680;
		id_sequence[2] = 97531;
	}

The problem with the code above is the value of the size parameter used during the malloc() call. It uses a value of '3' which by definition results in a buffer of three bytes to be created. However the intention was to create a buffer that holds three ints, and in C, each int requires 4 bytes worth of memory, so an array of 12 bytes is needed, 4 bytes for each int. Executing the above code could result in a buffer overflow as 12 bytes of data is being saved into 3 bytes worth of allocated space. The overflow would occur during the assignment of id_sequence[0] and would continue with the assignment of id_sequence[1] and id_sequence[2].

The malloc() call could have used '3*sizeof(int)' as the value for the size parameter in order to allocate the correct amount of space required to store the three ints.


### Massage output in cppcheck

	[test.cpp:11]: (error) Memory leak: id_sequence
	[test.cpp:5]: (error) The allocated size 3 is not a multiple of the underlying type's size.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memleak" severity="error" msg="Memory leak: id_sequence" verbose="Memory leak: id_sequence" cwe="401">
	            <location file="test.cpp" line="11"/>
	        </error>
	        <error id="mismatchSize" severity="error" msg="The allocated size 3 is not a multiple of the underlying type&apos;s size." verbose="The allocated size 3 is not a multiple of the underlying type&apos;s size." cwe="131">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>



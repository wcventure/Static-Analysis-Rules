# <center> insecureCmdLineArgs - CWE 119

长命令行参数可能使缓冲区溢出

### Type

id = "insecureCmdLineArgs"  
severity = "error"

cwe = "CWE-119: Improper Restriction of Operations within the Bounds of a Memory Buffer"  
cwe-type = "class"

    <<error id="insecureCmdLineArgs" severity="error" msg="Buffer overrun possible for long command line arguments." verbose="Buffer overrun possible for long command line arguments." cwe="119"/>


### Description

The software performs operations on a memory buffer, but it can read from or write to a memory location that is outside of the intended boundary of the buffer.

Certain languages allow direct addressing of memory locations and do not automatically ensure that these locations are valid for the memory buffer that is being referenced. This can cause read or write operations to be performed on memory locations that may be associated with other variables, data structures, or internal program data.

As a result, an attacker may be able to execute arbitrary code, alter the intended control flow, read sensitive information, or cause the system to crash.  

More Detail：https://cwe.mitre.org/data/definitions/119.html  
http://www.docin.com/p-1015625802.html


### Example cpp file

	void DontDoThis(char *input)
	{
		char buf[16];
		strcpy(buf, input);
		printf("%s/n",buf);
	}
	
	void main(int argc, char* argv[])
	{
		DontDoThis(argv[1]);
	}
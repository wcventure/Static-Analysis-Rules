# <center> leakNoVarFunctionCall - CWE 772

在有效的生命周期后没有释放资源

### Type

id = "leakNoVarFunctionCall"  
severity = "error"

cwe = "CWE-772: Missing Release of Resource after Effective Lifetime"  
cwe-type = "base"

	<error id="leakNoVarFunctionCall" severity="error" msg="Allocation with funcName, funcName doesn&apos;t release it." verbose="Allocation with funcName, funcName doesn&apos;t release it." cwe="772"/>



### Description

The software does not release a resource after its effective lifetime has ended, i.e., after the resource is no longer needed.
+ Extended Description
When a resource is not released after use, it can allow attackers to cause a denial of service.

More Detail：https://cwe.mitre.org/data/definitions/772.html  



### Example cpp file

The following C function does not close the file handle it opens if an error occurs. If the process is long-lived, the process can run out of file handles.

	\\(bad code)
	\\Example Language: C 
	int decodeFile(char* fName) {
		char buf[BUF_SZ];
		FILE* f = fopen(fName, "r");
		if (!f) {
			printf("cannot open %s\n", fName);
			return DECODE_FAIL;
		}
		else {
			while (fgets(buf, BUF_SZ, f)) {
				if (!checkChecksum(buf)) {
					return DECODE_FAIL;
				}
				else {
					decodeBlock(buf);
				}
			}
		}
		fclose(f);
		return DECODE_SUCCESS;
	}
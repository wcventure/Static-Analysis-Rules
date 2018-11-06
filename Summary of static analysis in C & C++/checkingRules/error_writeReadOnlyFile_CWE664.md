# <center> writeReadOnlyFile - CWE 664

在只为写入而打开的文件上做读取操作。

### Type

id = "writeReadOnlyFile"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="readWriteOnlyFile" severity="error" msg="Read operation on a file that was opened only for writing." verbose="Read operation on a file that was opened only for writing." cwe="664"/>



### Description

Read operation on a file that was opened only for writing

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

No examples can be found



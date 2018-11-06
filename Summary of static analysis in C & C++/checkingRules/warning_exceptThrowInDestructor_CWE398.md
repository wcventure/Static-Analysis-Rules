# <center> exceptThrowInDestructor - CWE 398

不能在析构函数里面抛出异常	

### Type

id = "exceptThrowInDestructor"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="exceptThrowInDestructor" severity="warning" msg="Class Class is not safe, destructor throws exception" verbose="The class Class is not safe because its destructor throws an exception. If Class is used and an exception is thrown that is caught in an outer scope the program will terminate." cwe="398"/>


### Description

https://blog.csdn.net/wind19/article/details/8213406	

从语法上面讲，析构函数抛出异常是可以的，C++并没有禁止析构函数引发异常，但是C++不推荐这一做法，从析构函数中抛出异常是及其危险的。

析构函数可能在对象正常结束生命周期时调用，也可能在有异常发生时从函数堆栈清理时调用。前一种情况抛出异常不会有无法预料的结果，可以正常捕获；但后一种情况下，因为函数发生了异常而导致函数的局部变量的析构函数被调用，析构函数又抛出异常，本来局部对象抛出的异常应该是由它所在的函数负责捕获的，现在函数既然已经发生了异常，必定不能捕获，因此，异常处理机制只能调用terminate()。若真的不得不从析构函数抛出异常，应该首先检查当前是否有仍未处理的异常，若没有，才可以正常抛出。 


析构函数中抛出异常时概括性总结

　　（1） C++中析构函数的执行不应该抛出异常；
　　（2） 假如析构函数中抛出了异常，那么你的系统将变得非常危险，也许很长时间什么错误也不会发生；但也许你的系统有时就会莫名奇妙地崩溃而退出了，而且什么迹象也没有，崩得你满地找牙也很难发现问题究竟出现在什么地方；
　　（3） 当在某一个析构函数中会有一些可能（哪怕是一点点可能）发生异常时，那么就必须要把这种可能发生的异常完全封装在析构函数内部，决不能让它抛出函数之外（这招简直是绝杀！呵呵！）；
　　（4） 主人公阿愚吐血地提醒朋友们，一定要切记上面这几条总结，析构函数中抛出异常导致程序不明原因的崩溃是许多系统的致命内伤！
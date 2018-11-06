# <center> raceAfterInterlockedDecrement - CWE 362

竞争条件: 该程序包含可与其他代码同时运行的代码序列，并且代码序列需要对共享资源进行临时独占访问，但存在时间窗口，其中共享资源可由另一同时运行的代码序列修改。

### Type

id = "raceAfterInterlockedDecrement"  
severity = "error"

cwe = "CWE-362: Concurrent Execution using Shared Resource with Improper Synchronization ('Race Condition')"  
cwe-type = "Class"

    <error id="raceAfterInterlockedDecrement" severity="error" msg="Race condition: non-interlocked access after InterlockedDecrement(). Use InterlockedDecrement() return value instead." verbose="Race condition: non-interlocked access after InterlockedDecrement(). Use InterlockedDecrement() return value instead." cwe="362"/>



### Description

The program contains a code sequence that can run concurrently with other code, and the code sequence requires temporary, exclusive access to a shared resource, but a timing window exists in which the shared resource can be modified by another code sequence that is operating concurrently.

This can have security implications when the expected synchronization is in security-critical code, such as recording whether a user is authenticated or modifying important state information that should not be influenced by an outsider.

A race condition occurs within concurrent environments, and is effectively a property of a code sequence. Depending on the context, a code sequence may be in the form of a function call, a small number of instructions, a series of program invocations, etc.

A race condition violates these properties, which are closely related:

- Exclusivity - the code sequence is given exclusive access to the shared resource, i.e., no other code sequence can modify properties of the shared resource before the original sequence has completed execution.
- Atomicity - the code sequence is behaviorally atomic, i.e., no other thread or process can concurrently execute the same sequence of instructions (or a subset) against the same resource.

A race condition exists when an "interfering code sequence" can still access the shared resource, violating exclusivity. Programmers may assume that certain code sequences execute too quickly to be affected by an interfering code sequence; when they are not, this violates atomicity. For example, the single "x++" statement may appear atomic at the code layer, but it is actually non-atomic at the instruction layer, since it involves a read (the original value of x), followed by a computation (x+1), followed by a write (save the result to x).

The interfering code sequence could be "trusted" or "untrusted." A trusted interfering code sequence occurs within the program; it cannot be modified by the attacker, and it can only be invoked indirectly. An untrusted interfering code sequence can be authored directly by the attacker, and typically it is external to the vulnerable program.

More Detail：https://cwe.mitre.org/data/definitions/362.html  



### Example cpp file

The following function attempts to acquire a lock in order to perform operations on a shared resource.

**bad code**
Example Language: C 
	
	void f(pthread_mutex_t *mutex) {
	pthread_mutex_lock(mutex);
	
	/* access shared resource */ 
	
	
	pthread_mutex_unlock(mutex);
	}

However, the code does not check the value returned by pthread_mutex_lock() for errors. If pthread_mutex_lock() cannot acquire the mutex for any reason, the function may introduce a race condition into the program and result in undefined behavior.

In order to avoid data races, correctly written programs must check the result of thread synchronization functions and appropriately handle all errors, either by attempting to recover from them or reporting it to higher levels.

**good code**
 
	int f(pthread_mutex_t *mutex) {
	int result;
	
	result = pthread_mutex_lock(mutex);
	if (0 != result)
	return result;
	
	
	/* access shared resource */ 
	
	
	return pthread_mutex_unlock(mutex);
	}
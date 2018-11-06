# <center> unknownEvaluationOrder - CWE 768

Expression 'x=x++' depends on order of evaluation of side effects

### Type

id = "unknownEvaluationOrder"  
severity = "error"

cwe = "CWE-768: Incorrect Short Circuit Evaluation"  
cwe-type = "Variant"

    <error id="unknownEvaluationOrder" severity="error" msg="Expression &apos;x = x++;&apos; depends on order of evaluation of side effects" verbose="Expression &apos;x = x++;&apos; depends on order of evaluation of side effects" cwe="768"/>



### Description

The software contains a conditional statement with multiple logical expressions in which one of the non-leading expressions may produce side effects. This may lead to an unexpected state in the program after the execution of the conditional, because short-circuiting logic may prevent the side effects from occurring.

Usage of short circuit evaluation, though well-defined in the C standard, may alter control flow in a way that introduces logic errors that are difficult to detect, possibly causing errors later during the software's execution. If an attacker can discover such an inconsistency, it may be exploitable to gain arbitrary control over a system.

If the first condition of an "or" statement is assumed to be true under normal circumstances, or if the first condition of an "and" statement is assumed to be false, then any subsequent conditional may contain its own logic errors that are not detected during code review or testing.

Finally, the usage of short circuit evaluation may decrease the maintainability of the code.

More Detail：  
[https://cwe.mitre.org/data/definitions/768.html](https://cwe.mitre.org/data/definitions/768.html)  
[https://blog.csdn.net/tsroad/article/details/49834261](https://blog.csdn.net/tsroad/article/details/49834261)  
[https://stackoverflow.com/questions/35413391/cppcheck-warning-expression-depends-on-order-of-evaluation-in-x-x-1-3](https://stackoverflow.com/questions/35413391/cppcheck-warning-expression-depends-on-order-of-evaluation-in-x-x-1-3)  


### Example cpp file

**Example 1**

上面的和下面的是等价的

	void main(){

		int x=0;
		x=x++；

		int a = 0;
		int b = a++;
		a = b;
	
	}

	//x:0 -> 1 -> 0

x++先引用x的值，然后自增，也就是x++表达式的值为0，x的值此时变为1，最后将0赋给x又修改了x自增后的值（也就是1）,所以x=x++的实际效果跟x=0一样！（在x++运算中是对x加了1，但返回的数是加之前的，你把加1之前的值又赋值给了x，那自然x又变回了它加1之前的值。）

经测试，不同编译器x的值可能不一样。x可能为1。

在一个序列点之间，连续两次改变，并且访问该变量，会带来问题，比如经典的：
	int i = 1;
	a = i++;
在一个序列点之间，改变了i的值，并且访问了i的值，它的作用是什么呢？是a[1] = 1；还是a[2] = 2呢？不确定，这种代码没有价值，并且老板肯定不会赏识你写出这么精简的代码，你会被开除的。再比如更经典的：
	int i = 1;
	printf("%d, %d, %d\n", i++, i++, i++);
	i = 1;
	printf("%d\n", i++ + i++ + i++);
	i = 1;
	printf("%d\n", ++i + ++i + ++i);

很多大学的C语言老师都会讲解这个问题，其实，这是一个不值得讲解的问题，这是在跟编译器较劲，不同的编译器可能会得出不同的结果（但是平常的编译器可能会得出相同的结果，让程序员私下总结错误的经验。），这种根据不同的实现而得出不同的结果的代码没什么用。i++ + i++ + i++只是一个表达式，在这个表达式的内多次访问了变量i，结果不确定。并且这又会引发另外一个有趣的问题，可能有人会认为在这条语句执行完成以后i自加了3次，那i肯定是4？这也不确定，可能很多编译器做得确实是4，但是，在C标准中有这样一条：当一个表达式的值取决于编译器实现而不是C语言标准的时候，其中所做的任何处理都会不确定。即，如果有一个编译器在i++ + i++ + i++这个表达式中只读取一次i的值，并且一直记住这个值，那么算第一个i++，因为i的值是1所以算出后i的值为2，再算第二个因为假设的是只读取一次i的值，那此时i的值还是1并且被加到2（因为没有经过序列点，所以i的值不能肯定为2），于是经过三次从1加到2的过程以后，最后i的值是2而不是期望的4，呵呵。其实这要看编译器如何实现了，不过既然得看编译器如何实现，那这种代码也得被炒鱿鱼。


**Example 2**

The following function attempts to take a size value from a user and allocate an array of that size (we ignore bounds checking for simplicity). The function tries to initialize each spot with the value of its index, that is, A[len-1] = len - 1; A[len-2] = len - 2; ... A[1] = 1; A[0] = 0; However, since the programmer uses the prefix decrement operator, when the conditional is evaluated with i == 1, the decrement will result in a 0 value for the first part of the predicate, causing the second portion to be bypassed via short-circuit evaluation. This means we cannot be sure of what value will be in A[0] when we return the array to the user.

	\\Example Language: C 
	define PRIV_ADMIN 0
	define PRIV_REGULAR 1
	typedef struct{
		int privileges;
		int id;
	} user_t;
	user_t *Add_Regular_Users(int num_users){
		user_t* users = (user_t*)calloc(num_users, sizeof(user_t));
		int i = num_users;
		while( --i && (users[i].privileges = PRIV_REGULAR) ){
			users[i].id = i;
		}
		return users;
	}
	
	int main(){
		user_t* test;
		int i;
		test = Add_Regular_Users(25);
		for(i = 0; i < 25; i++) 
			printf("user %d has privilege level %d\n", test[i].id, test[i].privileges);
	}

When compiled and run, the above code will output a privilege level of 1, or PRIV_REGULAR for every user but the user with id 0 since the prefix increment operator used in the if statement will reach zero and short circuit before setting the 0th user's privilege level. Since we used calloc, this privilege will be set to 0, or PRIV_ADMIN.


**another Example**：[https://stackoverflow.com/questions/35413391/cppcheck-warning-expression-depends-on-order-of-evaluation-in-x-x-1-3](https://stackoverflow.com/questions/35413391/cppcheck-warning-expression-depends-on-order-of-evaluation-in-x-x-1-3)
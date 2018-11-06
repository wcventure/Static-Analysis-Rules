# <center> wrongPipeParameterSize - CWE 686

PIPE（）的参数必须有2个整数。  
函数原型

	include <unistd.h>
	int pipe(int pipefd[2]);
	define _GNU_SOURCE             /* See feature_test_macros(7) */#include
	<fcntl.h>              /* Obtain O_* constant definitions */#include <unistd.h>
	int pipe2(int pipefd[2], int flags);


### Type

id = "wrongPipeParameterSize"  
severity = "error"

cwe = "CWE-686: Function Call With Incorrect Argument Type"  
cwe-type = "Variant"

    <error id="wrongPipeParameterSize" severity="error" msg="Buffer &apos;varname&apos; must have size of 2 integers if used as parameter of pipe()." verbose="The pipe()/pipe2() system command takes an argument, which is an array of exactly two integers.\012The variable &apos;varname&apos; is an array of size dimension, which does not match." cwe="686"/>



### Description

This check detects errors on POSIX systems, when a pipe command called with a wrong dimensioned file descriptor array. The pipe command requires exactly an integer array of dimension two as parameter.

More Detail：

http://cppcheck.sourceforge.net/devinfo/doxyoutput/checkother_8cpp_source.html  

https://linux.die.net/man/2/pipe


# README

As you said, PMD works on source code and therefore finds problems like: violation of naming conventions, lack of curly braces, misplaced null check, long parameter list, unnecessary constructor, missing break in switch, etc. PMD also tells you about the Cyclomatic complexity of your code which I find very helpful (FindBugs doesn't tell you about the Cyclomatic complexity).

FindBugs works on bytecode. Here are some problems FindBugs finds which PMD doesn't: equals() method fails on subtypes, clone method may return null, reference comparison of Boolean values, impossible cast, 32bit int shifted by an amount not in the range of 0-31, a collection which contains itself, equals method always returns true, an infinite loop, etc.

Usually each of them finds a different set of problems. Use both. These tools taught me a lot about how to write good Java code.

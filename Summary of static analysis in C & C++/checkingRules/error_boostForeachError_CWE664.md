# <center> boostForeachError - CWE 664

BOOST_FOREACH可以方便的遍历STL容器,但不能在scope做一些使迭代器失效的操作,比如说删除之类的

### Type

id = "boostForeachError"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="boostForeachError" severity="error" msg="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." verbose="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  
https://www.cnblogs.com/egmkang/archive/2011/05/14/2046205.html

### Implementation

	static const CWE CWE664(664);

	void CheckBoost::checkBoostForeachModification()
	{
	    const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();
	    const std::size_t functions = symbolDatabase->functionScopes.size();
	    for (std::size_t i = 0; i < functions; ++i) {
	        const Scope * scope = symbolDatabase->functionScopes[i];
	        for (const Token *tok = scope->classStart->next(); tok && tok != scope->classEnd; tok = tok->next()) {
	            if (!Token::simpleMatch(tok, "BOOST_FOREACH ("))
	                continue;
	
	            const Token *containerTok = tok->next()->link()->previous();
	            if (!Token::Match(containerTok, "%var% ) {"))
	                continue;
	
	            const Token *tok2 = containerTok->tokAt(2);
	            const Token *end = tok2->link();
	            for (; tok2 != end; tok2 = tok2->next()) {
	                if (Token::Match(tok2, "%varid% . insert|erase|push_back|push_front|pop_front|pop_back|clear|swap|resize|assign|merge|remove|remove_if|reverse|sort|splice|unique|pop|push", containerTok->varId())) {
	                    const Token* nextStatement = Token::findsimplematch(tok2->linkAt(3), ";", end);
	                    if (!Token::Match(nextStatement, "; break|return|throw"))
	                        boostForeachError(tok2);
	                    break;
	                }
	            }
	        }
	    }
	}
	
	void CheckBoost::boostForeachError(const Token *tok)
	{
	    reportError(tok, Severity::error, "boostForeachError",
	                "BOOST_FOREACH caches the end() iterator. It's undefined behavior if you modify the container inside.", CWE664, false
	               );
	}

### Example cpp file

BOOST_FOREACH可以方便的遍历STL容器,但不能在scope做一些使迭代器失效的操作,比如说删除之类的

	void main(){
	
	  vector<int> vec;
	  vec.push_back(1);
	  vec.push_back(2);
	  vec.push_back(3);
	  vec.push_back(4);
	
	  BOOST_FOREACH(int index, vec) {
		vec.push_back(5);
	  }

}

### Massage output in cppcheck

	[test.cpp:10]: (error) BOOST_FOREACH caches the end() iterator. It's undefined behavior if you modify the container inside.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="boostForeachError" severity="error" msg="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." verbose="BOOST_FOREACH caches the end() iterator. It&apos;s undefined behavior if you modify the container inside." cwe="664">
	            <location file="test.cpp" line="10"/>
	        </error>
	    </errors>
	</results>

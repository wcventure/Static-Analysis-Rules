# <center> unusedPrivateFunction - CWE 398

类中的私有函数没被使用过

### Type

id = "unusedPrivateFunction"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="unusedPrivateFunction" severity="style" msg="Unused private function: &apos;classname::funcname&apos;" verbose="Unused private function: &apos;classname::funcname&apos;" cwe="398"/>



### Description

Unused private function



### Example cpp file

	class DoubleDead {
	private:
		int a;
		void doTweedledee() {
		}
		void doTweedledumb() {
			doTweedledee();
		}

	public:
		DoubleDead(){
			a = 0;
		}
	};
	
	void main()
	{
		DoubleDead A;
	}



### Massage output in cppcheck

	[test\a\a.cpp:6]: (style) Unused private function: 'DoubleDead::doTweedledumb'
	[test\a\a.cpp:6]: (style) The function 'doTweedledumb' is never used.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedPrivateFunction" severity="style" msg="Unused private function: &apos;DoubleDead::doTweedledumb&apos;" verbose="Unused private function: &apos;DoubleDead::doTweedledumb&apos;" cwe="398">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="6"/>
	        </error>
	        <error id="unusedFunction" severity="style" msg="The function &apos;doTweedledumb&apos; is never used." verbose="The function &apos;doTweedledumb&apos; is never used." cwe="561">
	            <location file="test\a\a.cpp" line="6"/>
	        </error>
	    </errors>
	</results>






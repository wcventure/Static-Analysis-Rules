# <center> noConstructor - CWE 398

类没有构造函数

### Type

id = "noConstructor"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="noConstructor" severity="style" msg="The class &apos;classname&apos; does not have a constructor." verbose="The class &apos;classname&apos; does not have a constructor although it has private member variables. Member variables of builtin types are left uninitialized when the class is instantiated. That may cause bugs or undefined behavior." cwe="398"/>



### Description

The class 'DoubleDead' does not have a constructor.



### Example cpp file

	class DoubleDead {
	private:
		int a;
		void doTweedledee() {
			doTweedledumb();
		}
		void doTweedledumb() {
			doTweedledee();
		}
	};
	
	void main()
	{
		DoubleDead A;
	}



### Massage output in cppcheck

	[test\a\a.cpp:14]: (style) Unused variable: A
	[test\a\a.cpp:1]: (style) The class 'DoubleDead' does not have a constructor.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedVariable" severity="style" msg="Unused variable: A" verbose="Unused variable: A" cwe="563">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="14"/>
	        </error>
	        <error id="noConstructor" severity="style" msg="The class &apos;DoubleDead&apos; does not have a constructor." verbose="The class &apos;DoubleDead&apos; does not have a constructor although it has private member variables. Member variables of builtin types are left uninitialized when the class is instantiated. That may cause bugs or undefined behavior." cwe="398">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="1"/>
	        </error>
	    </errors>
	</results>





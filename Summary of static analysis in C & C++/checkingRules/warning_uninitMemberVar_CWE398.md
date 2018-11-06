# <center> uninitMemberVar - CWE 398

类中的成员变量没有在构造函数中被初始化

### Type

id = "uninitMemberVar"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="uninitMemberVar" severity="warning" msg="Member variable &apos;classname::varname&apos; is not initialized in the constructor." verbose="Member variable &apos;classname::varname&apos; is not initialized in the constructor." cwe="398"/>



### Description

Member variables are not initialized in the constructor.



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
	public:
		DoubleDead(){}
	};
	
	void main()
	{
		DoubleDead A;
	}



### Massage output in cppcheck

	[test\a\a.cpp:11]: (warning) Member variable 'DoubleDead::a' is not initialized in the constructor.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uninitMemberVar" severity="warning" msg="Member variable &apos;DoubleDead::a&apos; is not initialized in the constructor." verbose="Member variable &apos;DoubleDead::a&apos; is not initialized in the constructor." cwe="398">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="11"/>
	        </error>
	    </errors>
	</results>






# <center> virtualDestructor - CWE 404

如果在没有virtual destructor的情况下，delete一个子类只会调用父类的析构函数，而不会调用子类的析构函数，而有了virtual以后，就不会出现这个问题了。

### Type

id = "virtualDestructor"  
severity = "error"

cwe = "CWE-404: Improper Resource Shutdown or Release"  
cwe-type = "Base"

    <error id="virtualDestructor" severity="error" msg="Class &apos;Base&apos; which is inherited by class &apos;Derived&apos; does not have a virtual destructor." verbose="Class &apos;Base&apos; which is inherited by class &apos;Derived&apos; does not have a virtual destructor. If you destroy instances of the derived class by deleting a pointer that points to the base class, only the destructor of the base class is executed. Thus, dynamic memory that is managed by the derived class could leak. This can be avoided by adding a virtual destructor to the base class." cwe="404"/>



### Description

我们知道，用C++开发的时候，用来做基类的类的析构函数一般都是虚函数。这样做是为了当用一个基类的指针删除一个派生类的对象时，派生类的析构函数会被调用。当然，并不是要把所有类的析构函数都写成虚函数。因为当类里面有虚函数的时候，编译器会给类添加一个虚函数表，里面来存放虚函数指针，这样就会增加类的存储空间。所以，只有当一个类被用来作为基类的时候，才把析构函数写成虚函数。

The program does not release or incorrectly releases a resource before it is made available for re-use.

When a resource is created or allocated, the developer is responsible for properly releasing the resource as well as accounting for all potential paths of expiration or invalidation, such as a set period of time or revocation.

More Detail：https://cwe.mitre.org/data/definitions/562.html  
https://blog.csdn.net/aore2010/article/details/5881332  



### Example cpp file

In C++, what’s a virtual destructor and when is it needed?

As you may know, in C++ a destructor is generally used to deallocate memory and do some other cleanup for a class object and it’s class members whenever an object is destroyed. Destructors are distinguished by the tilde, the ‘~’ that appears in front of the destructor name. In order to define a virtual destructor, all you have to do is simply add the keyword “virtual” before the tilde symbol.

The need for virtual destructors in C++ is best illustrated by some examples. Let’s start by going through an example that does not use virtual destructors, and then we will go through an example that does use virtual destructors. Once you see the difference, you will understand why virtual destructors are needed. Take a look at the code below to start out:

Example without a Virtual Destructor:

	include iostream.h
	class Base
	{
	   	public:
	      	Base(){ cout<<"Constructing Base";}	      	
	     // this is a destructor:		
		~Base(){ cout<<"Destroying Base";}
	};
	
	class Derive: public Base
	{
	        public:
	       	Derive(){ cout<<"Constructing Derive";}	       	
	       	~Derive(){ cout<<"Destroying Derive";}
	 };
	
	void main()
	{
	    	Base *basePtr = new Derive();	        
	        delete basePtr;
	}

The output after running the code above would be:

	Constructing Base  
	Constructing Derive 
	Destroying Base

Based on the output above, we can see that the constructors get called in the appropriate order when we create the Derive class object pointer in the main function.

But there is a major problem with the code above: the destructor for the "Derive" class does not get called at all when we delete ‘basePtr’.

So, how can we fix this problem?

Well, what we can do is make the base class destructor virtual, and that will ensure that the destructor for any class that derives from Base (in our case, its the "Derive" class) will be called.

Example with a Virtual Destructor:

	class Base
	{
	   	public:
	      	Base(){ cout<<"Constructing Base";}
	
		// this is a virtual destructor:
		virtual ~Base(){ cout<<"Destroying Base";}
	};

Now, with that change, the output after running the code above will be:

	Constructing Base  
	Constructing Derive 
	Destroying Derive
	Destroying Base

Note that the derived class destructor will be called before the base class.

So, now you’ve seen why we need virtual destructors and also how they work.

One important design paradigm of class design is that if a class has one or more virtual functions, then that class should also have a virtual destructor.

### Massage output in cppcheck

	[test.cpp:8]: (error) Class 'Base' which is inherited by class 'Derive' does not have a virtual destructor.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="virtualDestructor" severity="error" msg="Class &apos;Base&apos; which is inherited by class &apos;Derive&apos; does not have a virtual destructor." verbose="Class &apos;Base&apos; which is inherited by class &apos;Derive&apos; does not have a virtual destructor. If you destroy instances of the derived class by deleting a pointer that points to the base class, only the destructor of the base class is executed. Thus, dynamic memory that is managed by the derived class could leak. This can be avoided by adding a virtual destructor to the base class." cwe="404">
	            <location file="test.cpp" line="8"/>
	        </error>
	    </errors>
	</results>
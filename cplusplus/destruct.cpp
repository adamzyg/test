#include <iostream>
using namespace std;

class Base
{
public:
	virtual Base* fun(int)
	{
		cout<<"1111111111111"<<endl;
		return this;
	}
	Base()
	{
		cout<<"Base::Base()"<<endl;
	}
	virtual ~Base()  ///
	{
		cout<<"Base::~Base()"<<endl;
	}
};

class D : public Base
{
public:
	virtual D* fun(int)
	{
		cout<<"22222222222"<<endl;
		return this;
	}
	D()
	{
		cout<<"D::D()"<<endl;
	}
	~D()
	{
		cout<<"D::~D()"<<endl;
	}
};

int main()
{
	//D d;
	Base *pd = new D;
	pd->fun(1);
	delete pd;
    return 0;
}

/*
 *
zyg@linux-OptiPlex-3046:~/test/cplusplus$ g++ destruct.cpp -o destruct  -std=c++0x -fno-elide-constructors
zyg@linux-OptiPlex-3046:~/test/cplusplus$ ./destruct
Base::Base()
D::D()
22222222222
D::~D()
Base::~Base()
 */

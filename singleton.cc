#include <iostream>
#include "singleton.h"

void test1(void)
{
	rlc::singleton<int> s(new int(0));
	
	std::cout << s << " " << *s << std::endl;
	*s = 1;
	std::cout << s << " " << *s << std::endl;
}

void test2(void)
{
	rlc::singleton<int> s1(new int(0)); // these two should point to the same thing
	rlc::singleton<int> s2;

	std::cout << s1 << " " << *s1 << std::endl;
	std::cout << s2 << " " << *s2 << std::endl;
	*s1 = 1;
	std::cout << s1 << " " << *s1 << std::endl;
	std::cout << s2 << " " << *s2 << std::endl;
}

void test3(void)
{
	bool caught = false;
	rlc::singleton<int> s1(new int(0)); // these two should point to the same thing
	try
	{
		rlc::singleton<int> s2(new int(1));
	}
	catch (...)
	{
		caught = true;
	}
	assert(caught);
}

int main(void)
{
	test1();
	test2();
	test3();

	return 0;
}


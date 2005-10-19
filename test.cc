#include <vector>
#include <algorithm>
#include <iostream>
#include "dag.hpp"

void test1(void)
{
	rlc::dag<int> dag;
	std::vector<int> v;
	
	for (int i = 0; i < 100; ++i)
	{
		dag.insert(i);
		v.push_back(i);
	}
	for (int c = 0; c < 5; c++)
	{
		std::random_shuffle(v.begin(), v.end());
		for (std::vector<int>::const_iterator i = v.begin(); i != v.end(); ++i)
		{
			try
			{
				std::cerr << "Setting a link between " << *i << " and " << *(i + 1) << ": ";
				dag.link(*i, *(i + 1));
				std::cerr << "OK" << std::endl;
			}
			catch (const rlc::dag<int>::circular_reference_exception &)
			{
				// ignore circular references in this test
				std::cerr << "Failed" << std::endl;
			}
		}
	}
	
	std::copy(dag.begin(), dag.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
}

void test2(void)
{
	rlc::dag<int> dag;
	std::vector<int> v;
	
	for (int i = 0; i < 10; i++)
		dag.insert(i);

	for (int i = 1; i < 4; ++i)
		dag.link(i, i + 1);
	
	try
	{
		dag.link(4, 2);
	}
	catch (const rlc::dag<int>::circular_reference_exception &)
	{
		std::cerr << "Circular reference detected" << std::endl;
	}
	catch(...)
	{
		std::cerr << "Something bad happened" << std::endl;
	}
	
	std::copy(dag.begin(), dag.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
}


int main(void)
{
	test1();
	test2();
}


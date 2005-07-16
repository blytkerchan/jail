#include <vector>
#include <algorithm>
#include <iostream>
#include "dag.hpp"

void test1(void)
{
	rlc::dag<int> dag;
	std::vector<int> v;
	
	for (int i = 0; i < 1000; ++i)
	{
		dag.insert(i);
		v.push_back(i);
	}
	for (int c = 0; c < 50; c++)
	{
		std::random_shuffle(v.begin(), v.end());
		for (std::vector<int>::const_iterator i = v.begin(); i != v.end(); ++i)
		{
			try
			{
//				std::cerr << "Setting a link between " << *i << " and " << *(i + 1) << std::endl;
				dag.link(*i, *(i + 1));
			}
			catch (const rlc::dag<int>::circular_reference_exception &)
			{
				// ignore circular references in this test
//				std::cerr << "Failed" << std::endl;
			}
		}
	}
	
	std::copy(dag.begin(), dag.end(), std::ostream_iterator<int>(std::cout, " "));
}

int main(void)
{
	test1();
}


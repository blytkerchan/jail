void * rpl_malloc(size_t size)
{
	if (size == 0)
		return malloc(1);
	return malloc(size);
}

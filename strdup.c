char * strdup(const char * s)
{
	char * retval;
	size_t size;

	size = strlen(s);
	retval = malloc(size + 1);
	memcpy(retval, s, size);
	retval[size] = 0;

	return retval;
}


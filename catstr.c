char * catstr(char * str1, char * str2)
{
	char * retval;

	if (str1 != NULL)
	{
		retval = (char*)realloc(str1, strlen(str1) + strlen(str2) + 1);
		strcat(retval, str2);
	}
	else
		retval = strdup(str2);
	
	return retval;
}


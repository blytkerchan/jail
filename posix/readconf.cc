#include <fstream>
#include "readconf.h"

// forward declaration - definition found in generated parser confp.c
extern "C" int yyparse(libconf_t * handle);

static int libconf_readconf_worker(libconf_t * handle)
{
	try
	{
		if (handle->yyin != NULL)
			delete (std::istream*)handle->yyin;
		handle->yyin = new std::fstream(handle->filename);
		yyparse(handle);
		delete (std::istream*)handle->yyin;
		handle->yyin = NULL;
	}
	catch (...)
	{
		return -1;
	}

	return 0;
}

extern "C" int libconf_readconf_global(libconf_t * handle)
{
	handle->filename = handle->global_config_filename;

	return libconf_readconf_worker(handle);
}

extern "C" int libconf_readconf_local(libconf_t * handle)
{
	handle->filename = handle->local_config_filename;
	
	return libconf_readconf_worker(handle);
}

#include "../libconf.h"

void test1(void)
{
	static char * argv[] = 
	{
		"./test1",
		"-DHELLO=\"world\"",
		NULL
	};
	static libconf_opt_t options[] = {
		{ "define", 'D', TP_YES, PT_STRING_LIST, "define", TP_YES, PT_STRING_LIST, "define a macro", DOE_WARNING },
		{ NULL, 0, TP_NO, PT_NONE, NULL, TP_NO, PT_NONE, NULL, DOE_ERROR }
	};
	int argc = 2;
	libconf_t * handle = NULL;
	
	handle = libconf_init("global_conf_test1", NULL, libconf_optconst(options), NULL, argc, argv);
	libconf_fini(handle);
}

int main(void)
{
	test1();
}

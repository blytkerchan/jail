map_t * map_new(map_key_cmp_func_t cmp_func)
{
	map_t * retval;

	retval = (map_t*)calloc(1, sizeof(map_t));
	retval->cmp_func = cmp_func;
	retval->tree = binomial_tree_new();
}

void map_free(map_t * handle)
{
	binomial_tree_free(handle->tree);
	free(handle);
}

void map_insert(map_t * handle, void * key, void * val)
{
	
}

void * map_get(map_t * handle, void * key);
void map_remove(map_t * handle, void * key);

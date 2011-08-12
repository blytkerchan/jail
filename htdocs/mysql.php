<?php 
function site_opendb()
{
	global $db_cfg;
	
	$db_cfg['link'] = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
	if (!$db_cfg['link'])
		return FALSE;
	if (!mysql_select_db($db_cfg['dbname']))
		return FALSE;
		
	return TRUE;
}

function site_closedb()
{
	global $db_cfg;

	mysql_close($db_cfg['link']);
}
?>

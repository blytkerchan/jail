<?php 
function check_session($session)
{
	global $db_cfg;
	
	$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
	if (!$link)
		return FALSE;
	if (!mysql_select_db($db_cfg['dbname']))
		return FALSE;
	$session = mysql_real_escape_string($session); // safety measure
	$query = "SELECT * FROM `users` WHERE uid='$session' LIMIT 1";
	if (!($result = mysql_query($query)))
	{
		mysql_close($link);
		return FALSE;
	}
	$line = mysql_fetch_assoc($result);
	mysql_free_result($result);
	mysql_close($link);

	return $line;
}

function close_session()
{
	global $remembered;
	global $session;
	
	setcookie("session", "", $remembered);
	if ($remembered)
		setcookie("remembered", "", $remembered);
	$session = 0;
}

function open_session($name, $passwd)
{
	global $db_cfg;

	$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
	if (!$link)
		return 0;
	if (!mysql_select_db($db_cfg['dbname']))
		return 0;
	$name = mysql_real_escape_string($name);
	$passwd = mysql_real_escape_string($passwd);
	$query = "SELECT * FROM `users` WHERE `name` LIKE '$name' AND `passwd` LIKE PASSWORD('$passwd')";
	if (!($result = mysql_query($query)))
	{
		mysql_close($link);
		return 0;
	}
	$line = mysql_fetch_assoc($result);
	mysql_free_result($result);
	mysql_close($link);
	setcookie("session", $line['uid']);

	return $line['uid'];
}

function create_session($name, $passwd)
{
	global $db_cfg;

	$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
	if (!$link)
		return 0;
	if (!mysql_select_db($db_cfg['dbname']))
		return 0;
	$name = mysql_real_escape_string($name);
	$passwd = mysql_real_escape_string($passwd);
	$query = "INSERT INTO `users` (`name`, `passwd`) VALUES ('$name', PASSWORD('$passwd'))";
	mysql_query($query);
	mysql_close($link);

	return open_session($name, $passwd);
}
$session = $_COOKIE['session'];
if ($remembered = $_COOKIE['remembered'])
{
	$forget = time() + 60 * 60 * 24 * 30;
	setcookie("session", $session, $forget);
	setcookie("remembered", $forget, $forget);
}

if (strcmp($_GET["login"], "do") == 0)
{
	if ($_POST["name"] && $_POST["passwd"])
	{
		$session = open_session($_POST["name"], $_POST["passwd"]);
		if (!$session)
			$session = create_session($_POST["name"], $_POST["passwd"]);
	}
}
else if (strcmp($_GET["login"], "remember") == 0)
{
	$remembered = $forget = time() + 60 * 60 * 24 * 30;
	setcookie("session", $session, $forget);
	setcookie("remembered", $forget, $forget);
}
else if ($_GET['logout'])
{
	close_session($session);
}

?>

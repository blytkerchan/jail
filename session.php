<?php 
function check_session($session)
{
	$retval = 0;
	
	$query = "SELECT * FROM `sessions` WHERE `uid`='".mysql_escape_string($session)."' LIMIT 1";
	$result = mysql_query($query);
	$line = mysql_fetch_assoc($result);
	mysql_free_result($result);

	if ((strcmp($line['IP'], $_SERVER['REMOTE_ADDR']) == 0) 
	&& (time() < $line['expire'])
	&& (strcmp($line['browser'], $_SERVER['HTTP_USER_AGENT']) == 0))
		$retval = $line['user'];
	update_account_expire($retval);

	return $retval;
}

function close_session($session)
{
	global $session;
	global $user;

	$remembered = $_COOKIE['remembered'];
	setcookie("session", "", $remembered);
	$session = 0;
	$user = 0;
	if ($remembered)
		setcookie("remembered", "", $remembered);
}

function create_session($name, $passwd)
{
	$query = "SELECT * FROM `users` WHERE `name`='" . mysql_escape_string($name) . "' AND `passwd`=PASSWORD('" . mysql_escape_string($passwd) . "') LIMIT 1";
	$result = mysql_query($query);
	$line = mysql_fetch_assoc($result);
	mysql_free_result($result);
	if (!$line['uid'])
		return 0;
	// we now have a valid user entry..
	$session = rand();
	$expire = time() + 300;
	$query = "INSERT INTO `sessions` (`uid`, `user`, `IP`, `browser`, `expire`) VALUES ('" . $session . "', '" . $line['uid'] . "', '" . $_SERVER['REMOTE_ADDR'] . "', '" . $_SERVER['HTTP_USER_AGENT'] . "', '" . $expire . "')";
	mysql_query($query);
	setcookie("session", $session);

	return $session;
}

function update_session_expire($session, $expire)
{
	setcookie("session", $session, $expire);

	$query = "UPDATE `sessions` SET `expire`='$expire' WHERE `uid`='$session' LIMIT 1";
	mysql_query($query);
}

function do_session()
{
	global $user;
	global $session;
	global $login_status;
	global $remembered;

	$session = $_COOKIE['session'];
	if ($remembered = $_COOKIE['remembered'])
	{
		$forget = time() + 60 * 60 * 24 * 30;
		setcookie("session", $session, $forget);
		setcookie("remembered", $forget, $forget);
	}
	$user = check_session($session);
	
	if (strcmp($_GET["login"], "do") == 0)
	{
		if ($_POST["name"] && $_POST["passwd"])
		{
			$session = create_session($_POST["name"], $_POST["passwd"]);
			$user = check_session($session);
		}
		if (!$user || !$session)
			$login_status = 1;
	}
	else if (strcmp($_GET["login"], "remember") == 0)
	{
		$remembered = $forget = time() + 60 * 60 * 24 * 30;
		update_session_expire($session, $forget);
		setcookie("remembered", $forget, $forget);
	}
	else if (strcmp($_GET['login'], "send") == 0)
	{
		if ($login_info = get_login_by_email($_POST['email']))
		{
			$letters = array('a', 'b', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');
			$password = "";
			for ($i = 0; $i < rand(8, 10); $i++)
				$password .= $letters[rand(0, count($letters) - 1)];
			update_login_password($login_info['uid'], $password);
			if (mail($login_info['name'] . "<" . $login_info['email'] . ">", 
						"Login credentials for http://jail-ust.sf.net", 
						"As per your request, these are your login credentials for the site of the Jail-ust project, at http://jail-ust.sf.net\n\nUsername: " . $login_info['name'] . "\nPassword: " . $password . "\n\nPlease note that your password has been reset for this query: it is not available in a readible form to anyone in the site's database.\n\nHave a nice day :)")
			)
				$login_status = 3;
			else
				$login_status = 4;
		}
		else
			$login_status = 2;
		
	}
	else if ($_GET['logout'])
	{
		close_session($session);
	}
}

?>

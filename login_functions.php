<?php
	function get_login_by_email($email)
	{
		$query = "SELECT * FROM `users` WHERE `email`='" . mysql_escape_string($email) . "' LIMIT 1";
		$result = mysql_query($query);
		$line = mysql_fetch_assoc($result);
		mysql_free_result($result);
		
		return $line;
	}

	function update_login_password($user, $passwd)
	{
		$query = "UPDATE `users` SET `passwd`=PASSWORD('$passwd') WHERE `uid`='$user'";
echo($query);
		mysql_query($query);
	}

	function create_account($name, $password, $email)
	{
		$expire = time() + 386 * 24 * 60 * 60;
		$query = "INSERT INTO `users` (`name`, `email`, `passwd`, `expire`) VALUES ('" . mysql_escape_string($name) . "', '" . mysql_escape_string($email) . "', PASSWORD('" . mysql_escape_string($password) . "'), '$expire')";

		if (!mysql_query($query))
			return FALSE;
		else if (!mysql_affected_rows())
			return FALSE;
		return TRUE;
	}

	function update_account_expire($uid)
	{
		$expire = time() + 386 * 24 * 60 * 60;
		$query = "UPDATE `users` SET `expire`='" . $expire . "' WHERE `uid`='$uid'";
		mysql_query($query);
	}

	function get_login_name($id)
	{
		$query = "SELECT * FROM `users` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		$line = mysql_fetch_assoc($result);

		return $line['name'];
	}
?>

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

	function edit_login($id, $section, $page)
	{
		$query = "SELECT * FROM `users` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		$line = mysql_fetch_assoc($result);

		?>
		<h1>Edit your Credentials</h1>
		<p>You can use the form below to edit your login credentials and preferences.<br/>
		Please note that you must enter the current password for any change to be taken into account!</p>
		<form method="post" action="index.php?section=<?php echo($section); ?>&page=<?php echo($page); ?>&login=save">
		<table>
		<tbody>
		<tr><td>Name</td>
		<td><input type="text" name="name" value="<?php echo($line['name']); ?>" size="20" /></td></tr>
		<tr><td>E-mail address</td>
		<td><input type="text" name="email" value="<?php echo($line['email']); ?>" size="20" /></td></tr>
		<tr><td>Current Password</td>
		<td><input type="password" name="passwd_o" size="20" /></td></tr>
		<tr><td>New Password</td>
		<td><input type="password" name="passwd_n1" size="20" /></td></tr>
		<tr><td>New Password (again)</td>
		<td><input type="password" name="passwd_n2" size="20" /></td></tr>
		</tbody>
		</table>
		<input type="submit" class="inputbox" />
		</form>
		<?php
	}

	function save_login($id, $name, $email, $o_passwd, $n_passwd1, $n_passwd2)
	{
		$query = "UPDATE `users` SET ";
		if ($name)
		{
			$query = $query  ."`name`='" . mysql_escape_string($name) . "'";
			$do_comma = 1;
		}
		if ($email)
		{
			if ($do_comma)
				$query = $query . ", ";
			$query = $query . "`email`='" . mysql_escape_string($email) . "'";
			$do_comma = 1;
		}
		if ($n_passwd1 && strcmp($n_passwd1, $n_passwd2) == 0)
		{
			if ($do_comma)
				$query = $query . ", ";
			$query = $query . "`passwd`=PASSWORD('" . mysql_escape_string($n_passwd1) . "')";
			$do_passwd = 1;
		}
		
		$query = $query . " WHERE `uid`='$id' AND `passwd`=PASSWORD('" . mysql_escape_string($o_passwd) . "') LIMIT 1";
		if (mysql_query($query) && mysql_affected_rows())
		{
			?>
			<h1>Login Information Updated</h1>
			<p>Your login credentials have been updated. Please make sure you don't forget the values you entered!</p>
			<?php
		}
		else
		{
			?>
			<h1>Error While Updating Credentials</h1>
			<p>The information you entered could not be taken into account. There are a number of possible reasons for this, among which:
			<ul>
			<li>If you changed your login name, another account with the new name may already exist</li>
			<li>If you changed your password, the two passwords you enter must be identical - this may not have been the case</li>
			<li>The password you enter in the Current Password box must correspond to the current password for any changed to be taken into account - even if the password is not changed</li>
			</ul>
			Please feel free to try again!
			</p>
			<?php
		}
	}
?>

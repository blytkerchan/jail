<?php
	function add_comment($user, $section, $page, $title, $comment)
	{
		global $sections;
		global $section_pages;

		$query = "INSERT INTO `comments` (`user`, `section`, `page`, `title`, `comment`, `date`) VALUES ('$user', '" . $sections[$section] . "', '" . $section_pages[$section][$page]['filename'] . "', '" . mysql_escape_string($title) . "', '" . mysql_escape_string($comment) . "', NOW())";
		mysql_query($query);
	}

	function edit_comment($user, $id, $title, $comment)
	{
		global $sections;
		global $section_pages;

		$query = "UPDATE `comments` SET `date`=NOW(), `comment`='" . mysql_escape_string($comment) . "', `title`='" . mysql_escape_string($title) . "' WHERE uid='" . mysql_escape_string($id) . "' AND `user`='$user'";
		mysql_query($query);
	}

	function del_comment($user, $uid)
	{
		$query = "DELETE FROM `comments` WHERE `uid`='" . mysql_escape_string($uid) . "' AND `user`='" . mysql_escape_string($user) . "' LIMIT 1";
		mysql_query($query);
	}
	
	function get_comments($user, $section, $page)
	{
		global $sections;
		global $section_pages;

		$query = "SELECT * FROM `comments` WHERE `section`='" . $sections[$section] . "' AND `page`='" . $section_pages[$section][$page]['filename'] . "'";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);

		return $retval;
	}

	function show_comments($user, $section, $page)
	{
		if (strcmp($_GET['comment'], "do") == 0)
		{
			if (strlen($_POST['title']) && strlen($_POST['comment']))
				add_comment($user, $section, $page, $_POST['title'], $_POST['comment']);
		}
		else if (strcmp($_GET['comment'], "display") != 0)
		{
			del_comment($user, $_GET['comment']);
		}
	
		$comments = get_comments($user, $section, $page);
		if ($comments)
		{
			echo("<h2>Comments</h2><p>");
			echo("<table width=\"100%\"><tbody>");
			foreach ($comments as $line)
			{
				?>
					<tr>
					<?php if ($line['user'] == $user) { ?>
					<td>
					<?php } else { ?>
					<td colspan="2">
					<?php } ?>
					<a href="index.php?comment=display&id=<?php echo($line['uid']); ?>&section=<?php echo($section) ?>&page=<?php echo($page); ?>">
					<?php echo($line['title']); ?>
					</a>
					</td>
					<?php if ($line['user'] == $user) { ?>
					<td align="right">
						<a href="index.php?comment=edit1&id=<?php echo($line['uid']); ?>&section=<?php echo($section) ?>&page=<?php echo($page); ?>">
							<img src="images/edit.png" border="0"/>
						</a>
						<a href="index.php?comment=<?php echo($line['uid']); ?>&section=<?php echo($section) ?>&page=<?php echo($page); ?>">
							<img src="images/trash.png" border="0"/>
						</a>
					</td>
					<?php } ?>
					</tr>
				<?php
			}
			echo("</tbody></table>");
		}
		else if ($user)
		{
			echo("<h2>Comments</h2><p>");
			echo("No comments on this page<br/>");
		}
		if ($user)
			echo("<a href=\"index.php?section=$section&page=$page&comment=create\">Create a comment</a><br/>");
	}

	function create_comment($user, $section, $page)
	{
		?>
		<h1>Create a Comment</h1>
		<p>You can create a comment by typing the title and the text of 
		your comment below. HTML tags will be displayed as text, newlines
		will be formatted as such; an empty line starts a new paragraph. 
		Backslashes will be removed from the text.
		No other formatting is (currently) possible and neither are links.</p>
		<form action="index.php?comment=do&section=<?php echo($section); ?>&page=<?php echo($page); ?>" method="post">
		<table width="100%">
		<tbody>
		<tr><td>Title</td><td align="right"><input name="title" type="text" size="50" /></td></tr>
		<tr><td colspan="2"><textarea name="comment" style="width:100%; height: 200px" cols="45" rows="5"></textarea></td></tr>
		<tr><td colspan="2"><input type="submit" value="submit" /></td></tr>
		</tbody>
		</table>
		</form>
		<?php
	}

	function get_comment_title($id)
	{
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);

		return $retval[0]['title'];
	}
	
	function get_comment_text($id)
	{
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);

		return $retval[0]['comment'];
	}

	function get_comment_date($id)
	{
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return "unknown date";
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);

		return $retval[0]['date'];
	}

	function get_comment_author($id)
	{
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return "unknown author";
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);
		
		$query = "SELECT * FROM `users` WHERE `uid`='" . $retval[0]['user'] . "' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return "unknown author";
		$line = mysql_fetch_assoc($result);
		$retval = $line;
		mysql_free_result($result);

		return $retval['name'];
	}

	function edit_comment1($user, $id, $section, $page)
	{
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return FALSE;
		$line = mysql_fetch_assoc($result);
		mysql_free_result($result);
		
		?>
		<h1>Edit a Comment</h1>
		<p>Below, you will find the text you've entered when creating the
		comment. You can now edit it to your heart's content.</p>
		<p>Remember, though, that HTML tags will (again) be displayed as text,
		newlines will be formatted as such; an empty line starts a new paragraph.
		Backslashes will be removed from the text entirely.</p>
		<form action="index.php?comment=edit2&section=<?php echo($section); ?>&page=<?php echo($page); ?>" method="post">
		<input type="hidden" name="id" value="<?php echo($line['uid']); ?>"/>
		<table width="100%">
		<tbody>
		<tr><td>Title</td><td align="right"><input name="title" type="text" size="50" value="<?php echo($line['title']); ?>"/></td></tr>
		<tr><td colspan="2"><textarea name="comment" style="width:100%; height: 200px" cols="45" rows="5"><?php echo(str_replace("\\", "", $line['comment'])); ?></textarea></td></tr>
		<tr><td colspan="2"><input type="submit" value="submit" /></td></tr>
		</tbody>
		</table>
		</form>
		<?php
	}
?>

<?php
	function add_comment($session, $section, $page, $title, $comment)
	{
		global $db_cfg;
		global $sections;
		global $section_pages;

		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return;
		if (!mysql_select_db($db_cfg['dbname']))
			return;
		$query = "INSERT INTO `comments` (`user`, `section`, `page`, `title`, `comment`) VALUES ('$session', '" . $sections[$section] . "', '" . $section_pages[$section][$page]['filename'] . "', '" . mysql_real_escape_string($title) . "', '" . mysql_real_escape_string($comment) . "')";
		mysql_query($query);
		mysql_close($link);
	}

	function del_comment($session, $uid)
	{
		global $db_cfg;

		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return;
		if (!mysql_select_db($db_cfg['dbname']))
			return;
		$query = "DELETE FROM `comments` WHERE `uid`='" . mysql_real_escape_string($uid) . "' AND `user`='" . mysql_real_escape_string($session) . "' LIMIT 1";
		mysql_query($query);
		mysql_close($link);
	}
	
	function get_comments($session, $section, $page)
	{
		global $db_cfg;
		global $sections;
		global $section_pages;

		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "SELECT * FROM `comments` WHERE `section`='" . $sections[$section] . "' AND `page`='" . $section_pages[$section][$page]['filename'] . "'";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);
		mysql_close($link);

		return $retval;
	}

	function show_comments($session, $section, $page)
	{
		if (strcmp($_GET['comment'], "do") == 0)
		{
			if (strlen($_POST['title']) && strlen($_POST['comment']))
				add_comment($session, $section, $page, $_POST['title'], $_POST['comment']);
		}
		else if (strcmp($_GET['comment'], "display") != 0)
		{
			del_comment($session, $_GET['comment']);
		}
	
		if (check_session($session))
		{
			echo("<h2>Comments</h2><p>");
			$comments = get_comments($session, $section, $page);
			if ($comments)
			{
				echo("<table width=\"100%\"><tbody>");
				foreach ($comments as $line)
				{
					?>
						<tr>
						<?php if ($line['user'] == $session) { ?>
						<td>
						<?php } else { ?>
						<td colspan="2">
						<?php } ?>
						<a href="index.php?comment=display&id=<?php echo($line['uid']); ?>&section=<?php echo($section) ?>&page=<?php echo($page); ?>">
						<?php echo($line['title']); ?>
						</a>
						</td>
						<?php if ($line['user'] == $session) { ?>
						<td align="right">
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
			else
				echo("No comments on this page<br/>");
			echo("<a href=\"index.php?section=$section&page=$page&comment=create\">Create a comment</a><br/>");
		}
	}

	function create_comment($session, $section, $page)
	{
		?>
		<h1>Create a Comment</h1>
		<p>You can create a comment by typing the title and the text of 
		your comment below. HTML tags will be displayed as text, newlines
		will be formatted as such; an empty line starts a new paragraph. 
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
		global $db_cfg;
	
		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);
		mysql_close($link);

		return $retval[0]['title'];
	}
	
	function get_comment_text($id)
	{
		global $db_cfg;
	
		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "SELECT * FROM `comments` WHERE `uid`='$id' LIMIT 1";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);
		mysql_close($link);

		return $retval[0]['comment'];
	}
?>

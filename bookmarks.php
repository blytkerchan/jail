<?php
	function set_bookmark($session, $section, $page, $label)
	{
		global $db_cfg;
		global $sections;
		global $section_pages;

		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "INSERT INTO `bookmarks` (`user`, `section`, `page`, `label`) VALUES ('$session', '" . $sections[$section] . "', '" . $section_pages[$section][$page]['filename'] . "', '" . mysql_real_escape_string($label) . "')";
		mysql_query($query);
		mysql_close($link);
	}
	function del_bookmark($session, $uid)
	{
		global $db_cfg;

		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link)
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "DELETE FROM `bookmarks` WHERE `uid`='" . mysql_real_escape_string($uid) . "' AND `user`='" . mysql_real_escape_string($session) . "' LIMIT 1";
		mysql_query($query);
		mysql_close($link);
	}
	
	function get_bookmarks($session)
	{
		global $db_cfg;
		
		$link = mysql_connect($db_cfg['host'], $db_cfg['user'], $db_cfg['passwd']);
		if (!$link) 
			return 0;
		if (!mysql_select_db($db_cfg['dbname']))
			return 0;
		$query = "SELECT * FROM `bookmarks` WHERE `user`='$session'";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);
		mysql_close($link);

		return $retval;
	}
	
	if (strcmp($_GET['bookmark'], "do") == 0)
	{
		if ($_POST['label'])
			set_bookmark($session, $section, $page, $_POST['label']);
		else
			set_bookmark($session, $section, $page, $sections[$section] . "/" . $section_pages[$section][$page]["name"]);
	}
	else if ($_GET['bookmark'])
	{
		del_bookmark($session, $_GET['bookmark']);
	}
	
	if (check_session($session))
	{
		echo("<h2>Bookmarks</h2><p>");
		$bookmarks = get_bookmarks($session);
		if ($bookmarks)
		{
			echo("<table width=\"100%\"><tbody>");
			foreach ($bookmarks as $line)
			{
				echo("<tr><td>");
				link_to_page($line['section'], $line['page'], $line['label']);
				echo("</td><td align=\"right\"><a href=\"index.php?section=$section&page=$page&bookmark=".$line['uid']."\"><img src=\"images/trash.png\" border=\"0\"/></a></td></tr>");
			}
			echo("</tbody></table>");
		}
		else
		{
			echo("no bookmarks currently stored<br/>");
		}
		?>
		<form action="index.php?section=<?php echo($section); ?>&page=<?php echo($page); ?>&bookmark=do" method="post">
		Bookmark this page as:<br/>
		<div align="right"><input type="text" size="20" name="label"/></div>
		<input type="submit" value="Bookmark!"/>
		</form>
		<?php
		echo("</p>");
	}
?>

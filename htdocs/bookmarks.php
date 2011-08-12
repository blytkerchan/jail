<?php
	function set_bookmark($user, $section, $page, $label)
	{
		global $sections;
		global $section_pages;

		$query = "INSERT INTO `bookmarks` (`user`, `section`, `page`, `label`) VALUES ('$user', '" . $sections[$section] . "', '" . $section_pages[$section][$page]['filename'] . "', '" . mysql_escape_string($label) . "')";
		mysql_query($query);
	}
	function del_bookmark($user, $uid)
	{
		$query = "DELETE FROM `bookmarks` WHERE `uid`='" . mysql_escape_string($uid) . "' AND `user`='" . mysql_escape_string($user) . "' LIMIT 1";
		mysql_query($query);
	}
	
	function get_bookmarks($user)
	{
		$query = "SELECT * FROM `bookmarks` WHERE `user`='$user'";
		$result = mysql_query($query);
		if (!$result)
			return 0;
		while ($line = mysql_fetch_assoc($result))
			$retval[] = $line;
		mysql_free_result($result);

		return $retval;
	}
	
	if (strcmp($_GET['bookmark'], "do") == 0)
	{
		if ($_POST['label'])
			set_bookmark($user, $section, $page, $_POST['label']);
		else
			set_bookmark($user, $section, $page, $sections[$section] . "/" . $section_pages[$section][$page]["name"]);
	}
	else if ($_GET['bookmark'])
	{
		del_bookmark($user, $_GET['bookmark']);
	}
	
	if ($user)
	{
		echo("<h2>Bookmarks</h2><p>");
		$bookmarks = get_bookmarks($user);
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
		<div align="right"><input type="text" size="20" style="width: 100%" name="label" class="inputbox"/>
		<input type="submit" value="Bookmark!" class="inputbox"/>
		</div>
		</form>
		<?php
		echo("</p>");
	}
?>

<?php include_once("config.php"); ?>
<?php include_once("functions.php"); ?>
<?php include_once("comments.php"); ?>
<?php include_once("session.php"); ?>
<?php include_once("page_header.php"); ?>
<?php include_once("section_menu.php"); ?>
<?php 
	$section = $_GET['section'];
	if (!$section || ($section < 0) || ($section >= count($sections)))
		$section = 0;
	$page = $_GET['page'];
	if (!$page)
		$page = 0;
?>
<table style="width: 100%">
<tbody>
<tr>
<td style="width: 20%;" valign="top">
<table><tbody><tr><td>
<?php // the left-hand menu comes here
	include_once("toc_functions.php");
	toc_header();
	for ($i = 0; $i < get_num_pages($section); $i++)
		toc_row($section, $i);
	toc_sf();
	toc_footer();
?>
</td></tr><tr><td>
<?php // the login menu comes here
	include_once("login_menu.php");
?>
</td></tr></tbody></table>
</td>
<td valign="top">
<div align="justify">
<?php // the center text comes here
	if (strcmp($_GET['login'], "why") == 0)
	{
		include_once("login_why.php");
	}
	else if (strcmp($_GET['login'], "create") == 0)
	{
		include_once("login_create.php");
	}
	else
	{
		if (strcmp($_GET['comment'], "create") == 0)
			create_comment($session, $section, $page);
		else if ((strcmp($_GET['comment'], "display") == 0) && $_GET['id'])
		{
			$title = get_comment_title($_GET['id']);
			$title = htmlentities($title);
			$text = get_comment_text($_GET['id']);
			$text = htmlentities($text);
			$text = str_replace("\r\n\r\n", "</p><p>", $text);
			$text = str_replace("\r\n", "<br/>", $text);
			$text = str_replace("\\", "", $text);
			if (!$title || !$text)
			{
				$title = "<h1>Comment not found</h1>";
				$text = "The comment you have requested, ID# " . $_GET['id'] . ", has not been found in the database or is not complete";
			}
			$text = "<p>" . $text . "</p>";
			echo("<h1>$title</h1>$text");
		}
		$filename = get_filename($section, $page);
		include_once("text/$filename.php");
	}
?>
</div>
</td>
<td style="width: 20%;" valign="top">
<?php // the right-hand text comes here
	include_once("bookmarks.php");
	show_comments($session, $section, $page);
	if (file_exists("text/rh_$filename.php"))
		include_once("text/rh_$filename.php");
	else
		include_once("text/rh_default.php");
?>
</td>
</tr>
</tbody>
</table>
<?php include_once("page_footer.php"); ?>

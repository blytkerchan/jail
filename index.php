<?php 
	include_once("config.php");
	include_once("functions.php");
	include_once("login_functions.php");
	include_once("mysql.php");
	include_once("comments.php");
	include_once("session.php");

	// open the site's database
	site_opendb();
	// check if we're creating a new account
	if (strcmp($_GET['login'], "make") == 0)
	{
		if ($_POST['name'] && (strcmp($_POST['passwd1'], $_POST['passwd2']) == 0))
		{
			if (create_account($_POST['name'], $_POST['passwd1'], $_POST['email']))
			{
				$_GET['login'] = "do";
				$_POST['passwd'] = $_POST['passwd1'];
			}
			else
				$login_status = 6;
		}
		else
			$login_status = 5;
	}
	// set up the session information & check it (also handles login)
	do_session();
	// see where we want to be
	$section = $_GET['section'];
	if (!$section || ($section < 0) || ($section >= count($sections)))
		$section = 0;
	$page = $_GET['page'];
	if (!$page)
		$page = 0;
	
	// emit the page header
	include_once("page_header.php");
	// emit the section menu
	include_once("section_menu.php");
	// start the layout
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
	if ($login_status)
	{
		include_once("login_fail.php");
		login_fail($login_status);
	}
	else if (strcmp($_GET['login'], "why") == 0)
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
			create_comment($user, $section, $page);
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
	show_comments($user, $section, $page);
	if (file_exists("text/rh_$filename.php"))
		include_once("text/rh_$filename.php");
	else
		include_once("text/rh_default.php");
?>
</td>
</tr>
</tbody>
</table>
<?php 
	include_once("page_footer.php"); 
	site_closedb();
?>

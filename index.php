<?php include_once("config.php"); ?>
<?php include_once("functions.php"); ?>
<?php include_once("page_header.php"); ?>
<?php include_once("section_menu.php"); ?>
<table style="width: 100%">
<tbody>
<tr>
<td style="width: 20%;" valign="top">
<?php // the left-hand menu comes here
	$section = $_GET['section'];
	if (!$section)
		$section = 0;
	include_once("toc_functions.php");
	toc_header();
	for ($i = 0; $i < get_num_pages($section); $i++)
		toc_row($section, $i);
	toc_sf();
	toc_footer();
?>
</td>
<td valign="top">
<div align="justify">
<?php // the center text comes here
	$page = $_GET['page'];
	if (!$page)
		$page = 0;
	$filename = get_filename($section, $page);
	include_once("text/$filename.php");
?>
</div>
</td>
<td style="width: 20%;" valign="top">
<?php // the right-hand text comes here
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

<?php
function back_to_top()
{
	?>
	<div align="right" style="font-size : 8pt;"><a href="#top">back to top</a></div>
	<?php
}

function get_name($section, $page)
{
	global $section_pages;

	return $section_pages[$section][$page]["name"];
}

function get_filename($section, $page)
{
	global $section_pages;

	return $section_pages[$section][$page]["filename"];
}

function get_num_pages($section)
{
	global $section_pages;

	return count($section_pages[$section]);
}

function get_section_no($section_name)
{
	global $sections;
	$section = 0;

	for ($i = 0; $i < count($sections); $i++)
	{
		if (strcasecmp($sections[$i], $section_name) == 0)
		{         
			$section = $i;
			break;
		}
	}

	return $section;
}

function get_page_no($section, $page_name)
{
	global $section_pages;

	$pages = $section_pages[$section];
	for ($i = 0; $i < count($pages); $i++)
	{
		if ((strcasecmp($pages[$i]["filename"], $page_name) == 0) || (strcasecmp($pages[$i]["name"], $page_name) == 0))
		{
			return $i;
		}
	}

	return 0;
}

function link_to_section($section_name, $text)
{
	$section = get_section_no($section_name);
	
	echo("<a href=\"index.php?section=$section\" target=\"_top\">$text</a>");
}

function link_to_page($section_name, $page_name, $text)
{
	$section = get_section_no($section_name);
	$page = get_page_no($section, $page_name);

	echo("<a href=\"index.php?section=$section&page=$page\" target=\"_top\">$text</a>");
}
?>

<?php
$sections = array(
	0 => "General", 
	1 => "Language", 
	2 => "Development", 
	3 => "Download");
$section_pages = array(
	0 => array(
		0 => array(
			"name" => "Welcome",
			"filename" => "welcome"), 
		1 => array(
			"name" => "Goals",
			"filename" => "goals"), 
		2 => array(
			"name" => "How you can help",
			"filename" => "how_you_can_help"), 
		3 => array(
			"name" => "Designing the Language",
			"filename" => "design"), 
		4 => array(
			"name" => "Progress",
			"filename" => "progress")),
	1 => array(
		0 => array(
			"name" => "The Language",
			"filename" => "language"), 
		1 => array(
			"name" => "Operators and Precedence",
			"filename" => "jailop"), 
		2 => array(
			"name" => "Types in Jail",
			"filename" => "jailtype")),
	2 => array(
		0 => array(
			"name" => "Development",
			"filename" => "development"), 
		1 => array(
			"name" => "Technical Architecture",
			"filename" => "tech_arch"), 
		2 => array(
			"name" => "The Jail License",
			"filename" => "license")),
	3 => array(
		0 => array(
			"name" => "Downloading",
			"filename" => "download"), 
		1 => array(
			"name" => "libcontain: the container library",
			"filename" => "libcontain"), 
		2 => array(
			"name" => "libmemory: the memory management library",
			"filename" => "libmemory")));

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

?>

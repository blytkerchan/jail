<?php
/* the currently logged-in user */
$user = 0;
$session = "";
$login_status = 0; // 0 == OK; 1 == login_error; 2 == email_not_found; 3 == mail_sent; 4 == mail_not_sent; 5 == password_not_same; 6 == create_account_failed

/* database access used by the site */
$db_cfg["host"] = "localhost";
$db_cfg["dbname"] = "jail";
$db_cfg["user"] = "jail";
$db_cfg["passwd"] = "jail";

/* the contents of the site - the directories */
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

?>

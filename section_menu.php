<div align="center">
<table width="60%" class="menu"><tr>
<?php
	for ($i = 0; $i < count($sections); $i++)
	{
		echo("<td width=\"" . 100 / count($sections) . "%\">");
		echo("<a class=\"toc\" target=\"_top\" href=\"index.php?section=$i\">" . $sections[$i] . "</a></td>");
		echo("</td>");
	}
?>
</tr></table>
</div>

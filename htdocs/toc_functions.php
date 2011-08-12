<?php
function toc_header()
{
?>
<table align="left" border="1" cellspacing="0" cellpadding="8"
class="toc">
<tbody>
<tr>
<td>
<table style="width: 100%;" cellspacing="0" cellpadding="8">
<tbody>
<?php
}

function toc_footer()
{
?>
</tbody></table>
</td></tr></tbody></table>
<?php
}

function toc_row($section, $page)
{
?>
<tr>
	<td valign="middle">
		<a class="toc" href="index.php?section=<?php echo($section) ?>&page=<?php echo($page) ?>" target="_top">
			<?php echo(get_name($section, $page)) ?>
		</a>
	</td>
</tr>
<?
}
function toc_sf()
{
	?>
<tr>
	<td valign="top">
		<a class="toc" 
			href="http://sourceforge.net/projects/jail-ust" 
			target="_top">
			SourceForge project page
		</a></td>
</tr>
	<?php
}
?>

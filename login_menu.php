<?php
	if (!$user)
	{
	?>
		<h2>Login</h2>
		<div style="margin-top: 0; font-size=smaller;">
		<p>
		Please login with your username and password<br/>
		<form method="post" action="index.php?login=do&section=<?php echo($section);?>&page=<?php echo($page); ?>">
		<table width="100%" border="0">
		<tbody>
		<tr>
			<td colspan="2">Name:</td>
		</tr>
		<tr>
			<td>&nbsp;</td><td align="right"><input type="text" size="20" name="name"/></td>
		</tr>
		<tr>
			<td colspan="2">Password:</td>
		</tr>
		<tr>
			<td>&nbsp;</td><td align="right"><input type="password" size="20" name="passwd"/></td>
		</tr>
		</tbody>
		</table>
		<input type="submit" value="Login!"/><input type="reset" value="Reset"/>
		</form>
		<table width="100%"><tbody><tr><td><a href="index.php?login=why&section=<?php echo($section); ?>&page=<?php echo($page); ?>">why?</a></td><td align="right"><a href="index.php?login=create&section=<?php echo($section); ?>&page=<?php echo($page); ?>">Create a login</a></td></tr></tbody></table>
		</p>
		</div>
	<?php
	}
	else
	{
		?>
		<table>
		<tbody>
		<tr><td>
		<form method="post" 
				action="index.php?logout=1&section=<?php 
					echo($section); 
				?>&page=<? 
					echo($page);
				?>">
			<input type="submit" value="Log out"/>
		</form>
		</td>
		<?php
		if (!$remembered)
		{
			?>
			<td>
			<form method="post"
					action="index.php?login=remember&section=<?php
						echo($section);
					?>&page=<?
						echo($page);
					?>">
				<input type="submit" value="Remember me"/>
			</form>
			<?php
		}
		?>
		</td></tr>
		</tbody>
		</table>
		<?php
	}
?>

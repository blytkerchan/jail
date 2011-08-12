<?php
	function login_fail($how)
	{
		switch ($how)
		{
		case 1 : // login failed
?>
<h1>Username Unknown or Password Incorrect</h1>
<p>The login and/or password you have entered have not been found. This could mean any of the following:
<ul>
<li>You don't have a login and password for this site yet<br/>
In that case, you can create one by clicking <a href="index.php?section=<?php echo($section);?>&page=<?php echo($page);?>&login=create">here</a></li>
<li>You made a mistake when typing your login and/or password<br/>
In that case, you should try again</li>
<li>You have forgotten your login and/or password<br/>
In that case, please enter your E-mail address below to have it sent to you.<br/>
<form method="post" action="index.php?login=send&section=<?php echo($section); ?>&page=<?php echo($page); ?>"><input type="text" name="email" size="50"/><input type="submit" value="Submit"/></form></li>
<li>There is a bug in the site or the database is inaccessible<br/>
In that case, there is nothing you can do but try again and/or report the error to the author.</li>
</ul>
Please note that any account that has not been used for more than a year (368 days) will be removed from the database.
</p>
<?php
			break;
		case 2 :	// E-mail address not found
?>
<h1>The E-mail address you have entered was not found</h1>
<p>This could mean any of the following things:
<ul>
<li>You don't actually have an account on this site yet<br/>
In that case, go ahead and create one</li>
<li>The E-mail address you've entered is the wrong one<br/>
In that case: try again below:<br/>
<form method="post" action="index.php?login=send&section=<?php echo($section); ?>&page=<?php echo($page); ?>"><input type="text" name="email" size="50"/><input type="submit" value="Submit"/></form></li>
<li>You didn't enter your E-mail address when you created your account<br/>
This is a bad idea (though perfectly legal): you'll have to create a new account</li>
</ul>
</p>
<?php
			break;
		case 3 :
?>
<h1>Mail Sent</h1>
<p>An E-mail with your new user credentials (same username, new password) has been sent to the E-mail address you have provided. You can use the information in that E-mail to log in on this site.</p>
<?php
			break;
		case 4 :
?>
<h1>Mail Not Sent</h1>
<p>The E-mail containing your new user credentials (same username, new password) could not be sent to the E-mail address you have provided. There could be any number of reasons for this, but seeing as your password was changed in the process, your account is now pretty much useless. Please send an E-mail to the author of this site (<a href="mailto:ronald@landheer.com">Ronald Landheer-Cieslak</a>) to advise him of the problem.</p>
<?php
			break;
		case 5 :
?>
<h1>Account Creation Error</h1>
<p>The passwords you have entered were not the same - the account has not been created.</p>
<p>Please try again</p>
<?php
			break;
		case 6 :
?>
<h1>Account Creation Error</h1>
<p>The site failed to create the account with the requested parameters. The most probable reason for this is that an account with the name you wanted already exists - please try a different username.</p>
<?php
			break;
		}
	}
?>

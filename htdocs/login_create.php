<h1>Creating a login</h1>
<p>To create a new account for this site, please fill in the form below, but
before you do that, please take note of the following policies:
<ul>
<li>Inactive accounts may be removed from the databases after 368 days (or more) of inactivity</li>
<li>Registered users have the right to add comments to the site. These comments are readible by all visitors of the site and should therefore be suitable for reading by people of all ages (including children). It is your responsibility to make it so</li>
<li>I (Ronald Landheer-Cieslak) do not manage the database this site stores its information in: it is managed by the SourceForge team. That means that they, as well as I, have access to any information you fill in in the form below. Personally, I trust them enough to give them that information (i.e. my username and E-mail address) but I thought you should know.</li>
<li>I (Ronald Landheer-Cieslak) also have access to the data in the database. The data I have access to will be treated confidentially (i.e. in most cases I won't even look at it myself)</li>
<li>Although the password is stored in the database in an unreadable form, this site is not (nor intended to be) secure. Do not use any credentials on this site that you might also use for, say, your bank account</li>
<li>Your E-mail address is optional, but it will only be used to 
<ul>
<li>Send you your login information if you've lost it</li>
<li>Tell you about new features on the site that are deemed very important</li>
</ul>
</li>
</ul>
<form method="post" action="index.php?login=make&section=<?php echo($section); ?>&page=<?php echo($page); ?>">
<table width="100%">
<tbody>
<tr><td>Name: </td><td align="right"><input type="text" name="name" size="50"/></td></tr>
<tr><td>E-mail address (optional): </td><td align="right"><input type="text" name="email" size="50"/></td></tr>
<tr><td>Password</td><td align="right"><input type="password" name="passwd1" size="50"/></td></tr>
<tr><td>Password (again)</td><td align="right"><input type="password" name="passwd2" size="50"/></td></tr>
<tr><td colspan="2" align="right"><input type="submit" value="Submit"/></td></tr>
</tbody>
</table>
</form>
</p>

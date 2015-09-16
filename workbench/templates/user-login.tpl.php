<?php // print $messages; ?>
<style type="text/css" media="all">
#branding {
  display: none;
}

#logo_container {
  margin: 0 auto;
  width: 420px;
  text-align: center;
  font-family: "Georgia", "Times New Roman", Serif;
}

#logo_container h1 {
  font-size: 2.3em;
  font-weight: normal;
  margin-bottom: 20px;
}

#page {
  /*
  margin: 10% auto 0 auto;
  Added next 2 lines  */
  margin: 0 auto;
  padding: 20 0 0 0;
  width: 420px;
}

.form-item {
  text-align: center;
}

input.form-text {
  width: 96%;
  height: 1.6rem;
  font-size: 1rem;
  padding: 6px;
}
</style>
<div id="logo_container">
  <img src="<?php echo base_path() . drupal_get_path('module', 'sidora'); ?>/images/logo.png" alt="Smithsonian logo">
  <h1>SIdora Workbench</h1><!-- SIdora team officially has the SI capitalized for anything reaching user, even though they often dont on internal information-->
</div>
<?php
 /* split the username and password from the submit button 
   so we can put in links above */
    $form['name']['#value'] = '';
    print drupal_render($form['name']);
    print drupal_render($form['pass']);
    print drupal_render($form['form_build_id']);
    print drupal_render($form['form_id']);
    print variable_get('sidora_login_message', '');
    print drupal_render($form['actions']);
?>
	<p>Not a member? <a href="<?php echo base_path(); ?>sidora/request_account"> Submit an Account Request</a><p>
<p>Need help? <a href="<?php echo base_path(); ?>sidora/contact_us">Contact Us</a>
<!--<?php $default_to = variable_get('system_mail', '');
echo '<a href="mailto:'.variable_get('account_request_email', $default_to).'?Subject=SIdora Account Help&body=My name is:-Enter Name- and my Email is:-Enter Email- and I am requesting help with my SIdora Account.  -Enter more information that indicates the kind of assistance you would like-)">Contact Us</a>';?>
--></p>
<!-- <p><a href="user/password">Request a New Password</a></p> -->

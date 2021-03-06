<?php 
/*
 * Copyright 2015 Smithsonian Institution.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.You may obtain a copy of
 * the License at: http://www.apache.org/licenses/
 *
 * This software and accompanying documentation is supplied without
 * warranty of any kind. The copyright holder and the Smithsonian Institution:
 * (1) expressly disclaim any warranties, express or implied, including but not
 * limited to any implied warranties of merchantability, fitness for a
 * particular purpose, title or non-infringement; (2) do not assume any legal
 * liability or responsibility for the accuracy, completeness, or usefulness of
 * the software; (3) do not represent that use of the software would not
 * infringe privately owned rights; (4) do not warrant that the software
 * is error-free or will be maintained, supported, updated or enhanced;
 * (5) will not be liable for any indirect, incidental, consequential special
 * or punitive damages of any kind or nature, including but not limited to lost
 * profits or loss of data, on any basis arising from contract, tort or
 * otherwise, even if any of the parties has been warned of the possibility of
 * such loss or damage.
 *
 *
 * This distribution includes several third-party libraries, each with their own
 * license terms. For a complete copy of all copyright and license terms, including
 * those of third-party libraries, please see the product release notes.
 */
?>
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
  margin: 10% auto 0 auto;
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
  <h1>Sidora Workbench</h1>
</div>
<?php 
$form['name']['#value'] = '';
print drupal_render_children($form); 
?>
<p>SIdora is currently limited only to selected projects but access will be
gradually expanded. If you are working on one of the projects please
fill out this form to request an account. Smithsonian account holders
must provide their SI usernames. Other users should provide a preferred
username and valid email address. You will be contacted by email with an
account invitation as soon as we have processed your request. If you do
not receive it shortly please check your spam filter. Feel free to
contact us again if you have not received a timely response. Passwords
should never be sent in any request and we will never ask for them. For
SI account holders your SIdora account is linked to your SI account.
Other users will be provided account instructions with your invitation.<p>

<?php
/**
 * @file
 *
 * This module's Admin settings form.
 */

/**
 * Defines this modules admin settings form.
 *
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The drupal form definition.
 */
function sidora_admin_settings() {
  module_load_include('inc', 'islandora', 'includes/content_model.autocomplete');
  $form = array(
/*

    'si_exhibition_data_url' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the website configuration for the page (not where to obtain the Fedora data itself)'),
      '#default_value' => variable_get('si_exhibition_data_url', 'http://***REMOVED***/exhibition_config/info/'),
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
*/
/*
    'sidora_metadata_xsl' => array(
      '#type' => 'textarea',
      '#title' => t('XSL for transforming the object metadata into HTML for display on the "Metadata" display'),
      '#default_value' => sidora_xsl_string(),
      //'#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
*/

    'si_exhibition_website_location' => array(
      '#type' => 'textfield',
      '#title' => t('The prefix for exhibition links'),
      '#default_value' => variable_get('si_exhibition_website_location', ''),
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
    'sidora_ontology_json' => array(
      '#type' => 'textarea',
      '#title' => t('JSON determining the layout and form choices presented for add new concept'),
      '#default_value' => variable_get('sidora_ontology_json',sidora_ontology_json_default()),
    ),
  );
  //$form['#submit'][]= 'sidora_admin_settings_save';

  module_load_include('inc', 'xml_form_builder', 'XMLFormRepository');
  $names = XMLFormRepository::GetNames();
  // No forms exist can only create.
  /*
  if (count($names) == 0) {
    return '<div>No forms are defined. Please create a new form.</div><br/>';
  }

  $table = array(
    'header' => array(
      array(
        'data' => t('Title'),
      ),
      array(
        'data' => t('Type'),
      ),
      array(
        'data' => t('Operations'),
        'colspan' => 6,
      ),
    ),
    'rows' => array(),
  );
  
  foreach ($names as $form_info) {
    $name = $form_info['name'];
    if ($form_info['indb']) {
      $type = t('Custom');
      $edit = l(t('Edit'), xml_form_builder_get_edit_form_path($name));
      $delete = l(t('Delete'), xml_form_builder_get_delete_form_path($name));
    }
    else {
      $type = t('Built-in');
      $edit = '';
      $delete = '';
    }
    $copy = l(t('Copy'), xml_form_builder_get_copy_form_path($name));
    $view = l(t('View'), xml_form_builder_get_view_form_path($name));
    $export = l(t('Export'), xml_form_builder_get_export_form_path($name));
    $associate = l(t('Associate'), xml_form_builder_get_associate_form_path($name));

    $table['rows'][] = array(
      $name,
      $type,
      $copy,
      $edit,
      $view,
      $export,
      $delete,
      $associate,
    );
  }
  */

  $form_names_dd = '<select class="form-names-dd form-select">';
  $form_names_dd .= '<option value="sidora_all">All forms that are not specifically called out</option>';
  foreach($names as $form_info){ $name = $form_info['name']; $form_names_dd .= '<option value="'.$name.'">'.$name.'</option>'; }
  $form_names_dd .= '</select>';

  $names = islandora_get_content_model_names();

  $model_names_dd = '<select class="model-names-dd form-select">';
  $model_names_dd .= '<option value="sidora_all">All models that are not specifically called out</option>';
  foreach($names as $model => $name){ $model_names_dd .= '<option value="'.$model.'">'.$name.'</option>'; }
  $model_names_dd .= '</select>';
  
  $directory_to_list_for_xsl = DRUPAL_ROOT.'/'.drupal_get_path('module','sidora').'/XSL';
  $xsl_filenames = array_diff(scandir($directory_to_list_for_xsl), array('..', '.', '.htaccess'));

  $xsl_names_dd = '<select class="xsl-names-dd form-select">';
  $xsl_names_dd .= '<option value="">None - Direct metadata to browser</option>';
  foreach($xsl_filenames as $name){ 
    if (sidora_endsWith(strtolower($name), ".xsl")){
      $xsl_names_dd .= '<option value="'.$name.'">'.$name.'</option>'; 
    }
  }
  $xsl_names_dd .= '</select>';
  /* example 
  $existing_json_sidora_form_config = '
    {
      "xsl_config":[
        {"form":"Expedition","model":"si:siBasicObject","dsid":"three","file":"four"},
        {"form":"Archaeology Site","model":"si:imageCModel","dsid":"three","file":"basic_form_definition_list.xsl"}
      ]
    }
  ';
  */
  $existing_json_sidora_form_config = variable_get('xsl_form_config', '');
  $sfc = json_decode($existing_json_sidora_form_config);
  $sfc_table = '
<script>
window.filesUpdated = function(updateObj){
  console.log(updateObj);
  var currentOptions = jQuery(jQuery(".xsl-names-dd")[0]).children("option").map(function() {return jQuery(this).val();}).get();
  var files = updateObj.files;
  for(var i = 0; i < files.length; i++){
    var currName = files[i].name;
    if (currentOptions.indexOf(currName) == -1){
      //If this is a brand new file, add it to the end of the file listing
      var optionsHtml = "<option value=\""+currName+"\">"+currName+"</option>";
      jQuery(".xsl-names-dd").append(optionsHtml);
    } 
  }
}
window.recolorTable = function(){
  var tableRows = jQuery(".sfc-table tr").removeClass("even").removeClass("odd");
  for(var i=0;i<tableRows.length;i++){
    jQuery(tableRows[i]).addClass( (i%2==0)?"even":"odd" );
  }
}
window.generateSidoraFormConfig = function(){
  var toAdd = jQuery(".sfc-table tr:visible").not(".removed").has("td");
  var obj = {"xsl_config":[]};
  for (var i = 0; i < toAdd.length; i++){
    obj.xsl_config.push({
      "form": jQuery(toAdd[i]).find(".form-names-dd").val(),
      "model": jQuery(toAdd[i]).find(".model-names-dd").val(),
      "dsid": jQuery(toAdd[i]).find("input[type=\'textfield\']").val(),
      "file": jQuery(toAdd[i]).find(".xsl-names-dd").val()
    });
  }
  return obj;
}
jQuery(document).ready(function(){
  jQuery("#anf-button").click(function(){
  // var myWin = window.open(Drupal.settings.basePath+"sidora/test/upload2","XSL_UPLOAD","width=550,height=250,0,status=0,");
	// added by RAmlani on 3/31/15 to stop using sidora/testing for uploading new xsl files in sidora admin on production site.
  var myWin = window.open(Drupal.settings.basePath+"sidora/info/xsl_Upload/upload2","XSL_UPLOAD","width=550,height=250,0,status=0,");
  });
 /* Delete XSL button - Lets user delete XSL files - Functionality added by ***REMOVED*** on 3/18/15 */
  // add jquery to the delete xsl button
	jQuery("#dxsl-button").click(function(){
 //   var delete_xslWin = window.open(Drupal.settings.basePath+"sidora/delete_xsl","XSL_DELETE","width=\'100%\',height=\'100%\',0,status=0,");
    Shadowbox.open({
      content:    Drupal.settings.basePath+"sidora/delete_xsl",
      player:     "iframe",
      title:      "Delete XSL",
      options: {
        onFinish:  function(){}
      }
    });

 });
	jQuery("#anr-button").click(function(){
    var tableRow = \'<tr><td>'.$form_names_dd.'</td><td>'.$model_names_dd.'</td><td><input type="textfield" class="form-text" /></td><td><select class="xsl-names-dd form-select">\'+jQuery(".xsl-names-dd")[0].innerHTML.replace(\'selected="selected"\',"")+\'</select></td><td><input type="button" class="form-submit remove-row" value="Remove"/></td></tr>\';
    jQuery(".sfc-table tbody").append(tableRow);
    recolorTable();
    window.establishRemoveReadd();
  });
  window.establishRemoveReadd();
});
window.establishRemoveReadd = function(){
  jQuery(".sfc-table .remove-row").not(".removable").click(function(){
    if (jQuery(this).closest("tr").hasClass("removed")){
      jQuery(this).closest("tr").removeClass("removed");
      console.log("remove");
      jQuery(this).val("Remove");
    }else{
      jQuery(this).closest("tr").addClass("removed"); //make red
      console.log("readd");
      jQuery(this).val("Readd");
    }
    recolorTable();
  }).addClass("removable");
}
window.formPreSubmit = function(){
  jQuery("input[name=\'xsl_form_config\']").val(JSON.stringify(generateSidoraFormConfig()));
}
</script>
<style>
tr.removed.even {
  background: #A00000;
}
tr.removed.odd {
  background: #600000;
}
</style>
<table class="sticky-enabled tableheader-processed sticky-table sfc-table">
 <thead><tr><th>Form</th><th>Model</th><th>DSID</th><th colspan="1">File</th><th>Remove</th> </tr></thead>
<tbody>
  <tr style="display:none;"><td>'.$form_names_dd.'</td><td>'.$model_names_dd.'</td><td><input type="textfield" class="form-text" /></td><td>'.$xsl_names_dd.'</td><td><input type="button" class="form-submit remove-row" value="Remove"/></td></tr>

';
  $even = FALSE;
  if (!empty($sfc->xsl_config)){
    foreach ($sfc->xsl_config as $config_line){
      $class = $even?'even':'odd';
      $sfc_table .= '<tr class="'.$class.'">';
      $sfc_table .= '<td>';
      $sfc_table .= str_replace("value=\"$config_line->form\"",'value="'.$config_line->form.'" selected="selected"',$form_names_dd);
      $sfc_table .= '</td>';
      $sfc_table .= '<td>';
      $sfc_table .= str_replace("value=\"$config_line->model\"",'value="'.$config_line->model.'" selected="selected"',$model_names_dd);
      $sfc_table .= '</td>';
      $sfc_table .= '<td>';
      $sfc_table .= '<input type="textfield" class="form-text" value="'.$config_line->dsid.'" />';
      $sfc_table .= '</td>';
      $sfc_table .= '<td>';
      $sfc_table .= str_replace("value=\"$config_line->file\"",'value="'.$config_line->file.'" selected="selected"',$xsl_names_dd);
      $sfc_table .= '</td>';
      $sfc_table .= '<td>';
      $sfc_table .= '<input type="button" class="form-submit remove-row" value="Remove"/>';
      $sfc_table .= '</td>';
      $sfc_table .= '</tr>';
      $even = !$even;
    }
  }
  $sfc_table .= '</tbody></table>';
  $add_new_files_button = '<input id="anf-button" type="button" class="form-submit" value="Upload New XSL File"/>';
  $add_new_row_button = '<input id="anr-button" type="button" class="form-submit" value="Add New Row to Configure"/>';
  $sfc_label = '<label>Metadata Form XSL Configuration</label>';
  // create the delete xsl files button
	$remove_xsl_file_button = '<input id="dxsl-button" type="button" class="form-submit" value="Remove a XSL File"/>';
	
  $form['xsl_form_config'] = array(
      '#type' => 'hidden',
      '#default_value' => variable_get('xsl_form_config', ''),
     // '#suffix' => $sfc_label.$sfc_table.$add_new_row_button.$add_new_files_button,//.theme('table',$table),
      '#suffix' => $sfc_label.$sfc_table.$add_new_row_button.$add_new_files_button.$remove_xsl_file_button,//.theme('table',$table),
  );

  $models = array();
  $model = 'not_a_content_model';
	$models[] = array(
		'#type' => 'fieldset',
		'#title' => 'Items that are not content models',
    '#description' => t('If the DSID does not exist for the object, the display will not be attempted.'),
		'resource_viewer_dsid_'.$model => array(
			'#type' => 'textfield',
			'#title' => t('DSID'),
			'#default_value' => variable_get('resource_viewer_dsid_'.$model,'DC'),
		),
		'resource_viewer_viewer_'.$model => array(
			'#type' => 'select',
			'#title' => t('Viewer'),
			'#options' => array(
        //remember to change this and the one below it when adding a new type
				'direct' => t('Direct stream to browser (actual image / binary / text)'),
				'web_view' => t('Direct stream to browser for web viewable items, download message for others'),
        'flexpaper_pdf' => t('Flexpaper / Convert to flexpaper / direct stream to browser'),
				'IDS' => t('Smithsonian IDS Viewer'),
        'OpenSeaDragon' => t('OpenSeaDragon'),
				'none' => t('None - no display available'),
				'never' => t('Never set'),
			),
			'#default_value' => variable_get('resource_viewer_viewer_'.$model,'never'),
		),
    //TODO: make both this and the looped additions to use the same code instead of copy/paste
	);

  foreach($names as $model => $name){
    $models[] = array(
      '#type' => 'fieldset',
      '#title' => $name,
      'resource_viewer_dsid_'.$model => array(
        '#type' => 'textfield',
				'#title' => t('DSID'),
				'#default_value' => variable_get('resource_viewer_dsid_'.$model,'OBJ'),
      ),
      'resource_viewer_viewer_'.$model => array(
        '#type' => 'select',
				'#title' => t('Viewer'),
        '#options' => array(
          'direct' => t('Direct stream to browser (actual image / binary / text)'),
				  'web_view' => t('Direct stream to browser for web viewable items, download message for others'),
          'flexpaper_pdf' => t('Flexpaper / Convert to flexpaper / direct stream to browser'),
          'JW Audio Player' => t('JW Player for audio'),
					'IDS' => t('Smithsonian IDS Viewer'),
          'OpenSeaDragon' => t('OpenSeaDragon'),
          'none' => t('None - no display available'),
          'never' => t('Never set'),
        ),
				'#default_value' => variable_get('resource_viewer_viewer_'.$model,'never'),
      ),
    );

  }
  $form['govt_msg'] = array(
    'fieldset' => array(
      '#type' => 'fieldset',
      '#title' => 'Log In Message',
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'sidora_login_message' => array(
        '#type' => 'textarea',
        '#title' => t('Government Warning text'),
        '#default_value' => variable_get('sidora_login_message', ''),
        '#description' => t('This message is shown on the log in screen.'),
      ),
    ),
  );
  $form['one'] = array(
    'fieldset' => array(
      '#type' => 'fieldset',
      '#title' => 'Resource Viewer Content Model Settings',
      '#description' => t('Listed below are all the Content Models with viewers for Content Viewer.  <br/>"Never set" will also not attempt to display the item, and is used for record-keeping as opposed to an operational difference from "None" <br/>"Smithsonian IDS Viewer" will require that the Smithsonian IDS have access to the image, which means that the image will need to be set to publicly viewable.  (In development - the direct download is to be used when not publicly viewable.)'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'models' => $models,
    ),
  );
  $form['sidora_person'] = array(
    'fieldset' => array(
      '#type' => 'fieldset',
      '#title' => 'Account Creation',
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('When automatically generating an object for a newly created user, the model is checked for associations.  If there is more than one association or no associations available the additional defaults below are used.'),
      'sidora_automatic_person_model' => array(
        '#type' => 'textfield',
        '#title' => t('Model'),
        '#default_value' => variable_get('sidora_automatic_person_model', 'si:HierarchyObject'),
      ),
      'sidora_automatic_person_form' => array(
        '#type' => 'textfield',
        '#title' => t('Form name'),
        '#default_value' => variable_get('sidora_automatic_person_form', 'Person 2015'),
        '#description' => t('This value will only be used if there is not exactly one association with the model listed above'),
      ),
      'sidora_automatic_person_dsid' => array(
        '#type' => 'textfield',
        '#title' => t('DSID for editing'),
        '#default_value' => variable_get('sidora_automatic_person_dsid', 'DESCMETA'),
        '#description' => t('This value will only be used if there is not exactly one association with the model listed above'),
      ),
    ),
  );
/*
  $form['one']['si_exhibition_data_urli3'] = array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the website configuration for the page (not where to obtain the Fedora data itself)'),
      '#default_value' => variable_get('si_exhibition_data_url', 'http://***REMOVED***/exhibition_config/info/'),
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    );
*/
  $form['OpenSeaDragonInfo'] = array(
    'fieldset' => array(
      '#type' => 'fieldset',
      '#title' => 'OpenSeaDragon Info',
      '#description' => t(''),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'osd_proxy_pid_prefix' => array(
        '#type' => 'textfield',
        '#title' => t('Prefix the URL before the pid for the base image'),
        '#default_value' => variable_get('osd_proxy_pid_prefix', ''),
        '#description' => t('The URL for the base image is made up of this prefix, then the pid, then the post-fix. (ex: http://***REMOVED***:8080/fedora/objects/ )'),
      ),
      'osd_proxy_pid_postfix' => array(
        '#type' => 'textfield',
        '#title' => t('Post-fix for the URL after the pid for the base image'),
        '#default_value' => variable_get('osd_proxy_pid_postfix', ''),
        '#description' => t('The URL for the base image is made up of the prefix, then the pid, then this post-fix. (ex: /datastreams/OBJ/content )'),
      ),
      'osd_proxy_resolver_url' => array(
        '#type' => 'textfield',
        '#title' => t('The base URL for the resolver'),
        '#default_value' => variable_get('osd_proxy_resolver_url', ''),
        '#description' => t('The resolver is the location of the image resizer itself. (ex: http://***REMOVED***/adore-djatoka/resolver )'),
      ),
    ),
  );
  $form['#attributes'] = array('OnSubmit' => 'formPreSubmit();');
  return system_settings_form($form);
}

/*
function sidora_admin_settings_save(){
  dsm("Form saving");
}
*/

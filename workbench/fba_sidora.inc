<?php

define("FBA_URI", 'http://fba.org.uk/schemas/fba/fba_rdf/1.0.0/');
define("FOAF_URI", 'http://xmlns.com/foaf/spec/'); //FBA MODIFICATION BY NBYWELL 05/10/2015
define("PURL_TERMS_URI", 'http://purl.org/dc/terms/'); //FBA MODIFICATION BY NBYWELL 05/10/2015
define("PURL_ELEMENTS_URI", 'http://purl.org/dc/elements/1.1/'); //FBA MODIFICATION BY NBYWELL 05/10/2015
define("FBA_MOLES_URI", 'http://fba.org.uk/schemas/moles/3.4.0'); //JIRA AEDA-22

function sidora_user_presave(&$edit, $account, $category) {
  if (strcmp(filter_input(INPUT_REQUEST, 'op'), 'Log in') === 0) {
    //check to see if the user's private collection has been created
    $accountpid = $edit['data']['islandora_user_pid'];
    $usercollectionns = str_replace(array('-', ':'), '', $accountpid);
    $connection = islandora_get_tuque_connection();
    $repository = $connection->repository;
    //Does object exist already?
    $object = islandora_object_load($usercollectionns);
    if (!$object) {
      if (system_is_fba()) {
        $user_collection_object = constructFbaTuqueObject($usercollectionns . ':collection'); // create an object with the given PID
      }
      else {
        $user_collection_object = $repository->constructObject($usercollectionns . ':collection'); // create an object with the given PID
      }
      $user_collection_object->models = array('islandora:collectionCModel');
      $user_collection_object->label = $accountpid . ' Private Collection';
      $user_collection_object->owner = $accountpid;
      $prefix = sidora_get_system_prefix();

      $user_collection_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $prefix . ':root');
      $module_path = drupal_get_path('module', 'sidora');
      sidora_update_datastream($user_collection_object, $module_path . '/fba/collection_policies/fba_sidora_collection_policy.xml', 'COLLECTION_POLICY', NULL, 'text/xml');
      sidora_update_datastream($user_collection_object, sidora_create_default_xacml($account), 'POLICY', NULL, 'text/xml');
      //SFOX add a CHILD_SECURITY XACML, so that objects in the collection can be viewed by owner only.
      sidora_update_datastream($user_collection_object, sidora_create_default_xacml($account), 'CHILD_SECURITY', NULL, 'text/xml');

      $new_fedora_object = islandora_add_object($user_collection_object); //throw an error if this fails?
      //SFOX Add FBA relext of "isOwnerOf"
      $user_account = $repository->getObject($accountpid);
      sidora_set_relationship($user_account->relationships, FBA_URI, 'isOwnerOf', $usercollectionns . ':collection');
    }
  }
}

function sidora_return_fba_concept_rdfs() {
  $contypes = sidora_return_fba_concepts();
  foreach ($contypes as $contype) {
    $retarray[] = $contype[1];
  }
  return $retarray;
}

function sidora_return_fba_resource_rdfs() {
  $returnarray = array_merge(sidora_return_fba_core_resource_rdfs(), sidora_return_fba_supp_resource_rdfs());
  return array_unique($returnarray);
}

function sidora_return_fba_core_resource_rdfs() {
  $restypes = sidora_return_fba_resources();
  foreach ($restypes as $restype) {
    $retarray[] = $restype[1];
  }
  return array_unique($retarray);
}

function sidora_return_fba_supp_resource_rdfs() {
  $supptypes = sidora_return_fba_supplementary();
  foreach ($supptypes as $supptype) {
    $retarray[] = $supptype[1];
  }
  return array_unique($retarray);
}

function sidora_return_fba_concept_cmodels() {
  $concepts = sidora_return_fba_concepts();
  foreach ($concepts as $concept) {
    $retarray[] = $concept[0];
  }
  return $retarray;
}

function sidora_return_fba_resource_cmodels() {
  $resources = sidora_return_fba_resources();
  foreach ($resources as $resource) {
    $retarray[] = $resource[0];
  }
  return $retarray;
}

function sidora_fba_return_vocab_cmodels() {
  return array(
    'fba:vocabpref_CModel',
    'fba:vocabnonpref_CModel',
    'fba:geopref_CModel',
    'fba:geononpref_CModel',
  );
}

function sidora_return_fba_concepts() {
  return array(
    array('fba:activity_cmodel', 'hasActivity', 'dtcac', 'Activity'),
    array('fba:dataset_cmodel', 'hasDataset', 'dtcds', 'Dataset'),
  );
}

function sidora_return_fba_resources() {
  return array_merge(sidora_return_fba_data_components(), sidora_return_fba_supplementary());
}

function sidora_return_fba_data_components() {
  return array(
    array('fba:measurement_cmodel', 'hasMeasurement', 'dtcmt', 'Measurement'),
    array('fba:analysis_cmodel', 'hasAnalysis', 'dtcan', 'Analysis'),
    array('fba:litreview_cmodel', 'hasLiteratureReview', 'dtclr', 'Literature Review'),
    array('fba:simulation_cmodel', 'hasSimulation', 'dtcsi', 'Simulation'),
    array('fba:synthesis_cmodel', 'hasSynthesis', 'dtcsy', 'Synthesis'),
  );
}

function sidora_return_fba_supplementary() {
  return array(
    array('fba:supp_basic_image_cmodel', 'hasSupplementaryFile', 'dtcbim', 'Image'),
    array('fba:supp_large_image_cmodel', 'hasSupplementaryFile', 'dtclim', 'Large Image'),
    array('fba:supp_pdf_cmodel', 'hasSupplementaryFile', 'dtcpdf', 'PDF'),
    array('fba:supp_text_cmodel', 'hasSupplementaryFile', 'dtctxt', 'Text'),
    array('fba:supp_excel_cmodel', 'hasSupplementaryFile', 'dtcxl', 'Excel'),
    array('fba:supp_word_cmodel', 'hasSupplementaryFile', 'dtcwd', 'Word'),
    array('si:imageCModel', 'hasSupplementaryFile', 'sibim', 'SI Image'),
    array('si:generalImageCModel', 'hasSupplementaryFile', 'sigim', 'SI Gen Image'),
  );
}

function sidora_return_all_types() {
//array 0 is content model pid
//array 1 is rels ext
//array 2 is namespace
//array 3 is text title
  return array_merge(
      sidora_return_fba_concepts(), sidora_return_fba_resources(), sidora_return_fba_supplementary());
}

function sidora_get_cmodel_rels_pair() {
  $allcmodeldata = sidora_return_all_types();
  foreach ($allcmodeldata as $cmodeldatarow) {
    $retarray[] = array($cmodeldatarow[0], $cmodeldatarow[1]);
  }
  return $retarray;
}

function sidora_get_cmodel_namespace_pair() {
  $allcmodeldata = sidora_return_all_types();
  foreach ($allcmodeldata as $cmodeldatarow) {
    $retarray[] = array($cmodeldatarow[0], $cmodeldatarow[2]);
  }
  return $retarray;
}

function sidora_get_namespace_rel_pair() {
  $allcmodeldata = sidora_return_all_types();
  foreach ($allcmodeldata as $cmodeldatarow) {
    $retarray[] = array($cmodeldatarow[2], $cmodeldatarow[1]);
  }
  return $retarray;
}

function sidora_get_rel_from_pid($pid) {
  $pidprefix = substr($pid, 0, strpos($pid, ':'));
  $pairs = sidora_get_namespace_rel_pair();
  foreach ($pairs as $pair) {
    if (strcmp($pair[0], $pidprefix) === 0) {
      $resourcerdf = $pair[1];
      break;
    }
  }
  return $resourcerdf;
}

function sidora_get_cmodel_from_pid($pid) {
  $pidprefix = substr($pid, 0, strpos($pid, ':'));
  $prefix = sidora_get_system_prefix();
  if (strcmp($pidprefix, $prefix . '-user') === 0) {
    $resourcerdf = 'fba:peopleCModel'; //SFOX Nasty hack, change this
  }
  else {
    $pairs = sidora_get_cmodel_namespace_pair();
    foreach ($pairs as $pair) {
      if (strcmp($pair[1], $pidprefix) === 0) {
        $resourcerdf = $pair[0];
        break;
      }
    }
  }
  return $resourcerdf;
}

function sidora_fba_supp_media_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_supp_media_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_supp_gen_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_supp_gen_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_simulation_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_simulation_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_synthesis_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_synthesis_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
  // Check "from" date.
  $from = explode('/', $form_state['values']['date_filter']['date_filter_from']);
  $to = explode('/', $form_state['values']['date_filter']['date_filter_to']);
  if (!checkdate(intval($from[1]), intval($from[2]), intval($from[0]))) {
    //Can't use form_set_error as it's all javascript now
    form_set_error($form_key . '][date_filter_from', t('<em>From</em> date is not formatted correctly.'));
  }
}

function sidora_fba_measurement_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_measurement_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_dataset_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_dataset_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_litreview_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_litreview_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_analysis_submit(array $form, array &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_analysis_validate($form, &$form_state) {
  //$form_key = $form_state['triggering_element']['#form_key'];
}

function sidora_fba_activity_submit(array $form, array &$form_state) {
  //foreach ($form_state['groups'] as $group) {
  //  $status = skinr_context_group_import($group, TRUE) && $status;
  // }
  //drupal_set_message(t('The skin configuration groups have been saved.'));
  //if (!$status) {
  //    drupal_set_message(t('Not all skin configuration groups could be saved!'), 'error', FALSE);
  //  }
}

function sidora_fba_activity_validate(&$form, &$form_state) {
//$error_message = t('Abstract is empty!');
//if (empty($form_state['values']['abstract'])) {
// Error.
//    form_error($form['abstract'], $error_message);
//    return;
//}
}

function sidora_fba_vocabpref_submit(&$form, &$form_state) {
  
}

/**
 * Implements hook_xml_islandora_content_model_forms_get_transforms of the XML Form Builder module
 * This allows admins to pick these transforms when using the XML Form Builder
 */
function sidora_islandora_content_model_forms_get_transforms() {
  $p = drupal_get_path('module', 'sidora');
  $transforms = array();
  $include_path = "$p/transforms";
  $dir = opendir($include_path);
  if ($dir !== FALSE) {
    while (($file = readdir($dir)) !== FALSE) {
      if (preg_match('/\.xslt?$/', $file)) {
        $transforms["$include_path/$file"] = $file . " (sidora)";
        $transforms[$file] = $file . " (sidora; legacy location)";
      }
    }
  }
  closedir($dir);
  return $transforms;
}

function sidora_ingest_form_submit(array $form, array &$form_state) {
  
}

function sidora_form_submit(array $form, array &$form_state) {
  
}

function system_is_fba() {
  if (strcmp(sidora_get_system_prefix(), FBA_PREFIX) === 0) {
    $is_fba = TRUE;
  }
  else {
    $is_fba = FALSE;
  }
  return $is_fba;
}

function sidora_form_validate($form, &$form_state) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_collection');
  $pid = $form_state['values']['pid'];

  $objectHelper = new ObjectHelper();
  $contentModels = $objectHelper->get_content_models_list($pid);
  foreach ($contentModels as $contentModel) {
    if ($contentModel->pid == 'islandora:collectionCModel') {
      $member_pids = get_related_items_as_array($pid, 'isMemberOfCollection');
      if (is_array($member_pids) && !empty($member_pids)) {
        form_set_error('new_collection_pid', t("Please purge all members of this collection before deleting the collection itself."));
        return;
      }
    }
  }
}

function sidora_return_fba_extensions() {
  $file_extensions = array(
    'fba:activity_cmodel' => array('csv'),
    'fba:dataset_cmodel' => array('csv'),
    'fba:measurement_cmodel' => array('csv'),
    'fba:analysis_cmodel' => array('csv'),
    'fba:litreview_cmodel' => array('csv'),
    'fba:simulation_cmodel' => array('csv'),
    'fba:synthesis_cmodel' => array('csv'),
    'fba:supp_basic_image_cmodel' => array('jpg', 'jpeg', 'png', 'gif'),
    'fba:supp_pdf_cmodel' => array('pdf'),
    'fba:supp_text_cmodel' => array('txt'),
    'fba:supp_excel_cmodel' => array('xls', 'xlsx'),
    'fba:supp_word_cmodel' => array('doc', 'docx'),
    '*' => array('*'),
  );
  return $file_extensions;
}

function sidora_return_fba_resource_types() {
  $rescmodels = sidora_return_fba_resource_cmodels();
  $sidoramodels = array('si:imageCModel', 'si:generalImageCModel',);
  $resource_types = array(
    'all' => array_merge($rescmodels, $sidoramodels),
    'images' => array(
      'si:imageCModel', 'si:generalImageCModel',
      'fba:supp_basic_image_cmodel',),
    'pdf' => array(
      'fba:supp_pdf_cmodel',),
    'csv' => array(
      'fba:analysis_cmodel', 'fba:litreview_cmodel', 'fba:simulation_cmodel', 'fba:synthesis_cmodel',),
    'audio' => array('islandora:sp-audioCModel'),
    'video' => array('islandora:sp_videoCModel'),
    'text' => array('fba:supp_text_cmodel',),
  ); //SFOX need to add these 'fba:supp_word_cmodel','fba:supp_excel_cmodel',),
  return $resource_types;
}

function sidora_return_fba_ontology_defaults() {
  $returnvalue = '{
    "FBA Activity": {
        "description": "FBA Activity",
        "children": {},
        "model": "fba:activity_cmodel",
        "form": "FBA Activity",
        "ontology-id": "FBAActivityOntologyID"
    },
    "FBA Dataset": {
        "description": "FBA Dataset",
        "children": {},
        "model": "fba:dataset_cmodel",
        "form": "FBA Dataset", 
        "ontology-id": "FBADatasetOntologyID"
  }
  }';
  return $returnvalue;
}

function sidora_return_fba_resource_ontology_json_default() {
  $returnvalue = '{
        "FBA Synthesis": {
            "description": "This resource is an FBA Synthesis.",
            "children": {},
            "model": "fba:synthesis_cmodel",
            "form": "FBA Synthesis",
            "ontology-id": "FBASynthesisOntologyID"
        },
        "FBA Simulation": {
            "description": "This resource is an FBA Simulation.",
            "children": {},
            "model": "fba:simulation_cmodel",
            "form": "FBA Simulation",
            "ontology-id": "FBASimulationOntologyID"
        },
        "FBA Literature Review": {
            "description": "This resource is an FBA Literature Review.",
            "children": {},
            "model": "fba:litreview_cmodel",
            "form": "FBA Literature Review",
            "ontology-id": "FBALiteratureReviewOntologyID"
        },
        "FBA Measurement": {
            "description": "This resource is an FBA Measurement.",
            "children": {},
            "model": "fba:measurement_cmodel",
            "form": "FBA Measurement",
            "ontology-id": "FBAMeasurementOntologyID"
        },
        "FBA Analysis": {
            "description": "This resource is an FBA Analysis.",
            "children": {},
            "model": "fba:analysis_cmodel",
            "form": "FBA Analysis",
            "ontology-id": "FBAAnalysisOntologyID"
        },
        "FBA Supplementary Media": {
            "description": "This resource is an FBA Supplementary Media.",
            "children": {},
            "model": "fba:supp_basic_image_cmodel",
            "form": "FBA Supplementary Media",
            "ontology-id": "FBASupplementaryMediaOntologyID"
        },
        "FBA Supplementary Generic Text": {
            "description": "This resource is an FBA Supplementary Generic Text.",
            "children": {},
            "model": "fba:supp_text_cmodel",
            "form": "FBA Supplementary Generic Text",
            "ontology-id": "FBASupplementaryGenericTextOntologyID"
        },
        "FBA Supplementary Generic Word": {
            "description": "This resource is an FBA Supplementary Generic Word.",
            "children": {},
            "model": "fba:supp_text_cmodel",
            "form": "FBA Supplementary Generic Word",
            "ontology-id": "FBASupplementaryGenericWordOntologyID"
        },
        "FBA Supplementary Generic Excel": {
            "description": "This resource is an FBA Supplementary Generic Excel.",
            "children": {},
            "model": "fba:supp_text_cmodel",
            "form": "FBA Supplementary Generic Excel",
            "ontology-id": "FBASupplementaryGenericExcelOntologyID"
        },
        "FBA Supplementary Generic PDF": {
            "description": "This resource is an FBA Supplementary Generic PDF.",
            "children": {},
            "model": "fba:supp_text_cmodel",
            "form": "FBA Supplementary Generic PDF",
            "ontology-id": "FBASupplementaryGenericPDFOntologyID"
        },
        "Tabular data": {
        "description": "This resource is a Tabular data file.",
        "children": {},
        "model": "si:datasetCModel",
        "form": "Sidora Tabular File Form"
    },    
    "General Image": {
    "description": "This resource is a basic image.",
    "children": {},
    "model": "si:generalImageCModel",
    "form": "Sidora Basic Image Form"
    },
    "Image": {
    "description": "This resource is an image.",
    "children": {},
    "model": "si:imageCModel",
    "form": "FBA Supplementary Media"
    }}';
  return $returnvalue;
}

function sidora_fba_pre_submitter($form, &$form_state) {
  module_load_include('module', 'fba_sidora') . '/fba_sidora.module';
  if (array_key_exists('association', $form_state)) {
    //$association = sidora_find_association($form_state);
    //edit metadata comes into here
    $association = $form_state['association'];
    $contentmodelpid = $association['content_model'];
  }
  else {
    //This route is when adding a new term through eg the data.local site
    $contentmodelpid = $form_state['build_info']['args'][0]['models'][0];
  }
  //SFOX change the DTCA datastream to type X from type M??
  $cmodels = sidora_fba_return_vocab_cmodels();
  if (in_array($contentmodelpid, $cmodels)) { //for geo/vocab terms only
    if ($form_state['submitted']) {
      $button = $form_state['clicked_button']['#name']; //ingest or update
      switch ($button) {
        case 'ingest': {
            $editflag = FALSE;
            $pid = $form_state['build_info']['args'][0]['objects'][0]->id;
            break;
          }
        case 'update': {
            $editflag = TRUE;
            $pid = $form_state['build_info']['args'][0]->id;

            break;
          }
      }
      preprocess_links($form, $form_state, $editflag, $pid);         // object then subject relations to be created.
    }
  }
}

function sidora_fba_submitter($form, &$form_state) {
  if ($form_state['submitted']) {
    if (array_key_exists('association', $form_state)) {
      //$association = sidora_find_association($form_state);
      //edit metadata comes into here
      $association = $form_state['association'];
      $contentmodelpid = $association['content_model'];
    }
    else {
      //This route is when adding a new term through for example the data.local site
      $contentmodelpid = $form_state['build_info']['args'][0]['models'][0];
    }

    //Object Linker
    $label_field = '';
    if (array_key_exists('pid', $form_state)) {
      $pid = $form_state['pid']; //When editing metadata
    }
    else {
      $pid = $form_state['build_info']['args'][0]['objects'][0]->id; //When ingesting
    }
    //Now we have a choice of resources/concepts/vocabterms
    $resourcecmodels = sidora_return_fba_resource_cmodels();
    sidora_fba_ingest_update($contentmodelpid, $resourcecmodels, $form_state, $form, $pid, $label_field);

    $conceptcmodels = sidora_return_fba_concept_cmodels();
    sidora_fba_ingest_update($contentmodelpid, $conceptcmodels, $form_state, $form, $pid, $label_field);

    $vocabcmodels = sidora_fba_return_vocab_cmodels();
    sidora_fba_ingest_update($contentmodelpid, $vocabcmodels, $form_state, $form, $pid, $label_field);
    //SFOX combine the above 3???
    $button = $form_state['clicked_button']['#name']; //ingest or update
    switch ($button) {
      case 'ingest': {
          if (in_array($contentmodelpid, $resourcecmodels)) {
            submit_uploaded_files($form, $form_state);
          }
          $repo_object = islandora_object_load($pid);
          $user_id = sidora_get_user_object();
          $usercollectionns = str_replace(array('-', ':'), '', $user_id);
          $usercollection = $usercollectionns . ':collection';
          $repo_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $usercollection);
          /* DTCA datastream, archivalRecordCreationDate */
          $dtcads = $repo_object['DTCA']->content;
          $dtcasxml = simplexml_load_string($dtcads);
          $date_str = sprintf("%s %s", date("Y-m-d"), date("H:i:s"));
          $dtcasxml->archivalRecordCreationDate = $date_str;
          $dtcasxml->archivalPublicationVersion = 0;
          $repo_object['DTCA']->content = $dtcasxml->asXML();
          break;
        }
      case 'update': {
          break;
        }
    }
  }
  switch ($contentmodelpid) {
    case 'fba:dataset_cmodel': {
        sidora_fba_dataset_submit($form, $form_state);
        break;
      }
    case 'fba:activity_cmodel': {
        sidora_fba_activity_submit($form, $form_state);
        break;
      }
    case 'fba:analysis_cmodel': {
        resource_form_submit($form, $form_state);
        break;
      }
    case 'fba:measurement_cmodel': {
        sidora_fba_measurement_submit($form, $form_state);
        break;
      }
    case 'fba:litreview_cmodel': {
        sidora_fba_litreview_submit($form, $form_state);
        break;
      }
    case 'fba:simulation_cmodel': {
        sidora_fba_simulation_submit($form, $form_state);
        break;
      }
    case 'fba:synthesis_cmodel': {
        sidora_fba_synthesis_submit($form, $form_state);
        break;
      }
    case 'fba:supp_basic_image_cmodel': {
        sidora_fba_supp_media_submit($form, $form_state);
        break;
      }
    //case 'fba:supp_large_image_cmodel':
    case 'fba:supp_pdf_cmodel': {
        sidora_fba_supp_gen_submit($form, $form_state);
        break;
      }
    case 'fba:supp_text_cmodel': {
        sidora_fba_supp_gen_submit($form, $form_state);
        break;
      }
    case 'fba:supp_excel_cmodel': {
        sidora_fba_supp_gen_submit($form, $form_state);
        break;
      }
    case 'fba:supp_word_cmodel': {
        sidora_fba_supp_gen_submit($form, $form_state);
        break;
      }
    case 'fba:vocabpref_CModel': {
        sidora_fba_vocabpref_submit($form, $form_state);
        break;
      }
    default: {
        
      }
  }
}

function sidora_fba_ingest_update($contentmodelpid, $cmodels, &$form_state, $form, $pid, $label_field) {
  if (in_array($contentmodelpid, $cmodels)) {
    if ($form_state['submitted']) {
      $button = $form_state['clicked_button']['#name']; //ingest or update
      switch ($button) {
        case 'ingest': {
            module_load_include('module', 'fba_sidora') . '/fba_sidora.module';
            $ocrProcessingActivated = false; // This variable will not longer be required once the processing has been fully tested.
            $ocrProcessingRequired = true;
            /*
             * Create Object-Object links.
             */
            process_links($form, $form_state, false, $pid);
            //        }
            // The ocrProcessingActivated condition has been included below
            // to disable this functionality until it has been fully tested.
            if ($ocrProcessingRequired && $ocrProcessingActivated) {
              /*
               * Extract the text from the PDF in the temporary directory.
               * directory, add it as an OCR datastream and then
               * remove the temporary directory.
               */
              $pdfFilePath = '';
              $escapedPdfFile = '';
              $escapedPdfDirPath = '';
              $tmpDir = '';
              $tmpDirPath = '';
              $concatOutputFilePath = '';
              $ocrMsg = '';

              extractOcrFromPdf($pid, $pdfFilePath, $escapedPdfFile, $escapedPdfDirPath, $tmpDir, $tmpDirPath, $concatOutputFilePath, $ocrMsg);
            }
            $_SESSION['fedora_ingest_files'] = '';
            $form_state['storage'] = NULL;
            break;
          }
        case 'update': {

            process_links($form, $form_state, true, $pid);
            /*
              $dsid = $form_state['values']['dsid'];
              $form_name = $form_state['values']['forms'];
              $document = $this->modifyMetadata($pid, $dsid, $form, $form_state); //can a domdocument be passed in here?
              $transform = islandora_content_model_get_transform($contentmodelpid, $form_name);
              $this->transform($document, $transform, $pid, $label); */
            $form_state['storage'] = NULL;
            break;
          }
      }
    }
  }
}

function sidora_fba_validater(&$form, &$form_state) {

  $button = $form_state['clicked_button']['#value'];
  if (in_array($button, array('Add', 'Delete'))) {
    //SFOX hack, as adding or deleting a tab always comes through here
    return;
  }
  module_load_include('module', 'fba_sidora') . '/fba_sidora.module';

  if (array_key_exists('association', $form_state)) {
    //$association = sidora_find_association($form_state);
    //comes into here when editing metadata
    $association = $form_state['association'];
    $contentmodelpid = $association['content_model'];
  }
  else {
    //This route is when adding a new term through eg the data.local site
    $contentmodelpid = $form_state['build_info']['args'][0]['models'][0];
  }

  //Place our presubmit handler just before islandora's.
  $presubmitpos = array_search('islandora_ingest_form_submit', $form_state['submit_handlers']);
  array_splice($form_state['submit_handlers'], $presubmitpos, 0, 'sidora_fba_pre_submitter');
  $form_state['submit_handlers'][] = 'sidora_fba_submitter';

  //Object Linker for resources only.
  //Handle removal of appended pid's if required
  if ($form_state['submitted']) {
    $cmodels = array_merge(sidora_fba_return_vocab_cmodels(), sidora_return_fba_concept_cmodels(), sidora_return_fba_resource_cmodels());
    if (in_array($contentmodelpid, $cmodels)) {
      $xml_form = new XMLForm($form_state);
      $doc = $xml_form->submit($form, $form_state);
      $document = $doc->document;
      preprocess_link_data($document);
      $rdf_relationships = variable_get('rdf_relationships');
      foreach ($rdf_relationships as $rdf_relationship) {
        if ($rdf_relationship['object_pid_append_required']) {
          if (array_key_exists('rdf_link', $rdf_relationship)) {
            foreach ($rdf_relationship['rdf_link'] as $rindex => $rdf_link) {
              $finalval = $rdf_link['data'];
              if (strlen($finalval) > 0) {
                $fps = explode('/', $rdf_relationship['field_path']);
                $i = 0;
                foreach ($fps as $fp) {
                  if (strlen($fp) === 0) {
                    unset($fps[$i]); //remove any empty strings
                  }
                  if (strcmp($fp, '0') === 0) {
                    $fps[$i] = $rindex;
                  }
                  $i++;
                }
                setValue($form_state['values'], $fps, $finalval);
              }
            }
          }
        }
      }
    }
  }
  //End object linker

  switch ($contentmodelpid) {
    case 'fba:dataset_cmodel': {
        sidora_fba_dataset_validate($form, $form_state);
        break;
      }
    case 'fba:activity_cmodel': {
        sidora_fba_activity_validate($form, $form_state);
        break;
      }
    case 'fba:analysis_cmodel': {
        sidora_fba_analysis_validate($form, $form_state);
        break;
      }
    case 'fba:measurement_cmodel': {
        sidora_fba_measurement_validate($form, $form_state);
        break;
      }
    case 'fba:litreview_cmodel': {
        sidora_fba_litreview_validate($form, $form_state);
        break;
      }
    case 'fba:simulation_cmodel': {
        sidora_fba_simulation_validate($form, $form_state);
        break;
      }
    case 'fba:synthesis_cmodel': {
        sidora_fba_synthesis_validate($form, $form_state);
        break;
      }
    case 'fba:supp_basic_image_cmodel': {
        sidora_fba_supp_media_validate($form, $form_state);
        break;
      }
    //case 'fba:supp_large_image_cmodel':
    case 'fba:supp_pdf_cmodel': {
        sidora_fba_supp_gen_validate($form, $form_state);
        break;
      }
    case 'fba:supp_text_cmodel': {
        sidora_fba_supp_gen_validate($form, $form_state);
        break;
      }
    case 'fba:supp_excel_cmodel': {
        sidora_fba_supp_gen_validate($form, $form_state);
        break;
      }
    case 'fba:supp_word_cmodel': {
        sidora_fba_supp_gen_validate($form, $form_state);
        break;
      }
    default: {
        
      }
  }
}

function setValue(&$data, $path, $value) {
//from http://stackoverflow.com/questions/15483496/how-to-dynamically-set-value-in-multidimensional-array-by-reference
  $temp = &$data;
  foreach ($path as $key) {
    $temp = &$temp[$key];
  }
  $temp = $value;
  return $value;
}

function constructFbaTuqueObject($namespace) {
  $tuque = islandora_get_tuque_connection();
  $object = $tuque->repository->constructObject($namespace);
  $object->relationships->registerNamespace('fba', FBA_URI);
  return $object;
}

function appendFbaNamespace(&$islandorafedoraobject) {
  $islandorafedoraobject->relationships->registerNamespace('fba', FBA_URI);
}

<?php

function sidora_get_fba_rdf($parent_id, $child_id) {
  module_load_include('inc', 'islandora_autocomplete_sparql', 'includes/islandora_autocomplete_sparql.vocab');
  //fba:user-projects is a special case

  if (strcmp($parent_id, 'fba:user-projects') == 0) {
    $forwardrdf = 'hasUser';
    //$reverserdf = 'isMemberOfCollection';
  }
  else {
    $childns = substr($child_id, 0, strpos($child_id, ":"));
    $pairs = sidora_get_namespace_rel_pair();
    foreach ($pairs as $pair) {
      if (strcmp($pair[0], $childns) === 0) {
        $forwardrdf = $pair[1];
        break;
      }
    }

    if (is_null($forwardrdf)) {
      $forwardrdf = 'hasUndeterminedRelation'; //todo: change this to something else, and/or use it to catch problems later (when logged in as repomanager?)
    }
  }
  return $forwardrdf;
}

function sidora_get_namespace_from_cmodel($content_model_pid) {
  $pairs = sidora_get_cmodel_namespace_pair();
  foreach ($pairs as $pair) {
    if (strcmp($pair[0], $content_model_pid) === 0) {
      $pidnamespace = $pair[1];
      break;
    }
  }
  if (is_null($pidnamespace)) {
    $pidnamespace = 'dtcuc';
  }
  return $pidnamespace;
}

function sidora_is_user($pid) {
  $isuser = FALSE;
  $pidns = substr($pid, 0, strpos($pid, ':'));
  if (strcmp($pidns, 'fba-user') === 0) {
    $isuser = TRUE;
  }
  return $isuser;
}

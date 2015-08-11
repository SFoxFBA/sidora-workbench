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
function exhibition_config_admin_settings() {
  $epid_html = exhibition_config_get_exhibition_html_table_for_user('');

  $form = array(
    'si_exhibition_website_location' => array(
      '#type' => 'textfield',
      '#title' => t('The prefix for exhibition links'),
      '#default_value' => variable_get('si_exhibition_website_location', ''),
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
    'si_exhibition_friendly_website_location' => array(
      '#type' => 'textfield',
      '#title' => t('The prefix for exhibition links for the friendly urls'),
      '#default_value' => variable_get('si_exhibition_friendly_website_location', ''),
      '#description' => t('The friendly name gets directly appended.  This is usually "&lt;server&gt;/public/" with the right server.'),
    ),
    'si_exhibition_configuration_list' => array(
      '#type' => 'markup',
      '#title' => t('List the exhibitions'),
      '#markup' => t($epid_html),
    ),
  );
  return system_settings_form($form);
}
<?php
/**
 * @file
 * Defines a Musicbrainz API administration pages.
 */

/**
 * Implements hook_admin().
 */
function musicbrainz_api_admin() {
  $form['musicbrainz_api_cache_lifetime'] = [
    '#type' => 'textfield',
    '#weight' => 5,
    '#title' =>  t('Cache lifetime'),
    '#default_value' => variable_get('musicbrainz_api_cache_lifetime', 86400),
    '#description' => t('The cache lifetime is measured in seconds. 86400 is 24 hrs.')
  ];
  $form['musicbrainz_api_max_hits'] = [
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Max hits'),
    '#default_value' => variable_get('musicbrainz_api_max_hits', 20),
    '#description' => t('Number of values to be returned with any result.'),
  ];

  $form['musicbrainz_api_uri'] = [
    '#type' => 'textfield',
    '#weight' => 0,
    '#title' => t('Musicbrainz API URI'),
    '#default_value' => variable_get('musicbrainz_api_uri', 'http://musicbrainz.org/ws/2/'),
    '#description' => t('The path to the API. This should not change.'),
  ];

  $form['musicbrainz_api_client_config'] = [
    '#type' => 'fieldset',
    '#weight' => 20,
    '#title' => t('Musicbrainz API Client configuration'),
    '#description' => t('Application specific configuration of Client keys.'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => TRUE,
  ];

  $form['musicbrainz_api_client_config']['musicbrainz_api_application_name'] = [
    '#type' => 'textfield',
    '#weight' => 20,
    '#title' => t('Musicbrainz API Application Name'),
    '#default_value' => variable_get('musicbrainz_api_application_name', ''),
    '#description' => t('The name of the application, communicating with Musicbrainz.'),
  ];

  $form['musicbrainz_api_client_config']['musicbrainz_api_user'] = [
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('The Users email'),
    '#default_value' => variable_get('musicbrainz_api_user', ''),
    '#description' => t('Email of the user that is connecting to Musicbrainz.'),
  ];

  return system_settings_form($form);
}
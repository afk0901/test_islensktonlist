<?php
/**
 * @file
 * Defines a Discogs API administration pages.
 */

/**
 * Implements hook_admin().
 */
function discogs_api_admin() {
  $form['discogs_api_cache_lifetime'] = [
    '#type' => 'textfield',
    '#weight' => 5,
    '#title' =>  t('Cache lifetime'),
    '#default_value' => variable_get('discogs_api_cache_lifetime', 86400),
    '#description' => t('The cache lifetime is measured in seconds. 86400 is 24 hrs.')
  ];
  $form['discogs_api_max_hits'] = [
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Max hits'),
    '#default_value' => variable_get('discogs_api_max_hits', 20),
    '#description' => t('Number of values to be returned with any result.'),
  ];

  $form['discogs_api_uri'] = [
    '#type' => 'textfield',
    '#weight' => 0,
    '#title' => t('Discogs API URI'),
    '#default_value' => variable_get('discogs_api_uri', 'https://api.discogs.com/'),
    '#description' => t('The path to the API. This should not change.'),
  ];

  $form['discogs_api_client_config'] = [
    '#type' => 'fieldset',
    '#weight' => 20,
    '#title' => t('Discogs API Client configuration'),
    '#description' => t('Application specific configuration of Client keys.'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => TRUE,
  ];

  $form['discogs_api_client_config']['discogs_api_application_name'] = [
    '#type' => 'textfield',
    '#weight' => 20,
    '#title' => t('Discogs API Application Name'),
    '#default_value' => variable_get('discogs_api_application_name', ''),
    '#description' => t('The name of the application, communicating with Discogs.'),
  ];

  $form['discogs_api_client_config']['discogs_api_application_url'] = [
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('URL'),
    '#default_value' => variable_get('discogs_api_application_url', ''),
    '#description' => t('URL of the website connecting to Discogs.'),
  ];

  $form['discogs_api_client_config']['discogs_api_consumer_key'] = [
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('Consumer Key'),
    '#default_value' => variable_get('discogs_api_consumer_key', ''),
    '#description' => t('Discogs Consumer Key.'),
  ];

  $form['discogs_api_client_config']['discogs_api_consumer_secret'] = [
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('Consumer Secret'),
    '#default_value' => variable_get('discogs_api_consumer_secret', ''),
    '#description' => t('Discogs Consumer Secret.'),
  ];

  return system_settings_form($form);
}
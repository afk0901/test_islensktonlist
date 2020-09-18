<?php
/**
 * @file
 * Defines a Spotify API administration pages.
 */

/**
 * Implements hook_admin().
 */
function spotify_api_admin() {
  $form['spotify_api_cache_lifetime'] = [
    '#type' => 'textfield',
    '#weight' => 5,
    '#title' =>  t('Cache lifetime'),
    '#default_value' => variable_get('spotify_api_cache_lifetime', 86400),
    '#description' => t('The cache lifetime is measured in seconds. 86400 is 24 hrs.')
  ];
  $form['spotify_api_max_hits'] = [
    '#type' => 'textfield',
    '#weight' => 10,
    '#title' => t('Max hits'),
    '#default_value' => variable_get('spotify_api_max_hits', 20),
    '#description' => t('Number of values to be returned with any result.'),
  ];

  $form['spotify_api_uri'] = [
    '#type' => 'textfield',
    '#weight' => 0,
    '#title' => t('Spotify API URI'),
    '#default_value' => variable_get('spotify_api_uri', 'http://api.spotify.com/v1'),
    '#description' => t('The path to the API. This should not change.'),
  ];

  $form['spotify_api_client_config'] = [
    '#type' => 'fieldset',
    '#weight' => 20,
    '#title' => t('Spotify API Client configuration'),
    '#description' => t('Application specific configuration of Client keys.'),
    '#collapsible' => TRUE, // Added
    '#collapsed' => TRUE,
  ];

  $form['spotify_api_client_config']['spotify_api_client_id'] = [
    '#type' => 'textfield',
    '#weight' => 20,
    '#title' => t('Spotify API Client ID'),
    '#default_value' => variable_get('spotify_api_client_id', ''),
    '#description' => t('The client ID, assigned by Spotify, when you register your app.'),
  ];

  $form['spotify_api_client_config']['spotify_api_client_secret'] = [
    '#type' => 'textfield',
    '#weight' => 30,
    '#title' => t('Spotify API Client Secret'),
    '#default_value' => variable_get('spotify_api_client_secret', ''),
    '#description' => t('The client Secret key, assigned by Spotify, when you register your app.'),
  ];

  return system_settings_form($form);
}
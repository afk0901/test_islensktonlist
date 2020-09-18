<?php
/**
 * ARTIST TO BAND
 */
function ismus_admin_add_artist_to_band() {
  $args = arg();
  $artist = null;
  $band = null;

  if(isset($args[2])) {
    switch($args[2]) {
      case "listamadur" :
        $artist = $args[3];
        break;
      case "hljomsveit" :
        $band = $args[3];
    }
  }

  if(isset($args[4])) {
    switch($args[4]) {
      case "listamadur" :
        $artist = $args[5];
        break;
      case "hljomsveit" :
        $band = $args[5];
    }
  }

  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect artist to band') . '</h2>',
    ],
    'create_artist_to_band_form' => drupal_get_form('ismus_admin_add_artist_to_band_form'),
  ];

  $artist_markup = _ismus_display_create_artist_member_markup(node_load($artist));
  $artist_markup = (strlen($artist_markup) > 0)
    ? $artist_markup
    : "<h2>Listamaðurinn er ekki skráður í neina hljómsveit</h2>";
  if($artist) {
    $build['artist_info'] = [
      '#type' => 'markup',
      '#markup' => $artist_markup,
    ];
  }

  $band_markup = _ismus_display_create_artist_member_markup(node_load($band));
  $band_markup = (strlen($band_markup) > 0)
    ? $band_markup
    : "<h2>Hljómsveitin inniheldur enga meðlimi</h2>";
  if($band) {
    $build['band_info'] = [
      '#type' => 'markup',
      '#markup' => $band_markup,
    ];
  }

  return $build;
}

function ismus_admin_add_artist_to_band_form($form, &$form_state) {
  $args = arg();
  $artist = null;
  $band = null;
  $band_string = null;
  $artist_string = null;

  if(isset($args[2])) {
    switch($args[2]) {
      case "listamadur" :
        $artist = $args[3];
        $artist = node_load($artist);
        $artist_string = check_plain($artist->title) . " [$artist->nid]";
        break;
      case "hljomsveit" :
        $band = $args[3];
        $band = node_load($band);
        $band_string = check_plain($band->title) . " [$band->nid]";
        break;
    }
  }

  if(isset($args[4])) {
    switch($args[4]) {
      case "listamadur" :
        $artist = $args[5];
        $artist = node_load($artist);
        $artist_string = check_plain($artist->title) . " [$artist->nid]";
        break;
      case "hljomsveit" :
        $band = $args[5];
        $band = node_load($band);
        $band_string = check_plain($band->title) . " [$band->nid]";
        break;
    }
  }

  $form['artist'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The artist to be added to a band'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];
  if(isset($artist_string)){
    $form['artist'] += ['#default_value' => $artist_string];
  }

  $form['band'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Band'),
    '#description' => t('The band that the artist belongs.'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];
  if(isset($band_string)){
    $form['band'] += ['#default_value' => $band_string];
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_add_artist_to_band_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $artist = ismus_api_extract_string_and_id_from_autocomplete($input['artist']);
  $band = ismus_api_extract_string_and_id_from_autocomplete($input['band']);

  if($artist['id'] == $band['id']){
    drupal_set_message('Trying to connect an entity to itself!', 'error');
    return;
  }

  $artist_relations = ismus_api_get_all_band_relations_for_artist($artist['id']);
  foreach($artist_relations as $rel) {
    if($rel->endpoints[LANGUAGE_NONE][1]['entity_id'] == $band['id']){
      drupal_set_message('Relationship already exists', 'warning');
      return;
    }
  }

  $relation = relation_create('a_er_medlimur_i_b', [
    ['entity_type' => 'node', 'entity_id' => $artist['id']],
    ['entity_type' => 'node', 'entity_id' => $band['id']],
  ], NULL);

  $relation_id = relation_save($relation);
  drupal_goto('/relation/' . $relation_id . '/edit');
}
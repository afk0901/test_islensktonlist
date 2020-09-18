<?php
/**
 * PUBLISHER TO ALBUM
 */
function ismus_admin_connect_publisher_to_album_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect publisher to album') . '</h2>',
    ],
    'create_artist_to_song_form' => drupal_get_form('ismus_admin_connect_publisher_to_album_form'),
  ];

  return $build;
}

function ismus_admin_connect_publisher_to_album_form($form, &$form_state) {
  $args = arg();
  $artist_string = null;
  $relation_string = null;

  if(isset($args[2])){
    if($args[2] == 'verk_id'){
      $work_id = $args[3];
      $work = node_load($work_id);
      $work_string = check_plain($work->title) . " [$work->nid]";
      $form_state['storage']['referer_id'] = $work_id;
    }
  }
  $form['work'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Album'),
    '#description' => t('The album.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
  ];
  if(isset($work_string)){
    $form['work'] += ['#default_value' => $work_string];
  }

  $form['publisher'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Publisher'),
    '#description' => t('The artist that performs in the song.'),
    '#autocomplete_path' => 'utgefandi/unique_publisher_autocomplete_callback',
    '#value' => (isset($relation_string)) ? $relation_string : "",
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_publisher_to_album_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $work = ismus_api_extract_string_and_id_from_autocomplete($input['work']);
  $publisher = ismus_api_extract_string_and_id_from_autocomplete($input['publisher']);

  $relation = ismus_api_get_publisher_exists_in_album_relations($work['id'], $publisher['id']);
  if(is_array($relation)){
    drupal_set_message('Relationship already exists', 'warning');
    drupal_goto('/relation/' . array_values($relation)[0]->rid . "/edit");
  }
  else{
    $relation = relation_create('er_gefin_ut_af', [
      ['entity_type' => 'node', 'entity_id' => $work['id']],
      ['entity_type' => 'node', 'entity_id' => $publisher['id']],
    ], null);

    $relation_id = relation_save($relation);
    drupal_set_message(t('Relationship saved'));

    drupal_goto('/relation/' . $relation_id . "/edit");
  }
}
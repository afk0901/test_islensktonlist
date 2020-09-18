<?php
/**
 * SONG TO ALBUM
 */
function ismus_admin_connect_song_to_album_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect songs to an album') . '</h2>',
    ],
    'connect_song_to_album_form' => drupal_get_form('ismus_admin_connect_song_to_album_form'),
  ];

  return $build;
}

function ismus_admin_connect_song_to_album_form($form, &$form_state) {
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
    '#description' => t('The Album.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
  ];
  if(isset($work_string)){
    $form['work'] += ['#default_value' => $work_string];
  }

  $form['song'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Song'),
    '#description' => t('The song.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
    '#value' => (isset($relation_string)) ? $relation_string : "",
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_song_to_album_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $work = ismus_api_extract_string_and_id_from_autocomplete($input['work']);
  $song = ismus_api_extract_string_and_id_from_autocomplete($input['song']);

  $relation = ismus_api_add_song_to_album_relation($song['id'], $work['id']);

  drupal_goto('/relation/' . $relation->rid . "/edit");
}

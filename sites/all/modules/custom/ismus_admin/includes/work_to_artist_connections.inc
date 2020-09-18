<?php
/**
 * WORK TO ARTIST
 */
function ismus_admin_connect_work_to_artist_page(){
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect work to artist') . '</h2>',
    ],
    'create_work_to_artist_form' => drupal_get_form('ismus_admin_connect_work_to_artist_form'),
  ];

  return $build;
}

function ismus_admin_connect_work_to_artist_form($form, &$form_state){
  $args = arg();
  $work_string = null;

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
    '#title' => t('Work'),
    '#description' => t('The work.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
  ];
  if(isset($work_string)){
    $form['work'] += ['#default_value' => $work_string];
  }

  $form['artist'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The artist.'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_work_to_artist_form_submit($form, &$form_state){
  $input = $form_state['input'];
  $work = ismus_api_extract_string_and_id_from_autocomplete($input['work']);
  $artist = ismus_api_extract_string_and_id_from_autocomplete($input['artist']);

  $relation = ismus_api_get_artist_performs_work_relations($artist['id'], $work['id']);
  if(is_array($relation)){
    drupal_set_message('Relationship already exists', 'warning');
  }
  else{
    ismus_api_create_artist_performs_work_relation($artist['id'], $work['id']);

    if(isset($form_state['storage']['referer_id'])){
      drupal_goto('/node/' . $form_state['storage']['referer_id']);
    }
  }
}
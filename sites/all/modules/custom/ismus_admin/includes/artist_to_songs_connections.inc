<?php

function ismus_admin_artist_to_songs_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect individual to all songs on one album.') . '</h2>',
    ],
    'connect_artist_to_songs_form' => drupal_get_form('ismus_admin_connect_artist_to_songs_form'),
  ];

  return $build;
}

function ismus_admin_connect_artist_to_songs_form($form, &$form_state)  {
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

  $form['artist'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The artist that performs in the song.'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
    '#value' => (isset($relation_string)) ? $relation_string : "",
  ];

  $form['instruments'] = [
    '#type' => 'textfield',
    '#title' => t('Instrument'),
    '#description' => t('The instruments that the artist performs on, on the album'),
    '#maxlength'=> 400,
    '#autocomplete_path' => 'taxonomy/autocomplete/field_role',
  ];

  $form['all_album'] = [
    '#type' => 'checkbox',
    '#title' => 'Færa listamanninn á öll lögin'
  ];

  $work_relations = ismus_api_get_all_appearance_relations_for_work($work_id);
  $works = ismus_api_load_entities_from_relations($work_relations, $work_id);
  foreach($works['contains'] as $work) {
    /* @todo Copy the information from artist autocomplete down to here */
    $table_data[] = [
      '#tree' => TRUE,
      'add_song' => [
        '#type' => 'checkbox',
      ],
      'song_nid' => [
        '#type' => 'textfield',
        '#value' => $work['node']->nid,
        '#size' => 10,
        '#readonly' => 'readonly'
      ],
      'song_title' => [
        '#type' => 'markup',
        '#markup' => $work['node']->title,
      ],
      'artist_role' => [
        '#type' => 'textfield',
        '#title' => t('Instrument'),
        '#maxlength'=> 400,
        '#description' => t('The instruments that the artist performs on, on the album'),
        '#autocomplete_path' => 'taxonomy/autocomplete/field_role',
      ],
    ];
  }

  $form['table_markup'] = [
    '#type' => 'markup',
    '#markup' => '<h2>' . t('Pick the songs you want the artist to be added to') . '</h2>'
  ];

  $form['table'] = array(
    '#theme' => 'ismus_admin_form_table',
    '#header' => array('Velja', 'ID', 'Titill lags', 'Hljóðfæri'),
    'rows' => $table_data
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_artist_to_songs_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $work = ismus_api_extract_string_and_id_from_autocomplete($input['work']);
  $artist = ismus_api_extract_string_and_id_from_autocomplete($input['artist']);

  if($input['all_album']) {
    ismus_api_add_individual_artist_to_album_songs($work['id'], $artist['id'], $input['instruments']);
    drupal_goto('node/' . $work['id']);
  }
  else {
    $songs_to_add = [];
    foreach($input as $key => $value) {
      if(is_numeric($key)) {
        if($value['add_song'] && strlen($value['artist_role']) > 0) {
          $song = [];
          $song['song_id'] = $value['song_nid'];
          $song['artist_id'] = $artist['id'];
          $song['instruments'] = $value['artist_role'];
          $song['album_id'] = $work['id'];
          $songs_to_add[] = $song;
        }
      }
    }

    ismus_api_add_individual_artist_to_multiple_album_songs($songs_to_add);
    drupal_goto('node/' . $work['id']);
  }

}
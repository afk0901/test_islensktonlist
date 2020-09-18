<?php
/**
 * SONGS TO ALBUM
 */
function ismus_admin_connect_songs_to_album_page() {
  $spotify_data = null;
  $discogs_data = null;

  $arg = (int)arg(3);
  $album = node_load($arg);
  $spotify_id = (isset($album->field_spotify_id[LANGUAGE_NONE]))
    ? $album->field_spotify_id[LANGUAGE_NONE][0]['value']
    : null;
  $discogs_id = (isset($album->field_discogs_id[LANGUAGE_NONE]))
    ? $album->field_discogs_id[LANGUAGE_NONE][0]['value']
    : null;
  $build['header'] = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<p>' . t('Compare the data from the music providers to your data.  
      Then choose which data source you would like to use.  Spotify data source
      if prefered, since it has actual song information as well, to embed within
      the song.') . '</p>',
    ],
  ];

  if(!$spotify_id && !$discogs_id) {
    $build['no_ids'] = [
      'header_text' => [
        '#type' => 'markup',
        '#markup' => '<p><strong><em>ENGAR TÓNLISTARVEITUR ERU SKILGREINDAR FYRIR ÞESSA PLÖTU</em></strong>',
      ],
    ];
  }
  else {
    if($spotify_id) {
      $spotify_data = spotify_api_get_album($spotify_id);
    }
    if($discogs_id) {
      $discogs_data = discogs_api_get_release($discogs_id);
    }

    if($spotify_data || $discogs_data) {
      $build['album_data'] = [
        '#type' => 'markup',
        '#markup' => _ismus_display_create_tracklist_data_markup(
          $spotify_data, $discogs_data
        ),
      ];
    }

    $build['song_approval_form'] = [
      'header_text' => [
        '#type' => 'markup',
        '#markup' => '<h2>' . t('Approve songs') . '</h2>',
      ],
      'search_for_artist_form' => drupal_get_form('ismus_admin_song_approval_form'),
    ];
  }

  return $build;
}

function ismus_admin_song_approval_form($form, &$form_state) {
  $album_id = (int)arg(3);
  $album_and_artist_relation = ismus_api_get_performer_of_work($album_id);

  //If the album isn't connected to an artist, we can't continue.
  if(!$album_and_artist_relation) {
    drupal_set_message(t('Performer not connected, can\'t continue.'), 'error');
    drupal_goto('node/' . $album_id);
  }

  $album_and_artist_relation = array_values($album_and_artist_relation);
  $album_and_artist_relation_endpoints = relation_get_endpoints($album_and_artist_relation[0]);
  $album_and_artist = array_values($album_and_artist_relation_endpoints['node']);
  $album = $album_and_artist[1];
  $artist = $album_and_artist[0];


  $spotify_id = (isset($album->field_spotify_id[LANGUAGE_NONE]))
    ? $album->field_spotify_id[LANGUAGE_NONE][0]['value']
    : null;
  $discogs_id = (isset($album->field_discogs_id[LANGUAGE_NONE]))
    ? $album->field_discogs_id[LANGUAGE_NONE][0]['value']
    : null;

  $form['live'] = [
    '#type' => 'checkbox',
    '#value' => 'live',
    '#title' => 'Live album',
    '#description' => t('Check this box if this is a live album and all the song should be created, no matter if they exist in the DB or not'),
  ];

  $form['spotify'] = [
    '#type' => 'checkbox',
    '#value' => 'spotify',
    '#title' => 'Spotify',
    '#description' => t('Check this box if you would like Spotify data to be applied'),
  ];

  $form['discogs'] = [
    '#type' => 'checkbox',
    '#value' => 'discogs',
    '#title' => 'Discogs',
    '#description' => t('Check this box if you would like Discogs data to be applied'),
  ];

  $form['spotify_id'] = [
    '#type' => 'hidden',
    '#value' => $spotify_id,
  ];

  $form['discogs_id'] = [
    '#type' => 'hidden',
    '#value' => $discogs_id,
  ];

  $form['album_id'] = [
    '#type' => 'hidden',
    '#value' => $album->nid,
  ];

  $form['artist_id'] = [
    '#type' => 'hidden',
    '#value' => $artist->nid,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Submit'),
  ];

  return $form;
}

function ismus_admin_song_approval_form_submit($form, &$form_state) {
  $spotify_data = null;
  $discogs_data = null;

  $values = $form_state['input'];

  $live = (isset($values['live'])) ? true : false;

  if($values['spotify_id']) {
    $spotify_data = spotify_api_get_album($values['spotify_id']);
  }
  if($values['discogs_id']) {
    $discogs_data = discogs_api_get_release($values['discogs_id']);
  }

  if(isset($values['spotify'])) {
    ismus_api_add_song_to_album($spotify_data['tracks']['items'], $values['album_id'], $values['artist_id'], $live);
  }
  elseif(isset($values['discogs'])) {
    ismus_api_add_song_to_album($discogs_data['tracklist'], $values['album_id'], $values['artist_id'], $live, false);
  }
  else {
    drupal_set_message('No data was fetched from any music provider', 'warning');
    return;
  }
  
  drupal_goto('node/' . $values['album_id']);
}

function ismus_admin_song_approval_mock_spotify_object() {
  $array = [];
  $array[0]['disc_number'] = 1;
  $array[0]['duration_ms'] = 222261;
  $array[0]['id'] = "5V2WwbOXNmhUX6Y0GsmW9S";
  $array[0]['name'] = "Frímann flugkappi";
  $array[0]['preview_url'] = "https://p.scdn.co/mp3-preview/55cbeea24b792a10ba6cf33ea68c88473a6dba6b?cid=null";
  $array[0]['track_number'] = 1;

  $array[1]['disc_number'] = 1;
  $array[1]['duration_ms'] = 118003;
  $array[1]['id'] = "6RrzX0ZCcIhuw10iMzjP3D";
  $array[1]['name'] = "Ólína og ég";
  $array[1]['preview_url'] = "https://p.scdn.co/mp3-preview/4e2a315d98036bb14c9fafabfd3ec7b3d16ef567?cid=null";
  $array[1]['track_number'] = 2;

  $array[2]['disc_number'] = 1;
  $array[2]['duration_ms'] = 133328;
  $array[2]['id'] = "4fgy1jBATup4CI8zhnECY4";
  $array[2]['name'] = "Söngur fjallkonunnar";
  $array[2]['preview_url'] = "https://p.scdn.co/mp3-preview/7f43dbb470d18b6f59401768a5c4cc330d116d9f?cid=null";
  $array[2]['track_number'] = 3;

  $array[3]['disc_number'] = 1;
  $array[3]['duration_ms'] = 139969;
  $array[3]['id'] = "66ONb1sLGwejAZPSjxS5Om";
  $array[3]['name'] = "Hveitibjörn";
  $array[3]['preview_url'] = "https://p.scdn.co/mp3-preview/d67261210ea4f3e3e13f9a476ed90ed45b59e3e5?cid=null";
  $array[3]['track_number'] = 4;

  $array[4]['disc_number'] = 1;
  $array[4]['duration_ms'] = 185388;
  $array[4]['id'] = "3bRWrQGWyXq2Z89cuvD1zy";
  $array[4]['name'] = "Hr. Reykjavík";
  $array[4]['preview_url'] = "https://p.scdn.co/mp3-preview/2029976e075563830009dd11e0c270b44bd29203?cid=null";
  $array[4]['track_number'] = 5;

  return $array;
}

function ismus_admin_song_approval_mock_discogs_object() {
  $array = [];
  $array[0]['duration'] = "";
  $array[0]['position'] = "A1";
  $array[0]['title'] = "Frímann Flugkappi";

  $array[1]['duration'] = "";
  $array[1]['position'] = "A2";
  $array[1]['title'] = "Ólína Og Ég";

  $array[2]['duration'] = "";
  $array[2]['position'] = "A3";
  $array[2]['title'] = "Söngur Fjallkonunnar";

  $array[3]['duration'] = "";
  $array[3]['position'] = "A4";
  $array[3]['title'] = "Hveitibjörn";

  $array[4]['duration'] = "";
  $array[4]['position'] = "A5";
  $array[4]['title'] = "Hr. Reykjavík";

  $array[5]['duration'] = "";
  $array[5]['position'] = "A6";
  $array[5]['title'] = "Gullna Hliðið";

  $array[6]['duration'] = "";
  $array[6]['position'] = "A7";
  $array[6]['title'] = "Í Mýrinni";

  $array[7]['duration'] = "";
  $array[7]['position'] = "B1";
  $array[7]['title'] = "Á Skotbökkum";

  $array[9]['duration'] = "";
  $array[9]['position'] = "B1";
  $array[9]['title'] = "Draugaborgin";

  return $array;
}
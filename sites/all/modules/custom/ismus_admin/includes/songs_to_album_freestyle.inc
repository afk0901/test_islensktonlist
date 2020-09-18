<?php
/**
 * SONGS TO ALBUM - FREESTYLE
 */
function ismus_admin_songs_to_album_freestyle_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Pair up the performers and the songs.') . '</h2>',
    ],
    'connect_artist_to_songs_form' => drupal_get_form('ismus_admin_connect_songs_to_album_freestyle_form'),
  ];

  return $build;
}

function ismus_admin_connect_songs_to_album_freestyle_form($form, &$form_state) {
  $spotify_data = null;
  $discogs_data = null;
  $track_counter = 30;
  $tracks = null;

  $arg = (int)arg(3);
  $album = node_load($arg);
  $spotify_id = (isset($album->field_spotify_id[LANGUAGE_NONE]))
    ? $album->field_spotify_id[LANGUAGE_NONE][0]['value']
    : null;
  $discogs_id = (isset($album->field_discogs_id[LANGUAGE_NONE]))
    ? $album->field_discogs_id[LANGUAGE_NONE][0]['value']
    : null;

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
      $track_counter = sizeof($spotify_data['tracks']['items']);

      foreach($spotify_data['tracks']['items'] as $data) {
        $work = ismus_api_search_for_work($data['name'], null, true);
        if($work) {
          $work_relations = array_values(ismus_api_get_performer_of_work($work->nid))[0];
          $artist = node_load($work_relations->endpoints[LANGUAGE_NONE][0]['entity_id']);
          if($artist->title == $data['artists'][0]['name']) {
            $array = [
              'track_name' => $work->title,
              'track_id' => $work->nid,
              'artist_name' => $artist->title,
              'artist_id' => $artist->nid,
            ];
          }
        }
        else{
          $artist = ismus_api_search_for_artist($data['artists'][0]['name']);
          if($artist) {
            $artist = array_values($artist)[0];
            $array = [
              'artist_name' => $artist->title,
              'artist_id' => $artist->nid,
              'track_name' => $data['name'],
            ];
          }
          else {
            $array = [
              'artist_name' => $data['artists'][0]['name'],
              'track_name' => $data['name'],
            ];
          }
        }
        $array['spotify_id'] = $data['id'];
        $array['preview_url'] = $data['preview_url'];
        $array['duration_ms'] = $data['duration_ms'];
        $tracks[] = $array;
      }
    }
    if(($discogs_id) && (!$spotify_id)) {
      $discogs_data = discogs_api_get_release($discogs_id);
      $tracklist = $discogs_data['tracklist'];
      $track_counter = sizeof($tracklist);

      foreach($tracklist as $data) {
        $work = ismus_api_search_for_work($data['title'], null, true);

        if($work) {
          $work_relations = array_values(ismus_api_get_performer_of_work($work->nid))[0];
          $artist = node_load($work_relations->endpoints[LANGUAGE_NONE][0]['entity_id']);
          if(strtolower($artist->title) == strtolower($data['artists'][0]['name'])) {
            $tracks[] = [
              'track_name' => $work->title,
              'track_id' => $work->nid,
              'artist_name' => $artist->title,
              'artist_id' => $artist->nid,
            ];
          }
        }
        else{
          $artist = ismus_api_search_for_artist($data['artists'][0]['name']);
          if($artist) {
            $artist = array_values($artist)[0];
            $tracks[] = [
              'artist_name' => $artist->title,
              'artist_id' => $artist->nid,
              'track_name' => ismus_api_correct_case($data['title']),
            ];
          }
          else {
            $tracks[] = [
              'artist_name' => $data['artists'][0]['name'],
              'track_name' => ismus_api_correct_case($data['title']),
            ];
          }
        }
      }
    }
  }

  for($i = 0; $i < $track_counter; $i++) {
    $array = [
      '#tree' => TRUE,
      'track_number' => [
        '#type' => 'textfield',
        '#size' => 10,
        '#readonly' => 'readonly',
        '#value' => $i + 1,
      ],
      'artist_title' => [
        '#type' => 'textfield',
        '#required' => true,
        '#description' => t('The name of the artist.'),
        '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',

      ],
      'song_title' => [
        '#type' => 'textfield',
        '#required' => true,
        '#description' => t('The name of the artist.'),
        '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
      ],
    ];
    if(isset($tracks)) {
      if(is_array($tracks[$i])) {
        if(isset($tracks[$i]['artist_id'])) {
          $array['artist_title']['#value'] = $tracks[$i]['artist_name'] . " [" . $tracks[$i]['artist_id'] . "]";
        }
        elseif(isset($tracks[$i]['artist_name'])&& !(isset($tracks[$i]['artist_id']))) {
          $array['artist_title']['#value'] = $tracks[$i]['artist_name'];
        }
        if(isset($tracks[$i]['track_id'])) {
          $array['song_title']['#value'] = $tracks[$i]['track_name'] . " [" . $tracks[$i]['track_id'] . "]";
        }
        elseif(isset($tracks[$i]['track_name']) && !(isset($tracks[$i]['track_id']))) {
          $array['song_title']['#value'] = $tracks[$i]['track_name'];
        }
      }
      if(isset($tracks[$i]['spotify_id'])) {
        $array['spotify_id'] = [
          '#type' => 'textfield',
          '#required' => true,
          '#description' => t('Spotify ID.'),
          '#value' => $tracks[$i]['spotify_id']
        ];
        $array['preview_url'] = [
          '#type' => 'textfield',
          '#required' => true,
          '#description' => t('URL to the preview.'),
          '#value' => $tracks[$i]['preview_url']
        ];
        $array['duration'] = [
          '#type' => 'textfield',
          '#required' => true,
          '#description' => t('Duration of song (in second.'),
          '#value' => $tracks[$i]['duration_ms']
        ];
      }
    }

    $table_data[] = $array;
  }

  $form['album_id'] = [
    '#type' => 'hidden',
    '#value' => $album->nid,
  ];

  if(isset($tracks[0]['spotify_id'])) {
    $header = ['# lags', 'Flytjandi', 'Titill lags', 'Spotify #', 'Lagbútur', 'Lengd'];
  }
  else {
    $header = ['# lags', 'Flytjandi', 'Titill lags'];
  }

  $form['table'] = array(
    '#theme' => 'ismus_admin_form_table',
    '#header' => $header,
    'rows' => $table_data
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_songs_to_album_freestyle_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $album_id = $input['album_id'];
  $album = node_load($album_id);

  foreach($input as $key => $value) {
    if(is_numeric($key)) {
      $artist_data = ismus_api_extract_name_and_id_from_text($value['artist_title']);
      $track_data = ismus_api_extract_name_and_id_from_text($value['song_title']);

      //If the track has ID, the song exists in the database, with artist data,
      //so we only connect the song to the album.
      if(isset($track_data['id'])) {
        ismus_api_add_song_to_album_relation(
          $track_data['id'],
          $album_id,
          _ismus_api_generate_track_number(null, $value['track_number'], false)
        );
      }
      //If the artist has ID, he exists in the database, so we only create the song
      //and connect the artist to the song.
      elseif(isset($artist_data['id'])) {
        if(!isset($value['spotify_id'])) {
          $song = ismus_api_create_song(
            ['title' => $track_data['text']],
            $album,
            $value['track_number'],
            false
          );
        }
        else {
          $song = ismus_api_create_song(
            [
              'name' => $track_data['text'],
              'id' => $value['spotify_id'],
              'preview_url' => $value['preview_url'],
              'duration_ms' => $value['duration'],
              'track_number' => $value['track_number']
            ],
            $album,
            $value['track_number'],
            true
          );
        }
        
        ismus_api_create_artist_performs_work_relation($artist_data['id'], $song->nid);
      }
      //If we neither have artist ID nor song ID, we create both song and artist,
      //add the song to the album and add the artist to the song.
      else {
        if(!isset($value['spotify_id'])) {
          $song = ismus_api_create_song(
            ['title' => $track_data['text']],
            $album,
            $value['track_number'],
            false
          );
        }
        else {
          $song = ismus_api_create_song(
            [
              'name' => $track_data['text'],
              'id' => $value['spotify_id'],
              'preview_url' => $value['preview_url'],
              'duration_ms' => $value['duration'],
              'track_number' => $value['track_number']
            ],
            $album,
            $value['track_number'],
            true);
        }
        
        $artist = new stdClass();
        $artist->name = $artist_data['text'];
        $artist = ismus_api_create_artist($artist);
        ismus_api_create_artist_performs_work_relation($artist->nid, $song->nid);
      }
    }
  }
  drupal_goto('node/' . $album_id);
}
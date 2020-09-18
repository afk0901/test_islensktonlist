<?php
/**
 * Create artist page.
 *
 * Here we start creating the artist.  It will display first a search form, for
 * an existing artist, and then guide the user to create one.
 *
 * @return array
 */
function ismus_admin_edit_song_page(){
  drupal_add_js('sites/all/modules/custom/ismus_admin/ismus_admin.js');
  $arg = (int)arg(2);
  $song = node_load($arg);
  $performance_relation = array_values(ismus_api_get_performer_of_work($song->nid));
  $artist = node_load($performance_relation[0]->endpoints[LANGUAGE_NONE][0]['entity_id']);

  module_load_include('inc', 'node', 'node.pages');
  $build['form'] = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Edit song') . '</h2>',
    ],
    'song_edit_form' => drupal_get_form("work_node_form", $song),
  ];

  $spotify_song_search = null;
  $spotify_song_entity = null;
  $array = null;

  //If the Spotify field is set, we query Spotify with the ID to get detailed
  //information.  If not, we send a Spotify Search Query to try to find the
  //artist.
  //The same logic applies to Discogs and MusicBrainz
  if(isset($song->field_spotify_id[LANGUAGE_NONE])) {
    $spotify_song_entity =
      spotify_api_get_track($song->field_spotify_id[LANGUAGE_NONE][0]['value']);
  }
  else {
    $spotify_song_search = spotify_api_search_track($song->title, $artist->title);
  }

  if($spotify_song_search) {
    $build['spotify_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_spotify_song_markup($spotify_song_search),
    ];
  }

  if($spotify_song_entity) {
    $build['artist_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_song_data_markup($spotify_song_entity),
    ];
  }

  return $build;
}

function ismus_admin_search_song_info_from_music_providers($song_name, $artist_name) {
  $spotify_song_search = null;

  $array = null;

  $spotify_artist_search = spotify_api_search_track($song_name, $artist_name);

  if(!$spotify_song_search) {
    $array['spotify_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_spotify_artist_markup($spotify_artist_search),
    ];
  }

  return $array;
}

function ismus_admin_get_song_info_from_music_providers($array_of_ids) {
  $spotify_song_entity = null;
}

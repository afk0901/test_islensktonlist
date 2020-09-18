<?php
/**
 * Create artist page.
 *
 * Here we start creating the artist.  It will display first a search form, for
 * an existing artist, and then guide the user to create one.
 *
 * @return array
 */
function ismus_admin_edit_album_page(){
  drupal_add_js('sites/all/modules/custom/ismus_admin/ismus_admin.js');
  $arg = (int)arg(2);
  $album = node_load($arg);
  $performance_relation = array_values(ismus_api_get_performer_of_work($album->nid));
  $publisher_relation = ismus_api_get_publisher_of_work($album->nid);
  $artist = node_load($performance_relation[0]->endpoints[LANGUAGE_NONE][0]['entity_id']);

  module_load_include('inc', 'node', 'node.pages');
  $build['form'] = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Edit album') . '</h2>',
    ],
    'album_edit_form' => drupal_get_form("work_node_form", $album),
  ];
  if($publisher_relation) {
    $publisher_relation = array_values($publisher_relation);
    $build['relation'] = [
      'header_text' => [
        '#type' => 'markup',
        '#markup' => '<h2>' . t('Edit publisher') . '</h2>
      <p><a href="/relation/' . $publisher_relation[0]->rid . '/edit">Breyta útgefanda</a></p>',
      ],
    ];
  }
  else {
    $build['relation'] = [
      'header_text' => [
        '#type' => 'markup',
        '#markup' => '<h2>' . t('Add publisher') . '</h2>
      <p><a href="/verk/tengja-utgefanda-vid-hljomplotu/verk_id/' . $album->nid . '/edit">Bæta við útgefanda</a></p>',
      ],
    ];
  }

  $spotify_album_search = null;
  $discogs_album_search = null;

  $spotify_album_entity = null;
  $discogs_album_entity = null;
  $array = null;

  //If the Spotify field is set, we query Spotify with the ID to get detailed
  //information.  If not, we send a Spotify Search Query to try to find the
  //artist.
  //The same logic applies to Discogs and MusicBrainz
  if(isset($album->field_spotify_id[LANGUAGE_NONE])) {
    $spotify_album_entity =
      spotify_api_get_album($album->field_spotify_id[LANGUAGE_NONE][0]['value']);
  }
  else {
    $spotify_album_search = spotify_api_search_album($album->title, $artist->title);
  }
  if(isset($album->field_discogs_id[LANGUAGE_NONE])) {
    $discogs_album_entity =
      discogs_api_get_release($album->field_discogs_id[LANGUAGE_NONE][0]['value']);
  }
  else {
    $discogs_album_search = discogs_api_search_album($album->title, $artist->title);
  }

  if($spotify_album_search) {
    $build['spotify_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_spotify_album_markup($spotify_album_search),
    ];
  }

  if($discogs_album_search) {
    $build['discogs_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_discogs_album_markup($discogs_album_search),
    ];
  }

  if($spotify_album_entity || $discogs_album_entity) {
    $build['artist_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_album_data_markup(
        $spotify_album_entity, $discogs_album_entity
      ),
    ];
  }

  return $build;
}

function ismus_admin_search_album_info_from_music_providers($album_name, $artist_name) {
  $spotify_album_search = null;
  $discogs_album_search = null;
  $musicbrainz_album_search = null;
  $array = null;

  $spotify_artist_search = spotify_api_search_album($album_name, $artist_name);
  $discogs_album_search = discogs_api_search_album($album_name, $artist_name);
  $musicbrainz_album_search = musicbrainz_api_search_album($album_name, $artist_name);

  if(!$spotify_album_search) {
    $array['spotify_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_spotify_artist_markup($spotify_artist_search),
    ];
  }

  return $array;
}

function ismus_admin_get_album_info_from_music_providers($array_of_ids) {
  $spotify_album_entity = null;
  $discogs_album_entity = null;
  $musicbrainz_album_entity = null;

}


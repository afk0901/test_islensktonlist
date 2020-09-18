<?php
/**
 * Created by PhpStorm.
 * User: drupalviking
 * Date: 19/07/16
 * Time: 10:05
 */
/**
 * Implements hook_drush_command().
 */
function ismus_data_migration_drush_command() {
  $items['ismus-data-migration-migrate-artist'] = [
    'description' => dt('Migrates all artists into the Artist Content type'),
    'arguments' => ['artist_id' => 'The ID of the artist from the old database'],
    'examples' => [
      'Standard example' => 'drush ismus-data-migration-migrate-artist'
    ],
    'aliases' => ['migrate-artist'],
  ];

  $items['ismus-data-migration-migrate-work'] = [
    'description' => dt('Migrates all work into the Work Content type'),
    'arguments' => [],
    'aliases' => ['migrate-work']
  ];

  $items['ismus-data-migration-migrate-memberships'] = [
    'description' => dt('Migrates all memberships into the Relation entity type'),
    'arguments' => [],
    'aliases' => ['migrate-memberships']
  ];

  $items['ismus-data-migration-migrate-work-appearances'] = [
    'description' => dt('Migrates all appearances into the Relation entity type'),
    'arguments' => [],
    'aliases' => ['migrate-appearances']
  ];

  $items['ismus-data-migration-migrate-participations'] = [
    'description' => dt('Migrates all participations into the Relation entity type'),
    'arguments' => [],
    'aliases' => ['migrate-participations']
  ];

  $items['ismus-data-migration-debug'] = [
    'aliases' => ['ismus-debug'],
  ];

  $items['ismus-data-migration-migrate-roles-and-rolegroups'] = [
    'description' => dt('Migrates all memberships into the Relation entity type'),
    'arguments' => [],
    'aliases' => ['migrate-roles']
  ];

  $items['ismus-data-migration-convert-artist-dates'] = [
    'description' => dt('Converts artist born and died dates from timestamp to ISO'),
    'arguments' => [],
    'aliases' => ['convert-artist-dates']
  ];

  $items['ismus-data-migration-convert-publisher-dates'] = [
    'description' => dt('Converts publisher to and from dates from timestamp to ISO'),
    'arguments' => [],
    'aliases' => ['convert-publisher-dates']
  ];

  $items['ismus-data-migration-convert-band-member-dates'] = [
    'description' => dt('Converts band member to and from dates from timestamp to ISO'),
    'arguments' => [],
    'aliases' => ['convert-band-member-dates']
  ];

  $items['ismus-data-migration-convert-publishing-date'] = [
    'description' => dt('Converts publishing date from timestamp to ISO'),
    'arguments' => [],
    'aliases' => ['convert-publishing-date']
  ];

  return $items;
}

function drush_ismus_data_migration_convert_artist_dates() {
  $artists = _ismus_data_migration_fetch_all_artists();

  foreach($artists as $artist) {
    if(isset($artist->field_born_new[LANGUAGE_NONE])) {
      $artist->field_born[LANGUAGE_NONE][0]['value'] =
        $artist->field_born_new[LANGUAGE_NONE][0]['value'];
      node_save($artist);
      drush_print("Artist " . $artist->title . " has new birth date");
    }
    if(isset($artist->field_died_new[LANGUAGE_NONE])) {
      $artist->field_died[LANGUAGE_NONE][0]['value'] =
        $artist->field_died_new[LANGUAGE_NONE][0]['value'];
      node_save($artist);
      drush_print("Artist " . $artist->title . " has new died-date");
    }
  }
}

function drush_ismus_data_migration_convert_publisher_dates() {
  $publishers = _ismus_data_migration_fetch_all_publishers();
  foreach($publishers as $publisher) {
    if(isset($publisher->field_from_new[LANGUAGE_NONE])) {
      $publisher->field_from[LANGUAGE_NONE][0]['value'] =
        $publisher->field_from_new[LANGUAGE_NONE][0]['value'];
      node_save($publisher);
      drush_print("Publisher " . $publisher->title . " has new from date");
    }
    if(isset($publisher->field_to_new[LANGUAGE_NONE])) {
      $publisher->field_to[LANGUAGE_NONE][0]['value'] =
        $publisher->field_to_new[LANGUAGE_NONE][0]['value'];
      node_save($publisher);
      drush_print("Publisher " . $publisher->title . " has new to date");
    }
  }
}

function drush_ismus_data_migration_convert_publishing_date() {
  $publishing_relations = _ismus_data_migration_fetch_all_published_relations();
  foreach($publishing_relations as $publishing_relation) {
    if(isset($publishing_relation->field_date_of_publication_new[LANGUAGE_NONE])) {
      $publishing_relation->field_date_of_publication[LANGUAGE_NONE][0]['value'] =
        $publishing_relation->field_date_of_publication_new[LANGUAGE_NONE][0]['value'];
      relation_save($publishing_relation);
      if(isset($publishing_relation->field_production_number[LANGUAGE_NONE])) {
        drush_print("Relation with production numer " . $publishing_relation->field_production_number[LANGUAGE_NONE][0]['value'] . " has new from date");
      }
      else {
        drush_print("Relation number " .$publishing_relation->rid . " has been updated with new date");
      }
    }
  }
}

function drush_ismus_data_migration_convert_band_member_dates() {
  $band_members = ismus_data_migration_fetch_all_band_member_relations();
  foreach($band_members as $band_member) {
    if(isset($band_member->field_from_new[LANGUAGE_NONE])) {
      $band_member->field_from[LANGUAGE_NONE][0]['value'] =
        $band_member->field_from_new[LANGUAGE_NONE][0]['value'];
      relation_save($band_member);
      drush_print("Relation number " .$band_member->rid . " has been updated with new from date");
    }
    if(isset($band_member->field_to_new[LANGUAGE_NONE])) {
      $band_member->field_to[LANGUAGE_NONE][0]['value'] =
        $band_member->field_to_new[LANGUAGE_NONE][0]['value'];
      relation_save($band_member);
      drush_print("Relation number " .$band_member->rid . " has been updated with new from date");
    }
  }
}

/**
 *
 */
function drush_ismus_data_migration_migrate_artist($artist_id = null){
  $old_artists = ($artist_id)
    ? _ismus_data_migration_fetch_old_artist_by_id($artist_id)
    : _ismus_data_migration_fetch_all_old_artists();

  foreach($old_artists as $artist){
    $artist_spotify_info = spotify_lookup_api_search_artist($artist->name);
    $artist_genre = _ismus_data_migration_find_artist_genre($artist->id);
    ismus_data_migration_add_artist($artist, $artist_spotify_info, $artist_genre);
  }
}

/**
 * Migrates all work from the old database.  If we haven't already migrated the
 * work, we will do so.
 *
 * It either accepts a work_id parameter or not.  If we supply the parameter,
 * only that particular id is migrated.  If no parameter is passed, we migrate
 * all work
 *
 * @param null|int $work_id
 */
function drush_ismus_data_migration_migrate_work($work_id = null){
  $old_work = ($work_id)
    ? _ismus_data_migration_fetch_old_work_by_id($work_id)
    : _ismus_data_migration_fetch_all_old_work();

  foreach($old_work as $work){
    //If it is an album, get the album information
    if($work->work_type_id == 1){
      $album_spotify_info = spotify_lookup_api_search_album($work->name);
      $album_genre = _ismus_data_migration_find_album_genre($work->id);

      $album = ismus_data_migration_add_album($work, $album_spotify_info, $album_genre);
      ismus_data_migration_add_publishing_data($work->id, $album);
    }
    elseif($work->work_type_id = 2){
      $song_spotify_info = spotify_lookup_api_search_track($work->name);
      ismus_data_migration_add_track($work, $song_spotify_info);
    }
  }
}

/**
 * Migrates all memberships from the old database.  If we haven't already
 * migrated the artist, we will do so.
 *
 * After we have found both artists in the database, we create the relationship
 * between the two.
 */
function drush_ismus_data_migration_migrate_memberships(){
  $memberships = _ismus_data_migration_fetch_all_artist_memberships();

  foreach($memberships as $membership){
    $artist = _ismus_data_migration_find_artist_by_old_id($membership->artist_id);
    if(!$artist){
      drush_ismus_data_migration_migrate_artist($membership->artist_id);
      $artist = _ismus_data_migration_find_artist_by_old_id($membership->artist_id);
    }
    $group = _ismus_data_migration_find_artist_by_old_id($membership->group_id);
    if(!$group){
      drush_ismus_data_migration_migrate_artist($membership->group_id);
      $group = _ismus_data_migration_find_artist_by_old_id($membership->group_id);
    }

    $membership_roles =
      _ismus_data_migration_fetch_all_artist_membership_roles(
        $membership->artist_id, $membership->group_id
      );

    $relation = relation_create('a_er_medlimur_i_b', [
      ['entity_type' => 'node', 'entity_id' => $artist->nid],
      ['entity_type' => 'node', 'entity_id' => $group->nid]
    ], null);
    $relation_id = relation_save($relation);
    $relation = relation_load($relation_id);
    if(isset($membership->from_date)){
      $relation->field_from[LANGUAGE_NONE][0]['value'] = strtotime($membership->from_date);
      $relation->field_from[LANGUAGE_NONE][0]['timezone'] = 'Atlantic/Reykjavik';
      $relation->field_from[LANGUAGE_NONE][0]['timezone_db'] = 'Atlantic/Reykjavik';
      $relation->field_from[LANGUAGE_NONE][0]['date_type'] = 'datestamp';
    }
    if(isset($membership->to_date)){
      $relation->field_to[LANGUAGE_NONE][0]['value'] = strtotime($membership->to_date);
      $relation->field_to[LANGUAGE_NONE][0]['timezone'] = 'Atlantic/Reykjavik';
      $relation->field_to[LANGUAGE_NONE][0]['timezone_db'] = 'Atlantic/Reykjavik';
      $relation->field_to[LANGUAGE_NONE][0]['date_type'] = 'datestamp';
    }
    if(isset($membership->rank)){
      $relation->field_rank[LANGUAGE_NONE][0]['value'] = $membership->rank;
    }
    if(isset($membership->original)){
      $relation->field_is_original[LANGUAGE_NONE][0]['value'] = $membership->original;
    }
    if($membership_roles){
      $index = 0;
      foreach($membership_roles as $role){
        $tax = taxonomy_get_term_by_name($role->role_name, 'artist_role');
        $tax = array_values($tax);
        $relation->field_role[LANGUAGE_NONE][$index++]['tid'] = $tax[0]->tid;
      }
    }

    relation_save($relation);
  }
}

function drush_ismus_data_migration_migrate_participations(){
  $participations = _ismus_data_migration_create_participation_array();

  foreach($participations as $artist_id => $works){
    foreach($works as $work_id){
      $artist = _ismus_data_migration_find_artist_by_old_id($artist_id);
      if(!$artist){
        drush_ismus_data_migration_migrate_artist($artist_id);
        $artist = _ismus_data_migration_find_artist_by_old_id($artist_id);
      }
      $song = _ismus_data_migration_find_work_by_old_id($work_id);
      if(!$song){
        drush_ismus_data_migration_migrate_work($work_id);
        $song = _ismus_data_migration_find_work_by_old_id($work_id);
      }

      $relation = relation_create('leikur_i', [
        ['entity_type' => 'node', 'entity_id' => $artist->nid],
        ['entity_type' => 'node', 'entity_id' => $song->nid]
      ], null);

      $relation_id = relation_save($relation);
      $relation = relation_load($relation_id);

      $roles = _ismus_data_migration_find_participation_by_artist_id_and_work_id($artist_id, $work_id);
      if($roles){
        $index = 0;
        foreach($roles as $role){
          $tax = taxonomy_get_term_by_name($role->role_name, 'artist_role');
          $tax = array_values($tax);
          $relation->field_role[LANGUAGE_NONE][$index++]['tid'] = $tax[0]->tid;
        }
      }

      relation_save($relation);
    }
  }
}

function drush_ismus_data_migration_migrate_work_appearances(){
  $appearances = _ismus_data_migration_fetch_all_appearances();

  foreach($appearances as $appearance){
    $song = _ismus_data_migration_find_work_by_old_id($appearance->item_id);
    if(!$song){
      drush_ismus_data_migration_migrate_work($appearance->item_id);
      $song = _ismus_data_migration_find_work_by_old_id($appearance->item_id);
    }
    $album = _ismus_data_migration_find_work_by_old_id($appearance->work_id);
    if(!$album){
      drush_ismus_data_migration_migrate_work($appearance->work_id);
      $album = _ismus_data_migration_find_work_by_old_id($appearance->work_id);
    }

    $relation = relation_create('kemur_fyrir_a', [
      ['entity_type' => 'node', 'entity_id' => $song->nid],
      ['entity_type' => 'node', 'entity_id' => $album->nid]
    ], null);
    $relation_id = relation_save($relation);
    $relation = relation_load($relation_id);

    if(isset($appearance->rank)){
      $relation->field_rank[LANGUAGE_NONE][0]['value'] = $appearance->rank;
    }

    $relation->field_apperance_type[LANGUAGE_NONE][0]['tid'] = 155;

    relation_save($relation);
  }
}

function drush_ismus_data_migration_debug(){
  //print_r(taxonomy_get_term_by_name('CD', 'media_type'));
  _ismus_data_migration_create_participation_array();
}

function drush_ismus_data_migration_migrate_roles_and_rolegroups(){
  $data = _ismus_data_migration_fetch_all_roles_and_rolegroups();
  $arr = [];
  $vocabulary = taxonomy_vocabulary_machine_name_load('artist_role');

  foreach($data as $role){
    $arr[$role->rolegroup_name][] = [
      'role_name' => $role->role_name,
      'artist_role_type' => $role->artisttype,
    ];
  }

  foreach($arr as $key => $value){
    $term = new stdClass();
    $term->name = $key;
    $term->vid = $vocabulary->vid;
    taxonomy_term_save($term);

    foreach($value as $val){
      $subterm = new stdClass();
      $subterm->name = $val['role_name'];
      $subterm->field_artist_role_type[LANGUAGE_NONE][0]['value'] = $val['artist_role_type'];
      $subterm->vid = $vocabulary->vid;
      $subterm->parent = $term->tid;
      taxonomy_term_save($subterm);
    }
  }
}
<?php
/**
 * MEMBERS OF AN ARTIST TO SONGS
 */
function ismus_admin_connect_artist_members_to_songs_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Copy members of the performing artist to all the songs on the album') . '</h2>',
    ],
    'connect_song_to_album_form' => drupal_get_form('ismus_admin_connect_artist_members_to_songs_form'),
  ];

  return $build;
}

function ismus_admin_connect_artist_members_to_songs_form($form, &$form_state) {

  //Step two of the form
  if(isset($form_state['storage']) && $form_state['storage']['page_two']) {
    $artist_id = $form_state['storage']['artist_id'];
    $album_id = $form_state['storage']['album_id'];
    $table_data = [];

    $artist_members_relations = ismus_api_get_all_band_relations_for_artist($artist_id);

    foreach($artist_members_relations as $amr) {
      $artist = node_load($amr->endpoints[LANGUAGE_NONE][0]['entity_id']);
      $role_string = "";
      foreach($amr->field_role[LANGUAGE_NONE] as $role) {
        $role_string .= taxonomy_term_load($role['tid'])->name . ",";
      }
      $role_string = substr($role_string, 0, -1);

      $table_data[] = [
        '#tree' => TRUE,
        'add_artist' => [
          '#type' => 'checkbox',
        ],
        'artist_nid' => [
          '#type' => 'textfield',
          '#value' => $artist->nid,
          '#size' => 10,
        ],
        'relationship_rid' => [
          '#type' => 'textfield',
          '#value' => $amr->rid,
          '#size' => 10,
        ],
        'artist_name' => [
          '#type' => 'markup',
          '#markup' => '<p>' . $artist->title . '</p>',
        ],
        'artist_role' => [
          '#type' => 'textfield',
          '#required' => true,
          '#title' => t('Instrument'),
          '#description' => t('The instruments that the artist performs on, on the album'),
          '#autocomplete_path' => 'taxonomy/autocomplete/field_role',
          '#value' => $role_string,
        ],
      ];
    }

    $form['album_id'] = [
      '#type' => 'hidden',
      '#value' => $album_id,
    ];

    $form['artist_id'] = [
      '#type' => 'hidden',
      '#value' => $artist->nid,
    ];

    $form['table'] = array(
      '#theme' => 'ismus_admin_form_table',
      '#header' => array('Velja', 'ID', 'RID', 'Listamaður', 'Hljóðfæri'),
      'rows' => $table_data
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Færa valda hljóðfæraleikara yfir á lög plötunnar.'),
    );

    return $form;
  }
  //Step one
  $album_id = (int)arg(3);
  $album_and_artist_relation = ismus_api_get_performer_of_work($album_id);

  $album_and_artist_relation = array_values($album_and_artist_relation);
  $album_and_artist_relation_endpoints = relation_get_endpoints($album_and_artist_relation[0]);
  $album_and_artist = array_values($album_and_artist_relation_endpoints['node']);

  $artist = $album_and_artist[0];

  $form['artist_name'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The name of the artist (band, group or individual)'),
    '#default_value' => check_plain($artist->title) . " [$artist->nid]",
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];

  $form['album_id'] = [
    '#type' => 'hidden',
    '#value' => $album_id,
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

function ismus_admin_connect_artist_members_to_songs_form_submit($form, &$form_state) {
  //@TODO : Change this code to work with two step form
  //This is step 1 of the process.  Searching for duplicates in the database
  //and passing it to step 2.
  if($form_state['clicked_button']['#value'] == 'Next') {
    $values = $form_state['values'];
    $artist = ismus_api_extract_string_and_id_from_autocomplete($values['artist_name']);
    $form_state['storage']['page_two'] = TRUE;
    $form_state['storage']['artist_id'] = (int)$artist['id'];
    $form_state['storage']['album_id'] = $values['album_id'];
    $form_state["rebuild"] = TRUE;
  }
  else if($form_state['storage']['page_two']) {
    //Handle page two submission
    $values = $form_state['input'];
    $album_id = $form_state['storage']['album_id'];
    foreach($values as $key => $value) {
      if(is_numeric($key)) {
        if($value['add_artist']) {
          ismus_api_add_artist_to_album_songs_with_relation(
            $album_id, $value['artist_nid'], $value['artist_role']
          );
        }
      }
    }

    drupal_goto('node/' . $album_id);
  }
}
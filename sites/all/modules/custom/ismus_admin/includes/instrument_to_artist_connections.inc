<?php
/**
 * INSTRUMENT TO ARTIST
 */
function ismus_admin_connect_instrument_to_artist_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect instrument to artist') . '</h2>',
    ],
    'create_instrument_to_artist_form' => drupal_get_form('ismus_admin_connect_instrument_to_artist_form'),
  ];

  return $build;
}

function ismus_admin_connect_instrument_to_artist_form($form, &$form_state) {
  $args = arg();
  $artist_string = null;
  $relation_string = null;

  if(isset($args[2])){
    if($args[2] == 'listamadur_id'){
      $artist_id = $args[3];
      $artist = node_load($artist_id);
      $artist_string = check_plain($artist->title) . " [$artist->nid]";
      $form_state['storage']['referer_id'] = $artist_id;

      $relations = ismus_api_get_all_instruments_for_artist($artist_id);
      if($relations) {
        foreach($relations as $rel) {
          $relation_array[] = taxonomy_term_load($rel->endpoints[LANGUAGE_NONE][1]['entity_id'])->name;
        }
        $relation_string = implode(', ', $relation_array);
      }
    }
  }
  $form['artist'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The artist.'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];
  if(isset($artist_string)){
    $form['artist'] += ['#default_value' => $artist_string];
  }

  $form['instrument'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Instrument'),
    '#description' => t('The instrument.'),
    '#autocomplete_path' => 'taxonomy/autocomplete/field_role',
    '#value' => (isset($relation_string)) ? $relation_string : "",
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_instrument_to_artist_form_submit($form, &$form_state) {
  $input = $form_state['input'];
  $tags_array = explode(',', $input['instrument']);
  $vocabulary = taxonomy_vocabulary_machine_name_load('artist_role');
  $artist_id = ismus_api_extract_id_from_brackets($input['artist']);

  foreach($tags_array as $tag) {
    $tax = _ismus_api_search_taxonomy_term($vocabulary->vid, trim($tag));
    if($tax) {
      //Term is available, lets check if it's already connected....
      $relations = ismus_api_get_all_instruments_for_artist($artist_id, $tax->tid);
      if(!$relations) {
        //Add the relation
        $relation = relation_create('leikur_a', [
          ['entity_type' => 'node', 'entity_id' => $artist_id],
          ['entity_type' => 'taxonomy_term', 'entity_id' => $tax->tid]
        ], null);

        relation_save($relation);
      }
    }
  }

  drupal_goto('node/' . $artist_id);
}
<?php
/**
 * CREATE ALBUM
 */

/**
 * Create album page.
 *
 * Here we start creating the album.  It will display first a search form, for
 * an existing albums, and then guide the user to create one.
 *
 * @return array
 */
function ismus_admin_create_album_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Create album') . '</h2>',
    ],
    'search_for_album_form' => drupal_get_form('ismus_admin_search_for_album_form'),
  ];

  return $build;
}

function ismus_admin_search_for_album_form($form, &$form_state) {
  //This is step two of the form
  if(isset($form_state['storage']) && $form_state['storage']['page_two']){
    $artist_array = ismus_api_extract_name_and_id_from_text($form_state['storage']['page_one_values']['album_artist']);
    if(sizeof($form_state['storage']['drupal_album_array']) > 0){
      $drupal_albums = theme('table', ['rows' => $form_state['storage']['drupal_album_array']]);
      $form['drupal_albumss'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi hljómplötur fundust í kerfinu</h2>' . $drupal_albums . "<p>Ef listamaðurinn sem þú ert að stofna er í listanum, skaltu ýta <a href='/'>hér </a> til að hætta við</p>",
      ];
    }
    if(sizeof($form_state['storage']['old_db_album_array']) > 0){
      $form['old_db_album_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi hljómplötur fundust í eldri gagnagrunni</h2>',
      ];
      $form['old_db_albums'] = [
        '#type' => 'radios',
        '#options' => $form_state['storage']['old_db_album_array']
      ];
      $form['old_db_album_disclaimer'] = [
        '#type' => 'markup',
        '#markup' => '<p>Ef hljómplatan sem þú ert að stofna er í þessum lista, skaltu velja hann úr listanum.  Þar með færast allar tengingar við hann úr eldri gagnagrunni inn í þann nýja.</p>',
      ];
    }

    $artist_name = "";
    if(isset($artist_array['id'])) {
      $artist_name = ' með listamanninum <strong>' . $artist_array['text'] . '</strong>';
      $form['album_name_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Hljómplatan sem þú ert að stofna</h2>' . '<p><strong>' . $form_state['storage']['page_one_values']['album_title'] . '</strong>' . $artist_name . '</p>',
      ];
    }
    else{
      $form['album_name_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Hljómplatan sem þú ert að stofna</h2>' .
          '<p><strong>' . $form_state['storage']['page_one_values']['album_title']  . '</strong></p>' .
          '<p>Listamaðurinn <strong>' . $artist_array['text'] . '</strong> verður einnig stofnaður</p>',
      ];
    }


    $form['album_title'] = [
      '#type' => 'hidden',
      '#value' => $form_state['storage']['page_one_values']['album_title'],
    ];

    if(isset($artist_array['id'])) {
      $form['artist_id'] = [
        '#type' => 'hidden',
        '#value' => $artist_array['id'],
      ];
    }

    if(isset($artist_array['text'])) {
      $form['artist_name'] = [
        '#type' => 'hidden',
        '#value' => $artist_array['text'],
      ];
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );

    return $form;
  }
  //This is step 3 of the form
  elseif(isset($form_state['storage']) && $form_state['storage']['page_three']){
    //return ismus_admin_create_album_form($form, $form_state);
  }

  //Original state (step 1) of the form
  $form['album_title'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Album'),
    '#description' => t('The name of the album')
  ];

  $form['album_artist'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The name of the artist.'),
    '#autocomplete_path' => 'lag/unique_artist_autocomplete_callback',
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

function ismus_admin_search_for_album_form_submit($form, &$form_state) {
  //This is step 1 of the process.  Searching for duplicates in the database
  //and passing it to step 2.
  if($form_state['clicked_button']['#value'] == 'Next'){
    $values = $form_state['values'];

    $album_title = $values['album_title'];

    $album_search_results = ismus_api_search_for_work($album_title, HLJOMPLATA);
    $exclude_array = null;
    $drupal_album_array = [];
    if($album_search_results) {
      foreach($album_search_results as $asr){
        //Get the old id from the album, so we can exclude those, when searching
        //the old database
        if(isset($asr->field_original_id[LANGUAGE_NONE])){
          $exclude_array[] = $asr->field_original_id[LANGUAGE_NONE][0]['value'];
        }

        //Then add the user to an array, so we can display the search results and
        //ask if the album is already in the database
        $avatar_img_atom = scald_atom_load($asr->field_avatar[LANGUAGE_NONE][0]['sid']);
        $avatar_image_path = image_style_url('thumbnail', $avatar_img_atom->file_source);
        $avatar_image = "<img src='{$avatar_image_path}' />";

        $drupal_album_array[] = [
          $avatar_image,
          l($asr->title, 'node/' . $asr->nid),
        ];
      }
    }

    $old_db_album_search_results = ismus_api_search_for_work_in_old_db($album_title, $exclude_array);

    $old_db_album_array = [];
    if($old_db_album_search_results) {
      foreach($old_db_album_search_results as $odasr){
        $old_db_album_array[$odasr->id] = $odasr->name . " - " . $odasr->artist_name . " - " . $odasr->work_type;
      }
    }

    $form_state['storage']['page_two'] = TRUE;
    $form_state['storage']['page_three'] = FALSE;
    $form_state['storage']['page_one_values'] = $values;
    $form_state['storage']['drupal_album_array'] = $drupal_album_array;
    $form_state['storage']['old_db_album_array'] = $old_db_album_array;
    $form_state["rebuild"] = TRUE;
  }
  //Step 2.  Manly just storing the input in the state variable, so we can
  //pass them along.  Also the rebuild parameter has to be set to true, so we
  //continue and do not start the first part of the form again
  else if($form_state['storage']['page_two']){
    //Handle page two submission
    $values = $form_state['values'];

    $object = new stdClass();
    $object->name = $values['album_title'];
    if(isset($values['old_db_albums'])){
      $object->old_id = $values['old_db_albums'];
    }

    $album = ismus_api_create_work($object);
    if(isset($values['artist_id'])) {
      ismus_api_create_artist_performs_work_relation($values['artist_id'], $album->nid);
    }
    elseif(isset($values['artist_name'])) {
      $artist_obj = new stdClass();
      $artist_obj->name = $values['artist_name'];
      $artist = ismus_api_create_artist($artist_obj);
      ismus_api_create_artist_performs_work_relation($artist->nid, $album->nid);
    }
    drupal_goto('node/' . $album->nid);
  }
}
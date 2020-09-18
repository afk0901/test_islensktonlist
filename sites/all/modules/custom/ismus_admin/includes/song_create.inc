<?php
/**
 * CREATE SONG
 */

/**
 * Create song page.
 *
 * Here we start creating the song.  It will display first a search form, for
 * an existing song, and then guide the user to create one.
 *
 * @return array
 */
function ismus_admin_create_song_page() {
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Create song') . '</h2>',
    ],
    'search_for_song_form' => drupal_get_form('ismus_admin_search_for_song_form'),
  ];

  return $build;
}

function ismus_admin_search_for_song_form($form, &$form_state) {
  //This is step two of the form
  if(isset($form_state['storage']) && $form_state['storage']['page_two']){
    $artist_array = ismus_api_extract_name_and_id_from_text($form_state['storage']['page_one_values']['song_artist']);
    if(sizeof($form_state['storage']['drupal_song_array']) > 0){
      $drupal_songs = theme('table', ['rows' => $form_state['storage']['drupal_song_array']]);
      $form['drupal_songs'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi lög fundust í kerfinu</h2>' . $drupal_songs . "<p>Ef lagið sem þú ert að stofna er í listanum, skaltu ýta <a href='/'>hér </a> til að hætta við</p>",
      ];
    }
    if(sizeof($form_state['storage']['old_db_song_array']) > 0){
      $form['old_db_song_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi lög fundust í eldri gagnagrunni</h2>',
      ];
      $form['old_db_songs'] = [
        '#type' => 'radios',
        '#options' => $form_state['storage']['old_db_song_array']
      ];
      $form['old_db_song_disclaimer'] = [
        '#type' => 'markup',
        '#markup' => '<p>Ef hljómlagiðplatan sem þú ert að stofna er í þessum lista, skaltu velja það úr listanum.  Þar með færast allar tengingar við það úr eldri gagnagrunni inn í þann nýja.</p>',
      ];
    }

    $artist_name = "";
    if(isset($artist_array['id'])) {
      $artist_name = ' með listamanninum <strong>' . $artist_array['text'] . '</strong>';
      $form['song_name_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Lagið sem þú ert að stofna</h2>' . '<p><strong>' . $form_state['storage']['page_one_values']['song_title'] . '</strong>' . $artist_name . '</p>',
      ];
    }
    else{
      $form['song_name_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Lagið sem þú ert að stofna</h2>' .
          '<p><strong>' . $form_state['storage']['page_one_values']['song_title']  . '</strong></p>' .
          '<p>Listamaðurinn <strong>' . $artist_array['text'] . '</strong> verður einnig stofnaður</p>',
      ];
    }


    $form['song_title'] = [
      '#type' => 'hidden',
      '#value' => $form_state['storage']['page_one_values']['song_title'],
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
    //return ismus_admin_create_song_form($form, $form_state);
  }

  //Original state (step 1) of the form
  $form['song_title'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Song'),
    '#description' => t('The name of the song')
  ];

  $form['song_artist'] = [
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

function ismus_admin_search_for_song_form_submit($form, &$form_state) {
  //This is step 1 of the process.  Searching for duplicates in the database
  //and passing it to step 2.
  if($form_state['clicked_button']['#value'] == 'Next'){
    $values = $form_state['values'];

    $song_title = $values['song_title'];

    $song_search_results = ismus_api_search_for_work($song_title, HLJOMPLATA);
    $exclude_array = null;
    $drupal_song_array = [];
    if($song_search_results) {
      foreach($song_search_results as $asr){
        //Get the old id from the song, so we can exclude those, when searching
        //the old database
        if(isset($asr->field_original_id[LANGUAGE_NONE])){
          $exclude_array[] = $asr->field_original_id[LANGUAGE_NONE][0]['value'];
        }

        //Then add the user to an array, so we can display the search results and
        //ask if the song is already in the database
        $avatar_img_atom = scald_atom_load($asr->field_avatar[LANGUAGE_NONE][0]['sid']);
        $avatar_image_path = image_style_url('thumbnail', $avatar_img_atom->file_source);
        $avatar_image = "<img src='{$avatar_image_path}' />";

        $drupal_song_array[] = [
          $avatar_image,
          l($asr->title, 'node/' . $asr->nid),
        ];
      }
    }

    $old_db_song_search_results = ismus_api_search_for_work_in_old_db($song_title, $exclude_array);

    $old_db_song_array = [];
    if($old_db_song_search_results) {
      foreach($old_db_song_search_results as $odasr){
        $old_db_song_array[$odasr->id] = $odasr->name . " - " . $odasr->artist_name . " - " . $odasr->work_type;
      }
    }

    $form_state['storage']['page_two'] = TRUE;
    $form_state['storage']['page_three'] = FALSE;
    $form_state['storage']['page_one_values'] = $values;
    $form_state['storage']['drupal_song_array'] = $drupal_song_array;
    $form_state['storage']['old_db_song_array'] = $old_db_song_array;
    $form_state["rebuild"] = TRUE;
  }
  //Step 2.  Manly just storing the input in the state variable, so we can
  //pass them along.  Also the rebuild parameter has to be set to true, so we
  //continue and do not start the first part of the form again
  else if($form_state['storage']['page_two']){
    //Handle page two submission
    $values = $form_state['values'];

    $object = new stdClass();
    $object->name = $values['song_title'];
    if(isset($values['old_db_songs'])){
      $object->old_id = $values['old_db_songs'];
    }

    $song = ismus_api_create_work($object, LAG);
    if(isset($values['artist_id'])) {
      ismus_api_create_artist_performs_work_relation($values['artist_id'], $song->nid);
    }
    elseif(isset($values['artist_name'])) {
      $artist_obj = new stdClass();
      $artist_obj->name = $values['artist_name'];
      $artist = ismus_api_create_artist($artist_obj);
      ismus_api_create_artist_performs_work_relation($artist->nid, $song->nid);
    }
    drupal_goto('node/' . $song->nid);
  }
}
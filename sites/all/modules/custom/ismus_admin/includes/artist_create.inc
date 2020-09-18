<?php
/**
 * Create artist page.
 *
 * Here we start creating the artist.  It will display first a search form, for
 * an existing artist, and then guide the user to create one.
 *
 * @return array
 */
function ismus_admin_create_artist_page(){
  $build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Create artist') . '</h2>',
    ],
    'search_for_artist_form' => drupal_get_form('ismus_admin_search_for_artist_form'),
  ];

  return $build;
}

/**
 * Search for artist form.
 *
 * This is a preliminary form, to try to limit duplicates in the database.
 * The user will start by searching the database for the artist he/she is creating
 * and has the option to opt out, if needed.
 *
 * This is a multistep form, in three steps.  The steps are explained in the submit
 * function.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 * @throws \Exception
 */
function ismus_admin_search_for_artist_form($form, &$form_state){
  //This is step two of the form
  if(isset($form_state['storage']) && $form_state['storage']['page_two']){
    if(sizeof($form_state['storage']['drupal_artist_array']) > 0){
      $drupal_artists = theme('table', ['rows' => $form_state['storage']['drupal_artist_array']]);
      $form['drupal_artists'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi listamenn fundust í kerfinu</h2>' . $drupal_artists . "<p>Ef listamaðurinn sem þú ert að stofna er í listanum, skaltu ýta <a href='/'>hér </a> til að hætta við</p>",
      ];
    }
    if(sizeof($form_state['storage']['old_db_artist_array']) > 0){
      $form['old_db_artists_markup'] = [
        '#type' => 'markup',
        '#markup' => '<h2>Eftirfarandi listamenn fundust í eldri gagnagrunni</h2>',
      ];
      $form['old_db_artists'] = [
        '#type' => 'radios',
        '#options' => $form_state['storage']['old_db_artist_array']
      ];
      $form['old_db_artist_disclaimer'] = [
        '#type' => 'markup',
        '#markup' => '<p>Ef listamaðurinn sem þú ert að stofna er í þessum lista, skaltu velja hann úr listanum.  Þar með færast allar tengingar við hann úr eldri gagnagrunni inn í þann nýja.</p>',
      ];
    }
    $form['artist_name_markup'] = [
      '#type' => 'markup',
      '#markup' => '<h2>Listamaðurinn sem þú ert að stofna</h2>' . '<p>' . $form_state['storage']['page_one_values']['artist_name'] . '</p>',
    ];

    $form['artist_name'] = [
      '#type' => 'hidden',
      '#value' => $form_state['storage']['page_one_values']['artist_name'],
    ];

    $form['artist_disclaimer'] = [
      '#type' => 'markup',
      '#markup' => '<p>Ef ekkert er valið hér að ofan mun listamaðurinn með þessu nafni verða stofnaður</p>',
    ];

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );

    return $form;
  }
  //This is step 3 of the form
  elseif(isset($form_state['storage']) && $form_state['storage']['page_three']){
    return ismus_admin_create_artist_form($form, $form_state);
  }

  //Original state (step 1) of the form
  $form['artist_name'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Artist'),
    '#description' => t('The name of the artist (band, group or individual)')
  ];

  $form['artist_type'] = [
    '#type' => 'select',
    '#required' => true,
    '#default_value' => EINSTAKLINGUR,
    '#options' => [
      EINSTAKLINGUR => 'Einstaklingur',
      HLJOMSVEIT => 'Hljómsveit',
      KOR => 'Kór',
    ],
    '#description' => t('The artist can belong to any of these three types'),
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

/**
 * Submission function for search for artist.
 *
 * This function is in three levels, since the form is three step:
 *  1) Validating a submit of the name of the artist
 *     When the name of the artist has been entered, we go through the entities
 *     to see if we already have someone with that name.  Also we go through
 *     the "old" database to see if we have any entries there.  We display
 *     the results on page two.
 *  2) Give the user an opertunity to either exit, if the artist was found, or
 *     to select it from the "older" database, or to do neither and continue to
 *     the third and final step, where the information about the artist is entered.
 *  3) The actual information form.  Here we gather the information entered in the
 *     last step of the form, create an object and send it to the processing
 *     function for creation.
 * @param $form
 * @param $form_state
 */
function ismus_admin_search_for_artist_form_submit($form, &$form_state){
  //This is step 1 of the process.  Searching for duplicates in the database
  //and passing it to step 2.
  if($form_state['clicked_button']['#value'] == 'Next'){
    $values = $form_state['values'];

    $artist_name = $values['artist_name'];

    $artist_search_results = ismus_api_search_for_artist($artist_name);
    $exclude_array = null;
    $drupal_artist_array = [];
    if($artist_search_results) {
      foreach($artist_search_results as $asr){
        //Get the old id from the artist, so we can exclude those, when searching
        //the old database
        if(isset($asr->field_original_id[LANGUAGE_NONE])){
          $exclude_array[] = $asr->field_original_id[LANGUAGE_NONE][0]['value'];
        }

        //Then add the user to an array, so we can display the search results and
        //ask if the artist is already in the database
        $avatar_img_atom = scald_atom_load($asr->field_avatar[LANGUAGE_NONE][0]['sid']);
        $avatar_image_path = image_style_url('thumbnail', $avatar_img_atom->file_source);
        $avatar_image = "<img src='{$avatar_image_path}' />";

        $drupal_artist_array[] = [
          $avatar_image,
          l($asr->title, 'node/' . $asr->nid),
        ];
      }
    }

    $old_db_artist_search_results = ismus_api_search_for_artist_in_old_db($artist_name, $exclude_array);

    $old_db_artist_array = [];
    if($old_db_artist_search_results) {
      foreach($old_db_artist_search_results as $odasr){
        $old_db_artist_array[$odasr->id] = $odasr->name;
      }
    }

    $form_state['storage']['page_two'] = TRUE;
    $form_state['storage']['page_three'] = FALSE;
    $form_state['storage']['page_one_values'] = $values;
    $form_state['storage']['drupal_artist_array'] = $drupal_artist_array;
    $form_state['storage']['old_db_artist_array'] = $old_db_artist_array;
    $form_state["rebuild"] = TRUE;
  }
  //Step 2.  Manly just storing the input in the state variable, so we can
  //pass them along.  Also the rebuild parameter has to be set to true, so we
  //continue and do not start the first part of the form again
  else if($form_state['storage']['page_two']){
    //Handle page two submission
    $values = $form_state['values'];

    $object = new stdClass();
    $object->name = $values['artist_name'];
    $object->artist_type_tid = $form_state['storage']['page_one_values']['artist_type'];
    if(isset($values['old_db_artists'])){
      $object->old_id = $values['old_db_artists'];
    }

    $node = ismus_api_create_artist($object);
    drupal_goto('node/' . $node->nid);
  }
}

/**
 * Create artist form
 *
 * This form is used to create one artist.  It can both be used as a standalone
 * form and as a part of the search for artist form.  The submission functions
 * are different though!
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function ismus_admin_create_artist_form($form, &$form_state){
  //This is the second step of the form
  if(isset($form_state['storage']['page_one_values'])){
    $input_values_one = $form_state['storage']['page_one_values'];
    $input_values_two = $form_state['storage']['page_two_values'];
    $form['artist_name'] = [
      '#type' => 'item',
      '#title' => 'Listamaður',
      '#markup' => '<p>' . $input_values_one['artist_name'] . '</p>',
    ];

    if(isset($input_values_two['old_db_artists'])){
      $form['old_db_artist_id'] = [
        '#type' => 'hidden',
        '#value' => $input_values_two['old_db_artists'],
      ];
    }
  }
  //This is the first step of the form
  else{
    $form['artist_name'] = [
      '#type' => 'textfield',
      '#title' => 'Listamaður',
    ];

  }
}
<?php
/**
EDIT ARTIST
 */

/**
 * Edit artist page.
 *
 * Contains
 * @return array
 */
function ismus_admin_edit_artist_page() {
  drupal_add_js('sites/all/modules/custom/ismus_admin/ismus_admin.js');
  $arg = (int)arg(2);
  $artist = node_load($arg);
  $spotify_artist_entity = null;
  $spotify_artist_search = null;
  $discogs_artist_entity = null;
  $discogs_artist_search = null;
  $musicbrainz_artist_entity = null;
  $musicbrainz_artist_search = null;

  //If the Spotify field is set, we query Spotify with the ID to get detailed
  //information.  If not, we send a Spotify Search Query to try to find the
  //artist.
  //The same logic applies to Discogs and MusicBrainz
  if(isset($artist->field_spotify_artist_id[LANGUAGE_NONE])) {
    $spotify_artist_entity =
      spotify_api_get_artist($artist->field_spotify_artist_id[LANGUAGE_NONE][0]['value']);
  }
  else {
    $spotify_artist_search = spotify_api_search_artist($artist->title);
  }
  if(isset($artist->field_discogs_artist_id[LANGUAGE_NONE])) {
    $discogs_artist_entity =
      discogs_api_get_artist($artist->field_discogs_artist_id[LANGUAGE_NONE][0]['value']);
  }
  else {
    $discogs_artist_search = discogs_api_search_artist($artist->title, true);
  }
  if(isset($artist->field_musicbrainz_artist_id[LANGUAGE_NONE])) {
    $musicbrainz_artist_entity =
      musicbrainz_api_get_artist(
        $artist->field_musicbrainz_artist_id[LANGUAGE_NONE][0]['value']
      );
  }
  else {
    $musicbrainz_artist_search = musicbrainz_api_search_artist($artist->title, true);
  }

  /**$build = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Edit artist') . '</h2>',
    ],
    'search_for_artist_form' => drupal_get_form('ismus_admin_edit_artist_form'),
  ];*/

  module_load_include('inc', 'node', 'node.pages');
  $build['form'] = [
    'header_text' => [
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Edit artist') . '</h2>',
    ],
    'album_edit_form' => drupal_get_form("artist_node_form", $artist),
  ];

  if(!$spotify_artist_entity) {
    $build['spotify_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_spotify_artist_markup($spotify_artist_search),
    ];
  }
  if(!$discogs_artist_entity) {
    $build['discogs_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_discogs_artist_markup($discogs_artist_search),
    ];
  }
  if(!$musicbrainz_artist_entity) {
    $build['musicbrainz_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_musicbrainz_artist_markup(
        $musicbrainz_artist_search
      )
    ];
  }
  if($spotify_artist_entity || $discogs_artist_entity || $musicbrainz_artist_entity) {
    $build['artist_data'] = [
      '#type' => 'markup',
      '#markup' => _ismus_display_create_artist_data_markup(
        $spotify_artist_entity, $discogs_artist_entity, $musicbrainz_artist_entity
      ),
    ];
  }

  return $build;
}


/**
 * BELOW SHOULD ALL BE ABSOLETE
 */

/**
 * Edit artist form
 *
 * Heavily modified form.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function ismus_admin_edit_artist_form($form, &$form_state) {
  $arg = (int)arg(2);
  $artist = node_load($arg);
  $scald_atom = scald_atom_load($artist->field_avatar[LANGUAGE_NONE][0]['sid']);
  $img_src = image_style_url('thumbnail_150x150', $scald_atom->file_source);

  $form['names'] = [
    '#type' => 'fieldset',
    '#title' => 'Nöfn',
    '#collapsible' => true,
    '#collapsed' => false,
  ];

  $form['names']['name'] = [
    '#type' => 'textfield',
    '#title' => 'Nafn listamanns',
    '#value' => $artist->title,
  ];

  $form['names']['real_name'] = [
    '#type' => 'textfield',
    '#title' => 'Rétt nafn',
    '#description' => '"Rétt" nafn listamannsins Megas er Magnús Þór Jónsson, svo dæmi sé nefnt.',
    '#value' => (isset($artist->field_real_name[LANGUAGE_NONE]))
      ? $artist->field_real_name[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['names']['alternate_name'] = [
    '#type' => 'textfield',
    '#title' => 'Önnur nöfn',
    '#description' => 'Önnur nöfn (t.d. gælunöfn) listamannsins.  Ef fleiri en eitt, aðskilið með kommu.',
    '#value' => (isset($artist->field_alternate_names[LANGUAGE_NONE]))
      ? $artist->field_alternate_names[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['birthdate'] = [
    '#type' => 'textfield',
    '#title' => 'Fæðingardagur',
    '#value' => (isset($artist->field_born[LANGUAGE_NONE]))
      ? strftime('%d/%m/%Y', $artist->field_born[LANGUAGE_NONE][0]['value']) : "",
  ];

  $form['death_date'] = [
    '#type' => 'textfield',
    '#title' => 'Dánardagur',
    '#description' => 'Ef viðkomandi er látinn, er dánardagsetningin sett inn hér.',
    '#value' => (isset($artist->field_died[LANGUAGE_NONE]))
      ? strftime('%d/%m/%Y', $artist->field_died[LANGUAGE_NONE][0]['value']) : "",
  ];

  $form['foreign_artist'] = [
    '#type' => 'radios',
    '#options' => [0 => 'Nei', 1 => 'Já'],
    '#default_value' => 0,
    '#title' => 'Erlendur listamaður?',
    '#description' => 'Hakaðu við já, ef listamaðurinn er erlendur (þ.e. kemur ekki við sögu í íslenskri tónlistarsögu, t.d. David Bowie)',
    '#value' => (isset($artist->field_foreign_artist[LANGUAGE_NONE]))
      ? $artist->field_foreign_artist[LANGUAGE_NONE][0]['value'] : "0",
  ];

  $form['biography'] = [
    '#type' => 'textarea',
    '#title' => 'Lífshlaup',
    '#format' => (isset($artist->field_biography[LANGUAGE_NONE]))
      ? $artist->field_biography[LANGUAGE_NONE][0]['format'] : 'full_html',
    '#description' => 'Hér er hægt að setja inn texta um lífshlaup og/eða sögu viðkomandi',
    '#value' => (isset($artist->field_biography[LANGUAGE_NONE]))
      ? $artist->field_biography[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['artist_type'] = [
    '#type' => 'radios',
    '#options' => [EINSTAKLINGUR => 'Einstaklingur', HLJOMSVEIT => 'Hljómsveit', KOR => 'Kór'],
    '#title' => 'Listamannategund',
    '#description' => 'Listamennn eru annað hvort einstaklingar, kórar eða hljómsveit(listamannaflokkur)',
    '#required' => true,
    '#default_value' => (isset($artist->field_artist_type[LANGUAGE_NONE]))
      ? $artist->field_artist_type[LANGUAGE_NONE][0]['tid'] : EINSTAKLINGUR,
  ];

  $form['images'] = [
    '#type' => 'fieldset',
    '#title' => 'Ljósmyndir',
    '#collapsible' => true,
    '#collapsed' => true,
  ];

  $no_of_images = (isset($artist->field_images[LANGUAGE_NONE]))
    ? (count($artist->field_images[LANGUAGE_NONE])) + 3 : 3;

  $form['images']['image_count'] = [
    '#type' => 'hidden',
    '#value' => $no_of_images,
  ];
  $form['images']['avatar_markup'] = [
    '#type' => 'markup',
    '#markup' => "<h3>Avatar</h3><img src='" . $img_src . "' />",
  ];
  $form['images']['avatar'] = [
    '#type' => 'textfield',
    '#title' => 'avatar_url',
    '#size' => 255,
    '#maxlength' => 255,
    '#description' => 'Slóð að mynd eða tölustafir, sem gefa til kynna að myndin sé þegar til.  (152) er placeholder mynd',
    '#value' => (isset($artist->field_avatar[LANGUAGE_NONE]))
      ? $artist->field_avatar[LANGUAGE_NONE][0]['sid'] : "",
  ];

  for($image_counter = 1; $image_counter <= $no_of_images; $image_counter++) {
    if(isset($artist->field_images[LANGUAGE_NONE])){
      if(isset($artist->field_images[LANGUAGE_NONE][$image_counter-1])) {
        $tmp_image = scald_atom_load($artist->field_images[LANGUAGE_NONE][$image_counter-1]['sid']);
        if($tmp_image) {
          $tmp_img_src = image_style_url('thumbnail_150x150', $tmp_image->file_source);
          $form['images']['image' . $image_counter . 'markup'] = [
            '#type' => 'markup',
            '#markup' => "<h3>Mynd nr. {$image_counter}</h3><img src='" . $tmp_img_src . "' />",
          ];
        }
      }
    }

    $form['images']['image' . $image_counter] = [
      '#type' => 'textfield',
      '#title' => "Mynd nr {$image_counter}",
      '#size' => 255,
      '#maxlength' => 255,
      '#description' => 'Slóð að mynd eða tölustafir, sem gefa til kynna að myndin sé þegar til.',
      '#value' => (isset($artist->field_images[LANGUAGE_NONE][$image_counter-1]))
        ? $artist->field_images[LANGUAGE_NONE][$image_counter-1]['sid'] : "",
    ];
  }


  $form['music_providers_id'] = [
    '#type' => 'fieldset',
    '#title' => 'Music Providers IDs',
    '#collapsible' => true,
    '#collapsed' => true,
  ];

  $form['music_providers_id']['spotify_id'] = [
    '#type' => 'textfield',
    '#title' => 'Spotify ID',
    '#description' => 'Ef listamaðurinn er til á Spotify, er ID númerið hans sett hér inn',
    '#value' => (isset($artist->field_spotify_artist_id[LANGUAGE_NONE]))
      ? $artist->field_spotify_artist_id[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['music_providers_id']['discogs_id'] = [
    '#type' => 'textfield',
    '#title' => 'Discogs ID',
    '#description' => 'Ef listamaðurinn er til á Discogs, er ID númerið hans sett hér inn',
    '#value' => (isset($artist->field_discogs_artist_id[LANGUAGE_NONE]))
      ? $artist->field_discogs_artist_id[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['music_providers_id']['musicbrainz_id'] = [
    '#type' => 'textfield',
    '#title' => 'Musicbrainz ID',
    '#description' => 'Ef listamaðurinn er til á Musicbrainz, er ID númerið hans sett hér inn',
    '#value' => (isset($artist->field_musicbrainz_artist_id[LANGUAGE_NONE]))
      ? $artist->field_musicbrainz_artist_id[LANGUAGE_NONE][0]['value'] : "",
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Submit'),
  ];

  return $form;
}

function ismus_admin_edit_artist_form_submit($form, &$form_state) {
  $values = $form_state['input'];
  $arguments = arg();

  $artist = node_load((int)$arguments[2]);
  if(!is_object($artist)) {
    return;
  }

  $artist->title = $values['name'];

  if(strlen($values['real_name']) > 0) {
    $artist->field_real_name[LANGUAGE_NONE][0]['value'] = $values['real_name'];
  }
  if(strlen($values['alternate_name']) > 0) {
    $artist->field_alternate_names[LANGUAGE_NONE][0]['value'] =
      $values['alternate_name'];
  }
  if(strlen($values['birthdate']) > 0) {
    $birthday_epoch = strtotime($values['birthdate']);
    $artist->field_born[LANGUAGE_NONE][0]['value'] = $birthday_epoch;
    $artist->field_born[LANGUAGE_NONE][0]['timezone'] = 'Atlantic/Reykjavik';
    $artist->field_born[LANGUAGE_NONE][0]['timezone_db'] = 'Atlantic/Reykjavik';
    $artist->field_born[LANGUAGE_NONE][0]['date_type'] = 'datestamp';
  }
  if(strlen($values['death_date']) > 0) {
    $birthday_epoch = strtotime($values['death_date']);
    $artist->field_died[LANGUAGE_NONE][0]['value'] = $birthday_epoch;
    $artist->field_died[LANGUAGE_NONE][0]['timezone'] = 'Atlantic/Reykjavik';
    $artist->field_died[LANGUAGE_NONE][0]['timezone_db'] = 'Atlantic/Reykjavik';
    $artist->field_died[LANGUAGE_NONE][0]['date_type'] = 'datestamp';
  }
  if(strlen($values['biography']) > 0) {
    $artist->field_biography[LANGUAGE_NONE][0]['value'] = $values['biography'];
    $artist->field_biography[LANGUAGE_NONE][0]['safe_value'] =
      htmlentities($values['biography']);
  }
  if(strlen($values['spotify_id']) > 0) {
    $artist->field_spotify_artist_id[LANGUAGE_NONE][0]['value'] =
      $values['spotify_id'];
  }
  if(strlen($values['discogs_id']) > 0) {
    $artist->field_discogs_artist_id[LANGUAGE_NONE][0]['value'] =
      $values['discogs_id'];
  }
  if(strlen($values['musicbrainz_id']) > 0) {
    $artist->field_musicbrainz_artist_id[LANGUAGE_NONE][0]['value'] =
      $values['musicbrainz_id'];
  }
  if(strlen($values['avatar']) > 7) {
    $filename = transliteration_get(str_replace(' ', '_', mb_strtolower(trim($artist->title))));
    $scald_atom = _ismus_api_create_scald_image($values['avatar'], $filename);
    if($scald_atom) {
      $artist->field_avatar[LANGUAGE_NONE][0]['sid'] = $scald_atom;
    }
  }

  $image_count = $values['image_count'];
  for($i = 1; $i <= $image_count; $i++) {
    $image = $values['image'.$i];
    if(strlen($image) <= 7) {
      continue;
    }
    if(is_string($values['image'.$i])) {
      $filename = transliteration_get(str_replace(' ', '_', mb_strtolower(trim($artist->title))));
      $scald_atom = _ismus_api_create_scald_image($values['image'.$i], $filename);
      if($scald_atom) {
        $artist->field_images[LANGUAGE_NONE][$i -1]['sid'] = $scald_atom;
      }
    }
  }

  $artist->field_foreign_artist[LANGUAGE_NONE][0]['value'] = $values['foreign_artist'];
  $artist->field_artist_type[LANGUAGE_NONE][0]['tid'] = $values['artist_type'];

  node_save($artist);
}
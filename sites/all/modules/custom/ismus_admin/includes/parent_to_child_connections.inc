<?php

/**
 * PARENT TO CHILD
 */
function ismus_admin_connect_parent_to_child_page(){
  $build = array(
    'header_text' => array(
      '#type' => 'markup',
      '#markup' => '<h2>' . t('Connect parent to child') . '</h2>',
    ),
    'create_parent_to_child_connection_form' => drupal_get_form('ismus_admin_connect_parent_to_child_form'),
  );

  return $build;
}

function ismus_admin_connect_parent_to_child_form($form, &$form_state){
  $args = arg();
  $parent_id = null;
  $child_id = null;
  if($args[2] == 'parent') {
    $parent_id = (int)$args[3];
    $parent_work = node_load($parent_id);
    $parent_string = check_plain($parent_work->title) . " [$parent_work->nid]";
    $form_state['storage']['referer_id'] = $parent_work->nid;
  }
  elseif($args[2] == 'child') {
    $child_id = (int)$args[3];
    $child_work = node_load($child_id);
    $child_string = check_plain(($child_work->title) . " [$child_work->nid]");
    $form_state['storage']['referer_id'] = $child_work->nid;
  }
  $form['parent'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Parent'),
    '#description' => t('The parent song.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
  ];
  if(isset($parent_string)){
    $form['parent'] += ['#default_value' => $parent_string];
  }

  $form['child'] = [
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Child'),
    '#description' => t('The child song.'),
    '#autocomplete_path' => 'lag/unique_work_autocomplete_callback',
  ];
  if(isset($child_string)){
    $form['child'] += ['#default_value' => $child_string];
  }

  $tax = taxonomy_get_tree(11);  //Work difference

  $options_array = [];
  foreach($tax as $t){
    $options_array[$t->tid] = $t->name;
  }
  $form['work_difference'] = [
    '#type' => 'radios',
    '#required' => true,
    '#title' => 'Work difference',
    '#description' => 'Defines what type of difference the child is to the parent',
    '#options' => $options_array,
  ];

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function ismus_admin_connect_parent_to_child_form_submit($form, &$form_state){
  $input = $form_state['input'];
  $parent = ismus_api_extract_string_and_id_from_autocomplete($input['parent']);
  $child = ismus_api_extract_string_and_id_from_autocomplete($input['child']);

  if($parent['id'] == $child['id']){
    drupal_set_message('Trying to connect an entity to itself!', 'error');
    return;
  }

  $relation = ismus_api_get_work_is_part_of_another_work_relation($parent['id'], $child['id']);
  if(is_array($relation)){
    drupal_set_message('Relationship already exists', 'warning');
  }

  else{
    $relation = relation_create('er_til_i_utgafunni', [
      ['entity_type' => 'node', 'entity_id' => $parent['id']],
      ['entity_type' => 'node', 'entity_id' => $child['id']],
    ], null);

    $relation_id = relation_save($relation);
    $relation = relation_load($relation_id);
    $relation->field_work_difference[LANGUAGE_NONE][0]['tid'] = (int)$input['work_difference'];
    relation_save($relation);

    $child_node = node_load($child['id']);
    $child_node->field_is_original[LANGUAGE_NONE][0]['value'] = 0;
    node_save($child_node);
    drupal_set_message(t('Relationship saved and the Is original flag for the child node has been set to false.'));
  }

  drupal_goto('node/' . $form_state['storage']['referer_id']);
}
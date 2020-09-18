<?php
/**
 * @file
 * The primary PHP file for this theme.
 */

/**
 * Returns HTML for a wrapper for a menu sub-tree.
 *
 * @param array $variables
 *   An associative array containing:
 *   - tree: An HTML string containing the tree's items.
 *
 * @return string
 *   The constructed HTML.
 *
 * @see template_preprocess_menu_tree()
 * @see theme_menu_tree()
 *
 * @ingroup theme_functions
 */
function islensktonlist_menu_tree(&$variables) {
  return '<ul class="menu nav navbar-nav nav-pills">' . $variables['tree'] . '</ul>';
}
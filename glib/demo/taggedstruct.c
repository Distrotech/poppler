/*
 * Copyright (C) 2013 Igalia S.L.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <string.h>

#include "text.h"
#include "utils.h"

typedef struct {
  PopplerStructure *structure;
  GtkWidget        *view;
  GtkTreeStore     *store;

  GtkWidget        *type_value;
  GtkWidget        *lang_value;
  GtkWidget        *abbr_value;
  GtkWidget        *id_value;
  GtkWidget        *title_value;
  GtkWidget        *link_target;
  GtkTextBuffer    *text_buffer;
} PgdTaggedStructDemo;


static void
pgd_taggedstruct_free (PgdTaggedStructDemo *demo)
{
  if (!demo)
    return;

  if (demo->store) {
    g_object_unref (demo->store);
    demo->store = NULL;
  }

  if (demo->structure)
      g_object_unref (demo->structure);

  g_free (demo);
}


static void
populate_store_aux (GtkTreeStore *store, GtkTreeIter *parent, PopplerStructureElement *element)
{
  GEnumClass *enum_class = G_ENUM_CLASS (g_type_class_ref (POPPLER_TYPE_STRUCTURE_ELEMENT_KIND));
  GEnumValue *enum_value = g_enum_get_value (enum_class, poppler_structure_element_get_kind (element));
  GtkTreeIter pos;
  guint i;

  gtk_tree_store_append (store, &pos, parent);
  gtk_tree_store_set (store, &pos, 0, enum_value->value_nick, 1, element, -1);

  for (i = 0; i < poppler_structure_element_get_n_children (element); i++)
    populate_store_aux (store, &pos, poppler_structure_element_get_child (element, i));
}


static GtkTreeStore *
populate_store (PopplerStructure *structure)
{
  guint i;
  GtkTreeStore *store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);

  if (structure)
    {
      for (i = 0; i < poppler_structure_get_n_children (structure); i++)
        {
          PopplerStructureElement *element = poppler_structure_get_child (structure, i);
          populate_store_aux (store, NULL, element);
        }
    }
  else
    {
      GtkTreeIter pos;
      gtk_tree_store_append (store, &pos, NULL);
      gtk_tree_store_set (store, &pos, 0, "<b>Not a Tagged-PDF</b>", 1, NULL, -1);
    }

  return store;
}


static void
pgd_row_activated (GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  PgdTaggedStructDemo *demo = (PgdTaggedStructDemo*) user_data;
  GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
  PopplerStructureElement *element;
  GtkTreeIter iter;
  gpointer elementptr;

  if (!gtk_tree_model_get_iter (model, &iter, path))
    return;

  gtk_tree_model_get (model, &iter, 1, &elementptr, -1);
  element = POPPLER_STRUCTURE_ELEMENT (elementptr);

  gtk_label_set_text (GTK_LABEL (demo->id_value),
                      poppler_structure_element_get_id (element));
  gtk_label_set_text (GTK_LABEL (demo->title_value),
                      poppler_structure_element_get_title (element));
  gtk_label_set_text (GTK_LABEL (demo->lang_value),
                      poppler_structure_element_get_language (element));
  gtk_label_set_text (GTK_LABEL (demo->abbr_value),
                      poppler_structure_element_get_abbreviation (element));
  gtk_text_buffer_set_text (demo->text_buffer, "", -1);

  if (poppler_structure_element_is_content (element))
    {
      const gchar *text = poppler_structure_element_get_text (element, FALSE);
      if (text)
        gtk_text_buffer_set_text (demo->text_buffer, text, -1);
      gtk_label_set_text (GTK_LABEL (demo->type_value), "Content");
    }
  else
    {
      if (poppler_structure_element_is_inline (element))
        gtk_label_set_text (GTK_LABEL (demo->type_value), "Inline");
      else if (poppler_structure_element_is_block (element))
        gtk_label_set_text (GTK_LABEL (demo->type_value), "Block");
      else
        gtk_label_set_text (GTK_LABEL (demo->type_value), "Structure");
    }

  gtk_label_set_text (GTK_LABEL (demo->link_target), "");
  if (poppler_structure_element_get_reference_type (element) ==
      POPPLER_STRUCTURE_REFERENCE_LINK)
    {
      PopplerLinkMapping *mapping = poppler_structure_element_get_reference_link (element);
      if (mapping)
        {
          const char *text;
          switch (mapping->action->type)
            {
              case POPPLER_ACTION_URI:
                text = mapping->action->uri.uri;
                break;
            }
          if (text)
            gtk_label_set_text (GTK_LABEL (demo->link_target), text);
          poppler_link_mapping_free (mapping);
        }
    }
}


static void
pgd_cursor_changed (GtkTreeView *tree_view, gpointer user_data)
{
  GtkTreePath *path;
  gtk_tree_view_get_cursor (tree_view, &path, NULL);
  if (path)
    {
      pgd_row_activated (tree_view, path, NULL, user_data);
      gtk_tree_path_free (path);
    }
}


GtkWidget *
pgd_taggedstruct_create_widget (PopplerDocument *document)
{
  PgdTaggedStructDemo *demo;
  GtkCellRenderer *renderer;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *grid;
  GtkWidget *scroll;
  GtkWidget *w;
  gint row;

  demo = g_new0 (PgdTaggedStructDemo, 1);
  demo->structure = poppler_document_get_structure (document);
  if (demo->structure)
      g_object_ref (demo->structure);

  demo->store = populate_store (demo->structure);
  demo->view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (demo->store));

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (demo->view),
                                               0, "Type",
                                               renderer,
                                               "markup", 0,
                                               NULL);
  g_object_set (G_OBJECT (gtk_tree_view_get_column (GTK_TREE_VIEW (demo->view), 0)),
                "expand", TRUE, NULL);

  gtk_tree_view_expand_all (GTK_TREE_VIEW (demo->view));
  gtk_tree_view_set_show_expanders (GTK_TREE_VIEW (demo->view), TRUE);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (demo->view), TRUE);
  gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (demo->view), FALSE);
  gtk_tree_view_set_activate_on_single_click (GTK_TREE_VIEW (demo->view), TRUE);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scroll), demo->view);
  gtk_widget_show (demo->view);
  gtk_box_pack_start (GTK_BOX (hbox), scroll, TRUE, TRUE, 0);
  gtk_widget_show (scroll);

  row = 0;
  grid = gtk_grid_new ();
  gtk_container_set_border_width (GTK_CONTAINER (grid), 12);
  gtk_grid_set_row_homogeneous (GTK_GRID (grid), FALSE);
  gtk_grid_set_column_spacing (GTK_GRID (grid), 6);
  gtk_grid_set_row_spacing (GTK_GRID (grid), 6);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>Type:</b>", &demo->type_value, NULL, &row);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>ID:</b>", &demo->id_value, NULL, &row);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>Title:</b>", &demo->title_value, NULL, &row);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>Language:</b>", &demo->lang_value, NULL, &row);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>Abbreviation:</b>", &demo->abbr_value, NULL, &row);
  pgd_table_add_property_with_value_widget (GTK_GRID (grid), "<b>Link Target:</b>", &demo->link_target, NULL, &row);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_box_pack_start (GTK_BOX (vbox), grid, FALSE, FALSE, 0);
  gtk_widget_show (grid);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_set_border_width (GTK_CONTAINER (scroll), 12);
  gtk_box_pack_end (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);
  gtk_widget_show (scroll);

  gtk_container_add (GTK_CONTAINER (scroll), (w = gtk_text_view_new ()));
  gtk_widget_show (w);

  demo->text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w));
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (w), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (w), FALSE);
  gtk_text_buffer_set_text (demo->text_buffer, "", -1);
  gtk_widget_show (w);

  g_signal_connect (demo->view, "row-activated",
                    G_CALLBACK (pgd_row_activated),
                    demo);
  g_signal_connect (demo->view, "cursor-changed",
                    G_CALLBACK (pgd_cursor_changed),
                    demo);

  gtk_box_pack_end (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  g_object_weak_ref (G_OBJECT (hbox),
                     (GWeakNotify) pgd_taggedstruct_free,
                     demo);

  gtk_widget_show (hbox);
  return hbox;
}

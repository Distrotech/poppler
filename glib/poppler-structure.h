/* poppler-structure.h: glib interface to poppler
 *
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

#ifndef __POPPLER_STRUCTURE_H__
#define __POPPLER_STRUCTURE_H__

#include <glib-object.h>
#include "poppler.h"
#include "poppler-structure-element.h"

G_BEGIN_DECLS

#define POPPLER_TYPE_STRUCTURE            (poppler_structure_get_type ())
#define POPPLER_STRUCTURE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), POPPLER_TYPE_STRUCTURE, PopplerStructure))
#define POPPLER_IS_STRUCTURE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POPPLER_TYPE_STRUCTURE))

GType                       poppler_structure_get_type                  (void) G_GNUC_CONST;
guint                       poppler_structure_get_n_children            (PopplerStructure           *poppler_structure);
PopplerStructureElement    *poppler_structure_get_child                 (PopplerStructure           *poppler_structure,
                                                                         guint                       index);
GList                      *poppler_structure_find_elements             (PopplerStructure           *poppler_structure,
                                                                         PopplerStructureElementKind kind,
                                                                         gint                        page);

G_END_DECLS

#endif /* !__POPPLER_STRUCTURE_H__ */

/*  This file is part of the KDE libraries and the Kate part.
 *
 *  Copyright (C) 2001-2010 Christoph Cullmann <cullmann@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "kateglobal.h"
#include "katedocument.h"

#include <ktexteditor/factory.h>

/**
 * wrapper factory to be sure nobody external deletes our kateglobal object
 * each instance will just increment the reference counter of our internal
 * super private global instance ;)
 */
class KateFactory : public KTextEditor::Factory
{
  public:
    /**
     * constructor, ref the editor, too keep it alive
     * @param parent parent object
     * @param name name of factory
     */
    KateFactory ( QObject *parent = 0 );

    /**
     * destructor, release editor
     */
    virtual ~KateFactory ();

    KTextEditor::Editor *editor ();

    /**
     * reimplemented create object method
     * @param parentWidget parent widget
     * @param parent QObject parent
     * @param args additional arguments
     * @return constructed part object
     */
    KParts::Part *createPartObject ( QWidget *parentWidget, QObject *parent, const char *_classname, const QStringList & );
};

K_EXPORT_PLUGIN( KateFactory )

// kate: space-indent on; indent-width 2; replace-tabs on;

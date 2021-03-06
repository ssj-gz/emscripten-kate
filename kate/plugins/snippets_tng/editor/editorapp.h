/* This file is part of the KDE project
 * Copyright (C) 2009 Joseph Wenninger <jowenn@kde.org>
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _EDITOR_APP_
#define _EDITOR_APP_

#include <kuniqueapplication.h>
#include <kurl.h>
#include <qmap.h>
#include <qstringlist.h>

class SnippetEditorWindow;

class EditorApp: public KUniqueApplication {
  Q_OBJECT
  public:
    EditorApp();
    virtual ~EditorApp();
    virtual int newInstance();
    const QStringList& modes();
  private:    
    bool m_first;
    QMap<KUrl,SnippetEditorWindow*> m_urlWindowMap;
    bool openWindow(const KUrl& url);
    QStringList m_modes;

};

#endif

// kate: space-indent on; indent-width 2; replace-tabs on;

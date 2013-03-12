/* This file is part of the KDE libraries
   Copyright (C) 2010 Dominik Haumann <dhaumann kde org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "katescriptaction.h"
#include "katecommandlinescript.h"
#include "katescriptmanager.h"
#include "kateview.h"
#include "katedocument.h"
#include "kateglobal.h"
#include "kateviewhelpers.h"
#include "kactioncollection.h"

#include <kxmlguifactory.h>
#include <kmenu.h>
#include <kdebug.h>
#include <klocale.h>

//BEGIN KateScriptAction
KateScriptAction::KateScriptAction(const ScriptActionInfo& info, KateView* view)
  : KAction(info.text(), view)
  , m_view(view)
  , m_command(info.command())
  , m_interactive(info.interactive())
{
  if (!info.icon().isEmpty()) {
    setIcon(KIcon(info.icon()));
  }

  if (!info.shortcut().isEmpty()) {
    setShortcut(info.shortcut());
  }

  connect(this, SIGNAL(triggered(bool)), this, SLOT(exec()));
}

KateScriptAction::~KateScriptAction()
{
}

void KateScriptAction::exec()
{
  KateCommandLineBar* cmdLine = m_view->cmdLineBar();

  if (m_interactive) {
    m_view->bottomViewBar()->showBarWidget(cmdLine);
    cmdLine->setText(m_command + ' ', false);
  } else {
    cmdLine->execute(m_command);
  }
}
//END KateScriptAction


//BEGIN KateScriptActionMenu
KateScriptActionMenu::KateScriptActionMenu(KateView *view, const QString& text)
  : KActionMenu (KIcon("code-context"), text, view)
  , m_view(view)
{
#ifndef EMSCRIPTEN
  repopulate();
#else
  // We repopulate lazily with Emscripten, as it is slow and adds a long time to
  // application startup.
  // Need to add at least one action, as otherwise aboutToShow() will not be called.
  menu()->addAction(new QAction(QString("Dummy"), this));
  connect(menu(), SIGNAL(aboutToShow()), this, SLOT(repopulate()));
#endif

  // on script-reload signal, repopulate script menu
  connect(KateGlobal::self()->scriptManager(), SIGNAL(reloaded()),
          this, SLOT(repopulate()));
}

KateScriptActionMenu::~KateScriptActionMenu()
{
  cleanup();
}

void KateScriptActionMenu::cleanup()
{
  // delete menus and actions for real
  qDeleteAll(m_menus);
  m_menus.clear();

  qDeleteAll(m_actions);
  m_actions.clear();
}

#include <qdebug.h>
void KateScriptActionMenu::repopulate()
{
  // if the view is already hooked into the GUI, first remove it
  // now and add it later, so that the changes we do here take effect
  KXMLGUIFactory *viewFactory = m_view->factory();
#ifndef EMSCRIPTEN
  if (viewFactory)
    viewFactory->removeClient(m_view);
#else
  static bool initialRepopulationDone = false;
  if (!initialRepopulationDone)
  {
    // Remove the "Dummy" action we added at construction.
    menu()->removeAction(menu()->actions().first());
    // Don't repopulate every time we reopen this menu
    disconnect(menu(), SIGNAL(aboutToShow()), this, SLOT(repopulate()));
    initialRepopulationDone = false;
  }
#endif

  // remove existing menu actions
  cleanup();

  // now add all command line script commands
  QVector<KateCommandLineScript*> scripts =
    KateGlobal::self()->scriptManager()->commandLineScripts();

  QHash<QString, QMenu*> menus;

  foreach (KateCommandLineScript* script, scripts) {
    qDebug() << "Loading script: " << script->url();

    const QStringList &cmds = script->cmds();
    foreach (const QString& cmd, cmds) {

      ScriptActionInfo info = script->actionInfo(cmd);
      if (!info.isValid())
        continue;

      QMenu* m = menu();

      // show in a category submenu?
      if (!info.category().isEmpty()) {
        m = menus[info.category()];
        if (!m) {
          m = menu()->addMenu(info.category());
          menus.insert(info.category(), m);
          m_menus.append(m);
        }
      }

      // create action + add to menu
      KAction* a = new KateScriptAction(info, m_view);
      m->addAction(a);
      m_view->actionCollection()->addAction("tools_scripts_" + cmd, a);
      m_actions.append(a);
    }
  }

  // finally add the view to the xml factory again, if it initially was there
  if (viewFactory)
    viewFactory->addClient(m_view);
}

//END KateScriptActionMenu

// kate: space-indent on; indent-width 2; replace-tabs on;

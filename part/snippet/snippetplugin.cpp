/*  This file is part of the Kate project.
 *  Based on the snippet plugin from KDevelop 4.
 *
 *  Copyright (C) 2007 Robert Gruber <rgruber@users.sourceforge.net> 
 *  Copyright (C) 2012 Christoph Cullmann <cullmann@kde.org>
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
#include "snippetplugin.h"

#include <klocale.h>
#include <kpluginfactory.h>
#include <kaboutdata.h>
#include <kpluginloader.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/codecompletioninterface.h>
#include <QMenu>

#include <KActionCollection>
#include <KAction>

#include <KTextEditor/HighlightInterface>

#include "snippetview.h"
#include "snippetcompletionmodel.h"
#include "snippetstore.h"

#include "snippet.h"
#include "snippetrepository.h"
#include "snippetcompletionitem.h"
#include "editsnippet.h"

SnippetPlugin* SnippetPlugin::m_self = 0;

class SnippetViewFactory { // FIXME : public KDevelop::IToolViewFactory{
public:
    SnippetViewFactory(SnippetPlugin *plugin): m_plugin(plugin) {}

    virtual QWidget* create(QWidget *parent = 0)
    {
        Q_UNUSED(parent)
        return new SnippetView( m_plugin, parent);
    }

#if 0 //FIXME
    virtual Qt::DockWidgetArea defaultPosition()
    {
        return Qt::RightDockWidgetArea;
    }

    virtual QString id() const
    {
        return "org.kdevelop.SnippetView";
    }
#endif

private:
    SnippetPlugin *m_plugin;
};


SnippetPlugin::SnippetPlugin(QObject *parent, const QVariantList &)
  : QObject(parent)
{
    Q_ASSERT(!m_self);
    m_self = this;

    SnippetStore::init(this);

    m_model = new SnippetCompletionModel;
    

    //FIXME new KDevelop::CodeCompletion(this, m_model, QString());

    // setXMLFile( "kdevsnippet.rc" );

    m_factory = new SnippetViewFactory(this);
    
    /** FIXME
    core()->uiController()->addToolView(i18n("Snippets"), m_factory);
    connect( core()->partController(), SIGNAL(partAdded(KParts::Part*)), this, SLOT(documentLoaded(KParts::Part*)) );
*/
}

SnippetPlugin::~SnippetPlugin()
{
    m_self = 0;
}

SnippetPlugin* SnippetPlugin::self()
{
    return m_self;
}

/**
FIXME
void SnippetPlugin::unload()
{
    core()->uiController()->removeToolView(m_factory);
    delete SnippetStore::self();
}
*/

void SnippetPlugin::insertSnippet(Snippet* snippet)
{
/** FIXME
    KDevelop::IDocument* doc = core()->documentController()->activeDocument();
    if (!doc) return;
    if (doc->isTextDocument()) {
        SnippetCompletionItem item(snippet, static_cast<SnippetRepository*>(snippet->parent()));
        KTextEditor::Range range = doc->textSelection();
        if ( !range.isValid() ) {
            range = KTextEditor::Range(doc->cursorPosition(), doc->cursorPosition());
        }
        item.execute(doc->textDocument(), range);
        if ( doc->textDocument()->activeView() ) {
            doc->textDocument()->activeView()->setFocus();
        }
    }
*/
}

void SnippetPlugin::insertSnippetFromActionData()
{
    KAction* action = dynamic_cast<KAction*>(sender());
    Q_ASSERT(action);
    Snippet* snippet = action->data().value<Snippet*>();
    Q_ASSERT(snippet);
    insertSnippet(snippet);
}

void SnippetPlugin::viewCreated( KTextEditor::Document*, KTextEditor::View* view )
{
    KAction* selectionAction = view->actionCollection()->addAction("edit_selection_snippet", this, SLOT(createSnippetFromSelection()));
    selectionAction->setData(QVariant::fromValue<void *>(view));
}

void SnippetPlugin::documentLoaded( KParts::Part* part )
{
    KTextEditor::Document *textDocument = dynamic_cast<KTextEditor::Document*>( part );
    if ( textDocument ) {
        foreach( KTextEditor::View* view, textDocument->views() )
          viewCreated( textDocument, view );

        connect( textDocument, SIGNAL(viewCreated(KTextEditor::Document*,KTextEditor::View*)), SLOT(viewCreated(KTextEditor::Document*,KTextEditor::View*)) );

    }
}

/** FIXME
KDevelop::ContextMenuExtension SnippetPlugin::contextMenuExtension(KDevelop::Context* context)
{
    KDevelop::ContextMenuExtension extension = KDevelop::IPlugin::contextMenuExtension(context);

    if ( context->type() == KDevelop::Context::EditorContext ) {
        KDevelop::EditorContext *econtext = dynamic_cast<KDevelop::EditorContext*>(context);
        if ( econtext->view()->selection() ) {
            QAction* action = new QAction(KIcon("document-new"), i18n("Create Snippet"), this);
            connect(action, SIGNAL(triggered(bool)), this, SLOT(createSnippetFromSelection()));
            action->setData(QVariant::fromValue<void *>(econtext->view()));
            extension.addAction(KDevelop::ContextMenuExtension::ExtensionGroup, action);
        }
    }

    return extension;
}
*/

void SnippetPlugin::createSnippetFromSelection()
{
    QAction * action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    KTextEditor::View* view = static_cast<KTextEditor::View*>(action->data().value<void *>());
    Q_ASSERT(view);

    QString mode;
    if ( KTextEditor::HighlightInterface* iface = qobject_cast<KTextEditor::HighlightInterface*>(view->document()) ) {
            mode = iface->highlightingModeAt(view->selectionRange().start());
    }
    if ( mode.isEmpty() ) {
        mode = view->document()->mode();
    }

    // try to look for a fitting repo
    SnippetRepository* match = 0;
    for ( int i = 0; i < SnippetStore::self()->rowCount(); ++i ) {
        SnippetRepository* repo = dynamic_cast<SnippetRepository*>( SnippetStore::self()->item(i) );
        if ( repo && repo->fileTypes().count() == 1 && repo->fileTypes().first() == mode ) {
            match = repo;
            break;
        }
    }
    bool created = !match;
    if ( created ) {
        match = SnippetRepository::createRepoFromName(
                i18nc("Autogenerated repository name for a programming language",
                      "%1 snippets", mode)
        );
        match->setFileTypes(QStringList() << mode);
    }

    EditSnippet dlg(match, 0, view);
    dlg.setSnippetText(view->selectionText());
    int status = dlg.exec();
    if ( created && status != KDialog::Accepted ) {
        // cleanup
        match->remove();
    }
}

#include "snippetplugin.moc"

# -*- coding: utf-8 -*-
'''A simple XML pretty printer. XML formatter which a good indents'''
# Copyright (c) 2013 by Pablo Martín <goinnn@gmail.com>
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>.

# This plugin originally was in this repository:
# <https://github.com/goinnn/Kate-plugins/blob/master/kate_plugins/xhtml_plugins/xml_plugins.py>

import kate

from xml.dom import minidom
from xml.parsers.expat import ExpatError

from libkatepate import text

KATE_ACTIONS = {'togglePrettyXMLFormat': {'text': 'Pretty XML',
                                          'shortcut': 'Ctrl+Alt+X',
                                          'menu': 'XML', 'icon': None},
}


@kate.action(**KATE_ACTIONS['togglePrettyXMLFormat'])
def togglePrettyJsonFormat():
    """Pretty format of a XML code"""
    currentDocument = kate.activeDocument()
    view = currentDocument.activeView()
    source = unicode(view.selectionText()).encode('utf-8', 'ignore')
    if not source:
        kate.gui.popup('Select a xml text', 2,
                       icon='dialog-warning',
                       minTextWidth=200)
    else:
        try:
            target = minidom.parseString(source)
            view.removeSelectionText()
            xml_pretty = target.toprettyxml()
            xml_pretty = '\n'.join([line for line in xml_pretty.split("\n") if line.replace(' ', '').replace('\t', '')])
            text.insertText(xml_pretty)
        except ExpatError:
            kate.gui.popup('This text is not a valid xml text', 2,
                        icon='dialog-warning',
                        minTextWidth=200)

/*
 * PDFedit - free program for PDF document manipulation.
 * Copyright (C) 2006, 2007, 2008  PDFedit team: Michal Hocko,
 *                                              Miroslav Jahoda,
 *                                              Jozef Misutka,
 *                                              Martin Petricek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in doc/LICENSE.GPL); if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 * MA  02111-1307  USA
 *
 * Project is hosted on http://sourceforge.net/projects/pdfedit
 */
#ifndef __ZOOMTOOL_H__
#define __ZOOMTOOL_H__

#include <qwidget.h>

class QResizeEvent;
class QComboBox;
class QValidator;

namespace gui {

/**
 Toolbutton allowing to change zoom level of current page<br>
 Can be placed in toolbar in place of ordinary button
 \brief Toolbar widget for changing zoom level
*/
class ZoomTool : public QWidget {
Q_OBJECT
public:
 ZoomTool(QWidget *parent=0,const char *name=NULL);
 ~ZoomTool();
 QSize sizeHint() const;
public slots:
 void updateZoom(float zoom);
signals:
 /**
  Signal emitted when user changes the zoom level
  @param zoom New zoom level
 */
 void zoomSet(float zoom);
protected:
 void resizeEvent (QResizeEvent *e);
protected slots:
 void selectZoom(const QString &newZoom);
protected:
 /** Zoom selection editable combo */
 QComboBox *zoomList;
 /** Validator for zoom values */
 QValidator *ival;
};

} // namespace gui

#endif

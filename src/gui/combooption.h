#ifndef __COMBOOPTION_H__
#define __COMBOOPTION_H__

#include "option.h"
class QString;
class QComboBox;
class QStringList;

namespace gui {

/**
 Class for widget containing one editable setting of type string, selectable from combobox<br>
 If current setting specify item not in the list, the first item in list is shown instead<br>
 User is unable to specify string not in the list<br>
 Used as one item type in option window<br>
*/
class ComboOption : public Option {
 Q_OBJECT
public:
 ComboOption(const QStringList &_values,const QString &_key=0,QWidget *parent=0);
 ~ComboOption();
 virtual void writeValue();
 virtual void readValue();
 virtual QSize sizeHint() const;
 void setCaseSensitive(bool value);
protected:
 virtual void resizeEvent (QResizeEvent *e);
protected:
 /** edit control used for editing the value (combo box) */
 QComboBox *ed;
 /** List of values in the control. */
 QStringList values;
 /** Are items case sensitive? */
 bool caseSensitive;
};

} // namespace gui

#endif

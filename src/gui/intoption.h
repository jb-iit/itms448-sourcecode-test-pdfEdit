#ifndef __INTOPTION_H__
#define __INTOPTION_H__

#include "stringoption.h"
class QString;

namespace gui {

/**
 class for widget containing one editable setting of type integer<br>
 Used as one item type in option window<br>
*/
class IntOption : public StringOption {
 Q_OBJECT
public:
 IntOption(const QString &_key=0,QWidget *parent=0,int defValue=0);
 virtual ~IntOption();
};

} // namespace gui

#endif

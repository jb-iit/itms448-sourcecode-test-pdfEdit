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
/** @file
 Checker and translation generator of menu configuration<br>
 This is helper utility used to:<br>
  - check menus for translatable strings and write them to .menu-trans.h,
    so they will be found by lupdate utility<br>
  - check menus for unreferenced items<br>
 @author Martin Petricek
*/

#include <iostream>
#include <string.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qdir.h>
#include "config.h"
#include "util.h"
#include "menu.h"
#include "menugenerator.h"

using namespace std;
using namespace util;

/** Constructor */
MenuGenerator::MenuGenerator() {
 set=new gui::StaticSettings();
 //generate to/from current directory
 set->tryLoad("pdfeditrc");
}

/** Destructor */
MenuGenerator::~MenuGenerator() {
 delete set;
}

/**
 Check if given name is a "special item" (or separator)
 @param itemName Name of the item
 @return true if the item is special, false if not.
*/
bool MenuGenerator::special(const QString &itemName) {
 if (itemName=="") return true;
 if (itemName=="-") return true;
 if (itemName.startsWith("_")) return true;
 return false;
}

/**
 Set menu item as "reachable" (increases its reference count) and add translation on it.
 Recursively run on subitems if item is list
 @param name Id of menu item
*/
void MenuGenerator::setAvail(const QString &name) {
 if (special(name)) return; //return if separator
 avail[name]+=1;
 if (avail[name]>=2) return; //already seen this one
 QString line=set->readEntry("gui/items/"+name);
 line=line.simplifyWhiteSpace();
 if (line.startsWith("list ")) { // List of values - a submenu, first is name of submenu, others are items in it
  line=line.remove(0,5);
  QStringList qs=explode(gui::MENULIST_SEPARATOR,line);
  QStringList::Iterator it=qs.begin();
  if (it!=qs.end()) {
   if (!special(*it)) { //not a separator or special item
    addLocString(name,*it);
   }
   ++it;
  } else fatalError("Invalid menu item in config:\n"+line);
  for (;it!=qs.end();++it) { //load all subitems
   setAvail(*it);
  }
 } else if (line.startsWith("label ")) { // A single item
  line=line.remove(0,6);
  addLocString(name,line);
 } else if (line.startsWith("item ")) { // A single item
  line=line.remove(0,5);
  QStringList qs=explode(gui::MENUDEF_SEPARATOR,line,true);
  addLocString(name,qs[0]);
  if (qs.count()<2) fatalError("Invalid menu item in config:\n"+line);
 } else { //something invalid
  fatalError("Invalid menu item in config:\n"+line);
 }
}

/**
 Check menu structure, print items and their reference counts
 Warn about unreferenced items
*/
void MenuGenerator::check() {
 QStringList items=set->entryList("gui/items");
 QString toolBarList=set->readEntry("gui/toolbars");
 toolBarList=toolBarList.simplifyWhiteSpace();
 QStringList toolb=QStringList::split(",",toolBarList);

 //Toolbars are root items
 for (QStringList::Iterator it=toolb.begin();it!=toolb.end();++it) {
  setAvail(*it);
 }

 //Main menu is root item
 setAvail("MainMenu");

 int ava;
 for (QStringList::Iterator it=items.begin();it!=items.end();++it) {
  cout << "Item : ";
  cout.width(20);
  cout.flags(ios::left);
  cout << convertFromUnicode(*it,CON);
  ava=avail[*it];
  if (!ava) cout << " (unreachable!)";
  else  cout << " (" << ava << " refs)";
  cout << endl;
 }
}

/**
 Add menu to localization list
 @param id Id of menu item
 @param name Caption of menu item
*/
void MenuGenerator::addLocString(const QString &id,const QString &name) {
 trans+=QString("QT_TRANSLATE_NOOP( \"gui::Settings\",\"")+name+"\",\""+id+"\")";
 cout << convertFromUnicode(id,CON) << " = " << convertFromUnicode(name,CON) << endl;
}

/** Produce dummy header used for menu items localization */
void MenuGenerator::translate() {
 check();
 QString trx=trans.join("\n")+"\n";
 QFile file(".menu-trans.h");
 if ( file.open( IO_WriteOnly ) ) {
  QTextStream stream( &file );
  stream << "//File automatically generated by menugenerator from pdfeditrc" << endl;
  stream << "//Do not edit, any changes will be overwritten" << endl;
  stream << trx;
  file.close();
 } else {
  fatalError("Cannot open file on write!");
 }
}

/**
 Main function of menugenerator utility
 @param argc Argument count
 @param argv Commandline arguments
 @return Executable return code
*/
int main(int argc, char *argv[]){
 MenuGenerator m;
 cout << "       \"menugenerator\" to check menus" <<endl
      << "       \"menugenerator -trans\" to generate translation" <<endl;
 if (argc>1) {
  if (strcmp(argv[1],"-trans")==0) {
   cout << "Checking menu" << endl;
   m.translate();
   cout << "Done checking menu" << endl;
  }
 } else { //check menus, do localization
   cout << "Checking menu" << endl;
   //TODO: check accelerator conflicts in single menu
   m.check();
   cout << "Done checking menu" << endl;
 }
}

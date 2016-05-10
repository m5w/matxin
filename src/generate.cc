/*
 *  Copyright (C) 2005 IXA Research Group / IXA Ikerkuntza Taldea.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#include <string>
#include <iostream>
#include <sstream>
#include <locale>
#include <clocale>

#include <lttoolbox/fst_processor.h>
#include <lttoolbox/ltstr.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "matxin_string_utils.h"
#include "transfer.h"

#include <data_manager.h>

using namespace std;

bool debug = false ;

int nodeCount = 0;

void usage(char *name)
{
  wcout << L"" << name << L" <generation rules> <morphological generator>" << endl ;
}

wstring& escape(wstring &s)
{
   if(s == L"\"") 
   {
     s = L"&quot;" ;
   }

   return s;
}

void procAttr(xmlNodePtr p)
{
    xmlAttr* attr = p->properties;
    if(attr) 
    {
      wcout << L" ";
    }
    while(attr)
    {
      xmlChar* value = xmlNodeListGetString(p->doc, attr->children, 1);
      wcout << towstring(attr->name) << L"=\"" << towstring(value) << L"\"";
      xmlFree(value); 
      if(attr->next)
      {
        wcout << L" ";
      }
      attr = attr->next;
    }
    xmlFree(attr);
}

void procNode(FSTProcessor &fstp, xmlNodePtr p)
{
    while(p != NULL) 
    {
      if(p->type == 1)
      {
        wcout << L"<" << towstring(p->name); 
        if(!xmlStrcmp(p->name, (xmlChar *)"NODE")) 
        {
          nodeCount++;
          wstring mstr, form = L"";

          xmlChar *lem = xmlGetProp(p, (xmlChar *)"lem");
          xmlChar *smi = xmlGetProp(p, (xmlChar *)"smi");
          xmlChar *mi = xmlGetProp(p, (xmlChar *)"mi");
          if(mi != NULL && lem != NULL) 
          {
            mstr = L"^" + towstring(lem) + towstring(mi) + L"$";
            form = fstp.biltrans(mstr);
            form = form.substr(1, form.size() - 2);
            wcerr << L"FORM: " << form << L"\t||\t"  << L"\t||\t" << towstring(lem) << L" " << towstring(mi) << endl;
            xmlSetProp(p, (xmlChar *)"form", (xmlChar *)wstos(form).c_str());
          }
          else if(lem != NULL && mi == NULL && smi != NULL) 
          {
            form = L"?" + towstring(lem) + L"|" + towstring(smi) ; 
            wcerr << L"NOMI: " << form << L"\t||\t" << endl;
            xmlSetProp(p, (xmlChar *)"form", (xmlChar *)wstos(form).c_str());
          }
          else if(lem != NULL && mi == NULL && smi != NULL) 
          {
            form = L"#" + towstring(mi) ; 
            wcerr << L"NOMI: " << form << L"\t||\t" << endl;
            xmlSetProp(p, (xmlChar *)"form", (xmlChar *)wstos(form).c_str());
          }
          else
          { 
            wstring err = L"";
            if(lem) err = err + towstring(lem);
            if(mi) err = err + towstring(mi);
            wcerr << L"FAIL: not received!\t||\t" << err << endl;
          }
           
        }
        procAttr(p);
      }
      else if(p->type == 3)
      {
        wcout << towstring(p->content);
      }

      if(p->xmlChildrenNode)
      {
        wcout << L">";
        procNode(fstp, p->xmlChildrenNode);
        wstring name = towstring(p->name);
        wcout << L"</" << name << L">";
      }
      else if(p->type == 1)
      {
        wcout << L"/>";
      }

      p = p->next;
    }
 

    return;
}


int main(int argc, char *argv[])
{
  FSTProcessor fstp;

  // This sets the C++ locale and affects to C and C++ locales.
  // wcout.imbue doesn't have any effect but the in/out streams use the proper encoding.
  locale::global(locale(""));

  if(argc != 3)
  {
    usage(argv[0]);
    exit(-1);
  }

  string genRulesFile = string(argv[1]);
  string morphGenFile = string(argv[2]);

  FILE *transducer = fopen(morphGenFile.c_str(), "r");
  fstp.load(transducer);
  fclose(transducer);
  fstp.initBiltrans();

  xsltStylesheetPtr sheet = xsltParseStylesheetFile((const xmlChar *)genRulesFile.c_str());

  xmlDocPtr doc, res = NULL;
  doc = xmlReadFd(0, "/", NULL, 0);

  xmlSubstituteEntitiesDefault(1);
  res = xsltApplyStylesheet(sheet, doc, NULL); 

  // now generate 

  xmlChar *findRules = (xmlChar*)"//SENTENCE" ;

  xmlXPathContextPtr context = xmlXPathNewContext(res);
  xmlXPathObjectPtr rres = xmlXPathEvalExpression(findRules, context);
  xmlNodeSetPtr nodes = rres->nodesetval;

  int size = (rres->nodesetval) ? nodes->nodeNr : 0;

  wcout << L"<corpus>" << endl;

  xmlNodePtr cur;
  for(int i = 0; i < size; i++) 
  {
    cur = nodes->nodeTab[i];

    wcout << L"<" << towstring(cur->name);
    procAttr(cur);
    if(cur->xmlChildrenNode) 
    {
      wcout << L">";
      procNode(fstp, cur->xmlChildrenNode);
      wcout << L"</" << towstring(cur->name) << L">";
    }
    else
    {
      wcout << L"/>" << endl; 
    }

    cur = cur->next;
  }

  wcout << L"</corpus>" << endl;

  wcerr << nodeCount << endl;

  xmlFreeDoc(doc);
  xmlFreeDoc(res);

  xsltCleanupGlobals();
  xmlCleanupParser();
}

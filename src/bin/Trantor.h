#ifndef _TRANTOR_H_
#define _TRANTOR_H_

#include <Efl_Egueb.h>
#include <Ecore.h>
#include <Egueb_Svg.h>
#include <Eon.h>

typedef struct _Trantor
{
	Efl_Egueb_Window *window;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *svg;
} Trantor;

void trantor_view_xml_new(Trantor *thiz, Egueb_Dom_Node *xml_doc);
Egueb_Dom_Node * trantor_svg_get(Trantor *thiz);
void trantor_element_select(Trantor *thiz, Egueb_Dom_Node *n);
void trantor_element_unselect(Trantor *thiz, Egueb_Dom_Node *n);

extern Egueb_Dom_String *TRANTOR_EVENT_ELEMENT_SELECTED;
extern Egueb_Dom_String *TRANTOR_EVENT_ELEMENT_UNSELECTED;

#endif

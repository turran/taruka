#ifndef _TRANTOR_H_
#define _TRANTOR_H_

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Egueb_Svg.h>
#include <Efl_Svg_Smart.h>

typedef struct _Trantor
{
	char *location;
	Egueb_Dom_Node *doc_svg;
	Evas_Object *o_drawing_area;
	Evas_Object *o_svg;
	Evas_Object *o_xml;
} Trantor;

void trantor_view_xml_new(Trantor *thiz, Egueb_Dom_Node *xml_doc);
Egueb_Dom_Node * trantor_svg_get(Trantor *thiz);
void trantor_element_select(Trantor *thiz, Egueb_Dom_Node *n);
void trantor_element_unselect(Trantor *thiz, Egueb_Dom_Node *n);

#endif

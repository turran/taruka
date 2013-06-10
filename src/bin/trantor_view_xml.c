#include <Trantor.h>

#if 0
static char * _myelement_to_string(Egueb_Dom_Node *n)
{
	MyElement *thiz;
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_String *name;
	Egueb_Dom_Node_Map_Named *attrs = NULL;
	char *str = NULL;
	char *ret = NULL;

	thiz = MYELEMENT(n);
	egueb_dom_element_tag_name_get(n, &name);
	str = eina_str_dup_printf("<%s", egueb_dom_string_string_get(name));
	/* dump every proeprty */
	egueb_dom_node_attributes_get(n, &attrs);
	if (attrs)
	{
		int count;
		int i;

		count = egueb_dom_node_map_named_length(attrs);
		for (i = 0; i < count; i++)
		{
			Egueb_Dom_Node *attr = NULL;
			Egueb_Dom_String *attr_name;
			Egueb_Dom_String *attr_value = NULL;

			egueb_dom_node_map_named_at(attrs, i, &attr);
			if (!attr) continue;
			if (!egueb_dom_attr_is_set(attr)) goto no_name;

			egueb_dom_attr_name_get(attr, &attr_name);
			if (!attr_name) goto no_name;

			egueb_dom_attr_string_get(attr, &attr_value);
			if (!attr_value) goto no_value;

			ret = eina_str_dup_printf("%s %s=\"%s\"", str,
					egueb_dom_string_string_get(attr_name),
					egueb_dom_string_string_get(attr_value));
			free(str);
			str = ret;

			egueb_dom_string_unref(attr_value);
no_value:
			egueb_dom_string_unref(attr_name);
no_name:
			egueb_dom_node_unref(attr);
		}
		egueb_dom_node_map_named_free(attrs);
	}
	/* nor some meta information */
	egueb_dom_node_document_get(n, &doc);
	ret = eina_str_dup_printf("%s> (ref: %d, doc: %p)", str,
			egueb_dom_node_ref_get(n), doc);
	free(str);
	egueb_dom_string_unref(name);
	if (doc) egueb_dom_node_unref(doc);

	return ret;
}

static void _myelement_dump(Egueb_Dom_Node *thiz, Eina_Bool deep, int level)
{
	Egueb_Dom_Node *child = NULL;
	char *str;
	int i;

	for (i = 0; i < level; i++)
	{
		printf(" ");
	}
	/* print the element */
	str = _myelement_to_string(thiz);
	printf("%s\n", str);
	free(str);

	/* in case of deep, also do the children */
	if (!deep) goto done;

	egueb_dom_node_child_first_get(thiz, &child);
	if (!child) goto done;

	do
	{
		Egueb_Dom_Node *sibling = NULL;

		egueb_dom_node_sibling_next_get(child, &sibling);
		_myelement_dump(child, deep, level + 1);
		child = sibling;
	} while (child);
done:
	egueb_dom_node_unref(thiz);
}
#endif

Evas_Object * trantor_view_xml_new(Evas *e, Egueb_Dom_Node *other_doc)
{
	Evas_Object *o;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *other_svg, *svg;
	Egueb_Dom_Node *text;

	o = efl_svg_new(e);
	doc = efl_svg_document_get(o);
	svg = egueb_svg_element_svg_new();

	egueb_dom_document_element_set(doc, egueb_dom_node_ref(svg));
	egueb_dom_document_element_get(other_doc, &other_svg);
	/* iterate over the whole tree of the other doc and create text
	 * elements here
	 */
	text = egueb_svg_element_text_new();
	egueb_dom_node_child_append(svg, text);
	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(other_svg);

	return o;
}

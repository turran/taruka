#include <Trantor.h>

typedef struct _Trantor_View_Xml
{
	Egueb_Dom_Node *svg;
	int level;
} Trantor_View_Xml;

static Eina_Bool _generate_node(Egueb_Dom_Node *n, void *data);

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

static Eina_Bool _generate_element(Trantor_View_Xml *thiz, Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *text;
	Egueb_Dom_Node *tnode;
	Egueb_Dom_Node *child = NULL;
	Egueb_Dom_String *name;
	Egueb_Svg_Font_Size font_size;
	Egueb_Svg_Length y;

	egueb_dom_element_tag_name_get(n, &name);
	text = egueb_svg_element_text_new();

	font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_font_size_set(text, &font_size);
	egueb_svg_length_set(&y, 12 * thiz->level, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_text_y_set(text, &y);
	tnode = egueb_dom_text_new();

	egueb_dom_character_data_append_data_inline(tnode, "<");
	egueb_dom_character_data_append_data(tnode, name);
	egueb_dom_node_child_append(text, tnode);

	egueb_dom_node_child_append(thiz->svg, text);
	/* now the attributes */
	/* now the children */
	egueb_dom_node_child_first_get(n, &child);
	if (child)
	{
		egueb_dom_character_data_append_data_inline(tnode, ">");
		do
		{
			Egueb_Dom_Node *next = NULL;

			thiz->level++;
			_generate_node(child, thiz);
			egueb_dom_node_sibling_next_get(child, &next);
			egueb_dom_node_unref(child);
			child = next;
		} while (child);
	}
	else
	{
		egueb_dom_character_data_append_data_inline(tnode, "/>");
	}
	return EINA_TRUE;
}

static Eina_Bool _generate_node(Egueb_Dom_Node *n, void *data)
{
	Trantor_View_Xml *thiz = data;
	Egueb_Dom_Node_Type type;

	egueb_dom_node_type_get(n, &type);
	switch (type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT_NODE:
		_generate_element(thiz, n);
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE_NODE:
		case EGUEB_DOM_NODE_TYPE_TEXT_NODE:
		case EGUEB_DOM_NODE_TYPE_CDATA_SECTION_NODE:
		case EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE_NODE:
		case EGUEB_DOM_NODE_TYPE_ENTITY_NODE:
		case EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION_NODE:
		case EGUEB_DOM_NODE_TYPE_COMMENT_NODE:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE_NODE:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT_NODE:
		case EGUEB_DOM_NODE_TYPE_NOTATION_NODE:
		break;
	}
	return EINA_TRUE;
}

void trantor_view_xml_new(Egueb_Dom_Node *doc, Egueb_Dom_Node *other_doc)
{
	Trantor_View_Xml thiz;
	Egueb_Dom_Node *other_svg, *svg;

	svg = egueb_svg_element_svg_new();

	egueb_dom_document_element_set(doc, egueb_dom_node_ref(svg));
	egueb_dom_document_element_get(other_doc, &other_svg);

	thiz.svg = svg;
	thiz.level = 1;
	_generate_node(other_svg,  &thiz);

	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(other_svg);
	egueb_dom_node_unref(doc);
	egueb_dom_node_unref(other_doc);
}

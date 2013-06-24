#include <Trantor.h>

typedef struct _Trantor_View_Xml
{
	Egueb_Dom_Node *svg;
	int tab;
	int level;
} Trantor_View_Xml;

static Eina_Bool _generate_node(Egueb_Dom_Node *n, void *data);

static void _element_name_over_cb(Egueb_Dom_Event *e, void *data)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Svg_Color color;

	egueb_dom_event_target_get(e, &target);
	egueb_svg_color_components_from(&color, 0xff, 0x00, 0x00);
	egueb_svg_element_color_set(target, &color);
	egueb_dom_node_unref(target);
}

static void _element_name_out_cb(Egueb_Dom_Event *e, void *data)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Svg_Color color;

	egueb_dom_event_target_get(e, &target);
	egueb_svg_color_components_from(&color, 0x00, 0x00, 0x00);
	egueb_svg_element_color_set(target, &color);
	egueb_dom_node_unref(target);
}

/* FIXME we should enable this once we have the <tspan> implemented */
static void _attrs_create(Trantor_View_Xml *thiz, Egueb_Dom_Node *n, Egueb_Dom_Node *parent)
{
	Egueb_Dom_Node_Map_Named *attrs = NULL;
	int count;
	int i;

	/* FIXME we moved the append here, because the node inserted is not propagating
	 * the event for the children nodes of the inserted node
	 */
	egueb_dom_node_child_append(thiz->svg, parent);

	egueb_dom_node_attributes_get(n, &attrs);
	if (!attrs) return;

	count = egueb_dom_node_map_named_length(attrs);
	for (i = 0; i < count; i++)
	{
		Egueb_Dom_Node *attr = NULL;
		Egueb_Dom_String *attr_name;
		Egueb_Dom_String *attr_value = NULL;
		Egueb_Dom_Node *tspan;
		Egueb_Dom_Node *tnode;
		Egueb_Svg_Font_Size font_size;

		egueb_dom_node_map_named_at(attrs, i, &attr);
		if (!attr) continue;
		if (!egueb_dom_attr_is_set(attr)) goto no_name;

		egueb_dom_attr_name_get(attr, &attr_name);
		if (!attr_name) goto no_name;

		egueb_dom_attr_string_get(attr, EGUEB_DOM_ATTR_TYPE_BASE, &attr_value);
		if (!attr_value)
		{
			egueb_dom_string_unref(attr_name);
			goto no_name;
		}

		tnode = egueb_dom_text_new();
		egueb_dom_character_data_append_data_inline(tnode, " ");
		egueb_dom_node_child_append(parent, tnode);

		tspan = egueb_svg_element_tspan_new();
		font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
		egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_UNIT_LENGTH_PX);
		egueb_svg_element_font_size_set(tspan, &font_size);

		tnode = egueb_dom_text_new();
		egueb_dom_character_data_append_data(tnode, attr_name);
		egueb_dom_character_data_append_data_inline(tnode, "=\"");
		egueb_dom_character_data_append_data(tnode, attr_value);
		egueb_dom_character_data_append_data_inline(tnode, "\"");

		egueb_dom_node_child_append(tspan, tnode);
		egueb_dom_node_child_append(parent, tspan);
no_name:
		egueb_dom_node_unref(attr);
	}

	egueb_dom_node_map_named_free(attrs);
}

static void _tag_create(Trantor_View_Xml *thiz, Egueb_Dom_Node *n, Eina_Bool open)
{
	Egueb_Dom_Node *text;
	Egueb_Dom_Node *tnode;
	Egueb_Dom_String *name;
	Egueb_Svg_Font_Size font_size;
	Egueb_Svg_Length x, y;

	egueb_dom_element_tag_name_get(n, &name);
	text = egueb_svg_element_text_new();

	font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_font_size_set(text, &font_size);

	egueb_svg_length_set(&y, 12 * thiz->level, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_text_y_set(text, &y);
	egueb_svg_length_set(&x, 12 * thiz->tab, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_text_x_set(text, &x);
	tnode = egueb_dom_text_new();

	if (open)
	{
		Egueb_Dom_Node *child = NULL;
		Egueb_Svg_Color color;

		egueb_dom_character_data_append_data_inline(tnode, "<");
		egueb_dom_character_data_append_data(tnode, name);
		egueb_dom_node_child_append(text, tnode);

		/* set the default fill paint to current color and the color to black */
		egueb_svg_color_components_from(&color, 0, 0, 0);
		egueb_svg_element_color_set(text, &color);
		egueb_svg_element_fill_set(text, &EGUEB_SVG_PAINT_CURRENT_COLOR);
		/* add the needed callbacks */
		egueb_dom_node_event_listener_add(text, EGUEB_DOM_EVENT_MOUSE_OVER,
				_element_name_over_cb, EINA_TRUE, NULL);
		egueb_dom_node_event_listener_add(text, EGUEB_DOM_EVENT_MOUSE_OUT,
				_element_name_out_cb, EINA_TRUE, NULL);

		/* now the attributes */
		_attrs_create(thiz, n, text);
		//egueb_dom_node_child_append(thiz->svg, text);
		/* now the children */
		egueb_dom_node_child_first_get(n, &child);

		/* another node for closing the tag */
		tnode = egueb_dom_text_new();
		if (child)
		{
			thiz->tab++;
			egueb_dom_character_data_append_data_inline(tnode, ">");
			egueb_dom_node_child_append(text, tnode);

			do
			{
				Egueb_Dom_Node *next = NULL;

				thiz->level++;
				_generate_node(child, thiz);
				egueb_dom_node_sibling_next_get(child, &next);
				egueb_dom_node_unref(child);
				child = next;
			} while (child);
			thiz->tab--;

			thiz->level++;
			_tag_create(thiz, n, EINA_FALSE);
		}
		else
		{
			egueb_dom_character_data_append_data_inline(tnode, "/>");
			egueb_dom_node_child_append(text, tnode);
		}
	}
	else
	{
		egueb_dom_character_data_append_data_inline(tnode, "</");
		egueb_dom_character_data_append_data(tnode, name);
		egueb_dom_character_data_append_data_inline(tnode, ">");
		egueb_dom_node_child_append(text, tnode);
		egueb_dom_node_child_append(thiz->svg, text);
	}
}

static Eina_Bool _generate_element(Trantor_View_Xml *thiz, Egueb_Dom_Node *n)
{
	_tag_create(thiz, n, EINA_TRUE);
	return EINA_TRUE;
}

static Eina_Bool _generate_text(Trantor_View_Xml *thiz, Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *text;
	Egueb_Dom_Node *tnode;
	Egueb_Dom_String *data;
	Egueb_Svg_Font_Size font_size;
	Egueb_Svg_Length x, y;
	Egueb_Svg_Color color;

	text = egueb_svg_element_text_new();

	font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_font_size_set(text, &font_size);

	egueb_svg_length_set(&y, 12 * thiz->level, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_text_y_set(text, &y);
	egueb_svg_length_set(&x, 12 * thiz->tab, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_text_x_set(text, &x);

	egueb_svg_color_components_from(&color, 0xff, 0, 0);
	egueb_svg_element_color_set(text, &color);
	egueb_svg_element_fill_set(text, &EGUEB_SVG_PAINT_CURRENT_COLOR);

	tnode = egueb_dom_text_new();
	egueb_dom_character_data_data_get(n, &data);
	egueb_dom_character_data_append_data(tnode, data);

	egueb_dom_node_child_append(text, tnode);
	egueb_dom_node_child_append(thiz->svg, text);

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

		case EGUEB_DOM_NODE_TYPE_TEXT_NODE:
		_generate_text(thiz, n);
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE_NODE:
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

void trantor_view_xml_new(Trantor *t, Egueb_Dom_Node *xml_doc)
{
	Trantor_View_Xml thiz;
	Egueb_Dom_Node *other_svg, *svg;

	svg = egueb_svg_element_svg_new();
	other_svg = trantor_svg_get(t);

	egueb_dom_document_element_set(xml_doc, egueb_dom_node_ref(svg));

	thiz.svg = svg;
	thiz.level = 1;
	thiz.tab = 0;
	_generate_node(other_svg,  &thiz);

	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(other_svg);
	egueb_dom_node_unref(xml_doc);
}

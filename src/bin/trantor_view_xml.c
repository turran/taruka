#include <Trantor.h>

typedef struct _Trantor_View_Xml
{
	Trantor *t;
	Egueb_Dom_Node *svg;
} Trantor_View_Xml;

typedef struct _Trantor_View_Xml_Generate_Data
{
	Trantor_View_Xml *thiz;
	int tab;
	int level;
} Trantor_View_Xml_Generate_Data;

/* the different keys */
static Egueb_Dom_String *_trantor_view_xml_key = NULL;
static Egueb_Dom_String *_trantor_view_xml_element_key = NULL;
static Eina_Bool _generate_node(Egueb_Dom_Node *n, void *data);

static Egueb_Dom_String * _trantor_view_xml_key_get(void)
{
	if (!_trantor_view_xml_key)
		_trantor_view_xml_key = egueb_dom_string_new_with_string("_trantor_view_xml");
	return _trantor_view_xml_key;
}

static Egueb_Dom_String * _trantor_view_xml_element_key_get(void)
{
	if (!_trantor_view_xml_element_key)
		_trantor_view_xml_element_key = egueb_dom_string_new_with_string("_trantor_view_xml_element");
	return _trantor_view_xml_element_key;
}

static void _element_name_over_cb(Egueb_Dom_Event *e, void *data)
{
	Trantor_View_Xml *thiz = data;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *real;
	Egueb_Svg_Color color;

	target = egueb_dom_event_target_get(e);
	egueb_svg_color_components_from(&color, 0xff, 0x00, 0x00);
	egueb_svg_element_color_set(target, &color);

	real = egueb_dom_node_user_data_get(target, _trantor_view_xml_element_key_get());
	trantor_element_select(thiz->t, real);

	egueb_dom_node_unref(target);
}

static void _element_name_out_cb(Egueb_Dom_Event *e, void *data)
{
	Trantor_View_Xml *thiz = data;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *real;
	Egueb_Svg_Color color;

	target = egueb_dom_event_target_get(e);
	egueb_svg_color_components_from(&color, 0x00, 0x00, 0x00);
	egueb_svg_element_color_set(target, &color);

	real = egueb_dom_node_user_data_get(target, _trantor_view_xml_element_key_get());
	trantor_element_unselect(thiz->t, real);

	egueb_dom_node_unref(target);
}

/* FIXME we should enable this once we have the <tspan> implemented */
static void _attrs_create(Trantor_View_Xml_Generate_Data *data, Egueb_Dom_Node *n, Egueb_Dom_Node *parent)
{
	Egueb_Dom_Node_Map_Named *attrs = NULL;
	int count;
	int i;

	/* FIXME we moved the append here, because the node inserted is not propagating
	 * the event for the children nodes of the inserted node
	 */
	egueb_dom_node_child_append(data->thiz->svg, parent, NULL);

	attrs = egueb_dom_node_attributes_get(n);
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

		attr = egueb_dom_node_map_named_at(attrs, i);
		if (!attr) continue;
		if (!egueb_dom_attr_is_set(attr)) goto no_name;

		attr_name = egueb_dom_attr_name_get(attr);
		if (!attr_name) goto no_name;

		egueb_dom_attr_string_get(attr, EGUEB_DOM_ATTR_TYPE_BASE, &attr_value);
		if (!attr_value)
		{
			egueb_dom_string_unref(attr_name);
			goto no_name;
		}

		tnode = egueb_dom_text_new();
		egueb_dom_character_data_append_data_inline(tnode, " ", NULL);
		egueb_dom_node_child_append(parent, tnode, NULL);

		tspan = egueb_svg_element_tspan_new();
		font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
		egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_LENGTH_UNIT_PX);
		egueb_svg_element_font_size_set(tspan, &font_size);

		tnode = egueb_dom_text_new();
		egueb_dom_character_data_append_data(tnode, attr_name, NULL);
		egueb_dom_character_data_append_data_inline(tnode, "=\"", NULL);
		egueb_dom_character_data_append_data(tnode, attr_value, NULL);
		egueb_dom_character_data_append_data_inline(tnode, "\"", NULL);

		egueb_dom_node_child_append(tspan, tnode, NULL);
		egueb_dom_node_child_append(parent, tspan, NULL);
no_name:
		egueb_dom_node_unref(attr);
	}

	egueb_dom_node_map_named_free(attrs);
}

static void _tag_create(Trantor_View_Xml_Generate_Data *data, Egueb_Dom_Node *n, Eina_Bool open)
{
	Egueb_Dom_Node *text;
	Egueb_Dom_Node *tnode;
	Egueb_Dom_String *name;
	Egueb_Svg_Font_Size font_size;
	Egueb_Svg_Length x, y;

	name = egueb_dom_element_tag_name_get(n);
	text = egueb_svg_element_text_new();

	font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_font_size_set(text, &font_size);

	egueb_svg_length_set(&y, 12 * data->level, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_text_y_set(text, &y);
	egueb_svg_length_set(&x, 12 * data->tab, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_text_x_set(text, &x);
	tnode = egueb_dom_text_new();

	if (open)
	{
		Egueb_Dom_Node *child = NULL;
		Egueb_Svg_Color color;

		egueb_dom_character_data_append_data_inline(tnode, "<", NULL);
		egueb_dom_character_data_append_data(tnode, name, NULL);
		egueb_dom_node_child_append(text, tnode, NULL);

		/* set the default fill paint to current color and the color to black */
		egueb_svg_color_components_from(&color, 0, 0, 0);
		egueb_svg_element_color_set(text, &color);
		egueb_svg_element_fill_set(text, &EGUEB_SVG_PAINT_CURRENT_COLOR);
		/* add the needed callbacks */
		egueb_dom_node_user_data_set(text, _trantor_view_xml_element_key_get(), n);
		egueb_dom_node_event_listener_add(text, EGUEB_DOM_EVENT_MOUSE_OVER,
				_element_name_over_cb, EINA_TRUE, data->thiz);
		egueb_dom_node_event_listener_add(text, EGUEB_DOM_EVENT_MOUSE_OUT,
				_element_name_out_cb, EINA_TRUE, data->thiz);

		/* now the attributes */
		_attrs_create(data, n, text);
		//egueb_dom_node_child_append(thiz->svg, text);
		/* now the children */
		child = egueb_dom_node_child_first_get(n);

		/* another node for closing the tag */
		tnode = egueb_dom_text_new();
		if (child)
		{
			data->tab++;
			egueb_dom_character_data_append_data_inline(tnode, ">", NULL);
			egueb_dom_node_child_append(text, tnode, NULL);

			do
			{
				Egueb_Dom_Node *next = NULL;

				data->level++;
				_generate_node(child, data);
				next = egueb_dom_node_sibling_next_get(child);
				egueb_dom_node_unref(child);
				child = next;
			} while (child);
			data->tab--;

			data->level++;
			_tag_create(data, n, EINA_FALSE);
		}
		else
		{
			egueb_dom_character_data_append_data_inline(tnode, "/>", NULL);
			egueb_dom_node_child_append(text, tnode, NULL);
		}
	}
	else
	{
		egueb_dom_character_data_append_data_inline(tnode, "</", NULL);
		egueb_dom_character_data_append_data(tnode, name, NULL);
		egueb_dom_character_data_append_data_inline(tnode, ">", NULL);
		egueb_dom_node_child_append(text, tnode, NULL);
		egueb_dom_node_child_append(data->thiz->svg, text, NULL);
	}
}

static Eina_Bool _generate_element(Trantor_View_Xml_Generate_Data *data, Egueb_Dom_Node *n)
{
	_tag_create(data, n, EINA_TRUE);
	return EINA_TRUE;
}

static Eina_Bool _generate_text(Trantor_View_Xml_Generate_Data *data, Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *text;
	Egueb_Dom_Node *tnode;
	Egueb_Dom_String *text_data;
	Egueb_Svg_Font_Size font_size;
	Egueb_Svg_Length x, y;
	Egueb_Svg_Color color;

	text = egueb_svg_element_text_new();

	font_size.type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	egueb_svg_length_set(&font_size.value.length, 12, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_font_size_set(text, &font_size);

	egueb_svg_length_set(&y, 12 * data->level, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_text_y_set(text, &y);
	egueb_svg_length_set(&x, 12 * data->tab, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_text_x_set(text, &x);

	egueb_svg_color_components_from(&color, 0xff, 0, 0);
	egueb_svg_element_color_set(text, &color);
	egueb_svg_element_fill_set(text, &EGUEB_SVG_PAINT_CURRENT_COLOR);

	tnode = egueb_dom_text_new();
	text_data = egueb_dom_character_data_data_get(n);
	egueb_dom_character_data_append_data(tnode, text_data, NULL);

	egueb_dom_node_child_append(text, tnode, NULL);
	egueb_dom_node_child_append(data->thiz->svg, text, NULL);

	return EINA_TRUE;
}

static Eina_Bool _generate_node(Egueb_Dom_Node *n, void *user_data)
{
	Trantor_View_Xml_Generate_Data *data = user_data;
	Egueb_Dom_Node_Type type;

	type = egueb_dom_node_type_get(n);
	switch (type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		_generate_element(data, n);
		break;

		case EGUEB_DOM_NODE_TYPE_TEXT:
		_generate_text(data, n);
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE:
		case EGUEB_DOM_NODE_TYPE_CDATA_SECTION:
		case EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE:
		case EGUEB_DOM_NODE_TYPE_ENTITY:
		case EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
		case EGUEB_DOM_NODE_TYPE_COMMENT:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
		case EGUEB_DOM_NODE_TYPE_NOTATION:
		break;
	}
	return EINA_TRUE;
}

void trantor_view_xml_new(Trantor *t, Egueb_Dom_Node *xml_doc)
{
	Trantor_View_Xml *thiz;
	Trantor_View_Xml_Generate_Data data;
	Egueb_Dom_Node *other_svg, *svg;

	svg = egueb_svg_element_svg_new();
	egueb_svg_element_overflow_set(svg, EGUEB_SVG_OVERFLOW_HIDDEN);
	other_svg = t->svg;

	egueb_dom_document_element_set(xml_doc, egueb_dom_node_ref(svg));

	thiz = calloc(1, sizeof(Trantor_View_Xml));
	thiz->t = t;
	thiz->svg = svg;

	data.thiz = thiz;
	data.level = 1;
	data.tab = 0;
	_generate_node(other_svg,  &data);

	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(xml_doc);
}

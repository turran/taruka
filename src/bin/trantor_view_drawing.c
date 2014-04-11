#include <Trantor.h>

typedef struct _Trantor_View_Drawing
{
	Trantor *t;
	Egueb_Dom_Node *svg;
	Egueb_Dom_Node *bounds;
	Egueb_Dom_Node *other_doc;
} Trantor_View_Drawing;

static void _trantor_view_drawing_selected_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Trantor_View_Drawing *thiz = data;
	Egueb_Dom_Node *target;
	Egueb_Dom_Node *doc;
	Egueb_Svg_Length length;
	Eina_Rectangle bounds;
	double aw = 0, ah = 0, oaw = 0, oah = 0;
	double x = 0, y = 0;

	target = egueb_dom_event_target_get(ev);
	if (!egueb_svg_is_renderable(target))
		goto done;

	doc = egueb_dom_node_document_get(thiz->svg);
#if 0
	egueb_svg_document_actual_width_get(doc, &aw);
	egueb_svg_document_actual_height_get(doc, &ah);
#endif
	egueb_dom_node_unref(doc);

#if 0
	egueb_svg_document_actual_width_get(thiz->other_doc, &oaw);
	egueb_svg_document_actual_height_get(thiz->other_doc, &oah);
#endif
	printf("us %g %g other size %g %g\n", aw, ah, oaw, oah);

	if (aw > oaw)
		x = (aw - oaw) / 2;
	if (ah > oah)
		y = (ah - oah) / 2;

	egueb_svg_renderable_user_bounds_get(target, &bounds);
	//printf("bounds %" EINA_EXTRA_RECTANGLE_FORMAT "\n", EINA_EXTRA_RECTANGLE_ARGS(&bounds));

	egueb_svg_length_set(&length, bounds.x + x, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_rect_x_set(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.y + y, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_rect_y_set(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.w, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_rect_width_set(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.h, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_rect_height_set(thiz->bounds, &length);
done:
	egueb_dom_node_unref(target);
}

static void _trantor_view_drawing_unselected_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Trantor_View_Drawing *thiz = data;
	Egueb_Dom_Node *target;
	Eina_Rectangle bounds;

	target = egueb_dom_event_target_get(ev);
	if (!egueb_svg_is_renderable(target))
		goto done;

	thiz = data;
	egueb_svg_renderable_user_bounds_get(target, &bounds);
	//printf("bounds %" EINA_EXTRA_RECTANGLE_FORMAT "\n", EINA_EXTRA_RECTANGLE_ARGS(&bounds));
done:
	egueb_dom_node_unref(target);
}

void trantor_view_drawing_new(Trantor *t, Egueb_Dom_Node *xml_doc)
{
	Trantor_View_Drawing *thiz;
	Egueb_Dom_Node *other_svg, *svg;
	Egueb_Dom_Node *bounds;
	Egueb_Svg_Color color;
	Egueb_Svg_Length width;

	svg = egueb_svg_element_svg_new();
	other_svg = trantor_svg_get(t);

	egueb_dom_document_element_set(xml_doc, egueb_dom_node_ref(svg));
	/* our bounds rectangle */
	bounds = egueb_svg_element_rect_new();
	egueb_svg_color_components_from(&color, 0xff, 0x00, 0x00);
	egueb_svg_element_color_set(bounds, &color);
	egueb_svg_element_stroke_set(bounds, &EGUEB_SVG_PAINT_CURRENT_COLOR);
	egueb_svg_element_fill_set(bounds, &EGUEB_SVG_PAINT_NONE);
	egueb_svg_length_set(&width, 1, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_stroke_width_set(bounds, &width);

	egueb_dom_node_child_append(svg, bounds, NULL);

	thiz = calloc(1, sizeof(Trantor_View_Drawing));
	thiz->t = t;
	thiz->svg = svg;
	thiz->bounds = bounds;
	thiz->other_doc = egueb_dom_node_document_get(other_svg);

	/* register the selected/unselected event */
	egueb_dom_node_event_listener_add(other_svg, TRANTOR_EVENT_ELEMENT_SELECTED,
			_trantor_view_drawing_selected_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(other_svg, TRANTOR_EVENT_ELEMENT_UNSELECTED,
			_trantor_view_drawing_unselected_cb,
			EINA_FALSE, thiz);

	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(other_svg);
	egueb_dom_node_unref(xml_doc);
}


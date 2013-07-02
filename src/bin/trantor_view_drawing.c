#include <Trantor.h>

typedef struct _Trantor_View_Drawing
{
	Trantor *t;
	Egueb_Dom_Node *svg;
	Egueb_Dom_Node *bounds;
} Trantor_View_Drawing;

static void _trantor_view_drawing_selected_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Trantor_View_Drawing *thiz = data;
	Egueb_Dom_Node *target;
	Egueb_Svg_Length length;
	Eina_Rectangle bounds;

	egueb_dom_event_target_get(ev, &target);
	if (!egueb_svg_is_renderable(target))
		goto done;

	egueb_svg_renderable_user_bounds_get(target, &bounds);
	printf("bounds %" EINA_EXTRA_RECTANGLE_FORMAT "\n", EINA_EXTRA_RECTANGLE_ARGS(&bounds));

	egueb_svg_length_set(&length, bounds.x, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_rect_x_set(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.y, EGUEB_SVG_UNIT_LENGTH_PX);
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

	egueb_dom_event_target_get(ev, &target);
	if (!egueb_svg_is_renderable(target))
		goto done;

	thiz = data;
	egueb_svg_renderable_user_bounds_get(target, &bounds);
	printf("bounds %" EINA_EXTRA_RECTANGLE_FORMAT "\n", EINA_EXTRA_RECTANGLE_ARGS(&bounds));
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
	egueb_svg_length_set(&width, 2, EGUEB_SVG_UNIT_LENGTH_PX);
	egueb_svg_element_stroke_width_set(bounds, &width);

	egueb_dom_node_child_append(svg, bounds);

	thiz = calloc(1, sizeof(Trantor_View_Drawing));
	thiz->t = t;
	thiz->svg = svg;
	thiz->bounds = bounds;

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


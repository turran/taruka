#include <Trantor.h>

typedef struct _Trantor_View_Drawing
{
	Trantor *t;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *svg;
	Egueb_Dom_Node *preview_area;
	Egueb_Dom_Node *bounds;
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

	doc = egueb_dom_node_owner_document_get(thiz->svg);
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

	egueb_svg_length_set(&length, bounds.x + x, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_rect_x_set_simple(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.y + y, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_rect_y_set_simple(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.w, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_rect_width_set_simple(thiz->bounds, &length);
	egueb_svg_length_set(&length, bounds.h, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_rect_height_set_simple(thiz->bounds, &length);
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

Egueb_Dom_Node * trantor_view_drawing_new(Trantor *t)
{
	Trantor_View_Drawing *thiz;
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *other_svg;
	Egueb_Svg_Color color;
	Egueb_Svg_Length l;

	thiz = calloc(1, sizeof(Trantor_View_Drawing));
	thiz->t = t;

	other_svg = t->svg;
	/* create a svg doc like:
	 * <svg>
	 *   <image id="previewArea"> <!-- the original document is embedded -->
	 *   <rect id="page"/>
	 *   <rect id="pageShadow"/>
	 * </svg>
	 */
	thiz->doc = egueb_svg_document_new();

	thiz->svg = egueb_svg_element_svg_new();
	egueb_dom_node_child_append(thiz->doc, egueb_dom_node_ref(thiz->svg), NULL);

	thiz->preview_area = egueb_svg_element_image_new();
	egueb_svg_element_image_svg_set(thiz->preview_area, egueb_dom_node_ref(other_svg));

	egueb_dom_node_child_append(thiz->svg, egueb_dom_node_ref(thiz->preview_area), NULL);

	n = egueb_svg_element_rect_new();
	/* some helpful items */
	/* our bounds rectangle */
	thiz->bounds = egueb_svg_element_rect_new();
	egueb_svg_color_components_from(&color, 0xff, 0x00, 0x00);
	egueb_svg_element_color_set(thiz->bounds, &color);
	egueb_svg_element_stroke_set(thiz->bounds, &EGUEB_SVG_PAINT_CURRENT_COLOR);
	egueb_svg_element_fill_set(thiz->bounds, &EGUEB_SVG_PAINT_NONE);
	egueb_svg_length_set(&l, 1, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_stroke_width_set(thiz->bounds, &l);
	egueb_svg_length_set(&l, 99.9, EGUEB_SVG_LENGTH_UNIT_PERCENT);
	egueb_svg_element_rect_width_set_simple(thiz->bounds, &l);
	egueb_svg_element_rect_height_set_simple(thiz->bounds, &l);
	egueb_dom_node_child_append(thiz->svg, egueb_dom_node_ref(thiz->bounds), NULL);

	/* register the selected/unselected event */
	egueb_dom_event_target_event_listener_add(other_svg, TRANTOR_EVENT_ELEMENT_SELECTED,
			_trantor_view_drawing_selected_cb,
			EINA_FALSE, thiz);
	egueb_dom_event_target_event_listener_add(other_svg, TRANTOR_EVENT_ELEMENT_UNSELECTED,
			_trantor_view_drawing_unselected_cb,
			EINA_FALSE, thiz);

	/* our own widget */
	n = eon_element_object_new();
	eon_element_object_document_set(n, egueb_dom_node_ref(thiz->doc));
	return n;
}

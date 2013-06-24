#include "Trantor.h"
#include <stdio.h>
#include <getopt.h>

static void help(const char *name)
{
	printf("Usage: %s [OPTIONS] FILE\n", name);
	printf("Where OPTIONS can be one of the following:\n");
	printf("-h Print this screen\n");
	printf("-d Draw the damage retangles\n");
	printf("-w The width of the window\n");
	printf("-e The height of the window\n");
	printf("-n The evas engine to use (list to list them all)\n");
	printf("And FILE can be a SVG file or a directory\n");
}

static void _cb_delete(Ecore_Evas *ee EINA_UNUSED)
{
	ecore_main_loop_quit();
}

static void _cb_resize(Ecore_Evas *ee)
{
	Evas *evas;
	Evas_Object *o;
	int width;
	int height;

	ecore_evas_geometry_get(ee, NULL, NULL, &width, &height);
	evas = ecore_evas_get(ee);

	/* resize the svg objects */
	o = evas_object_name_find(evas, "svg");
	evas_object_resize(o, width/2, height);

	o = evas_object_name_find(evas, "xml");
	evas_object_move(o, width/2, 0);
	evas_object_resize(o, width/2, height);
}

int main(int argc, char *argv[])
{
	Trantor *thiz;
	Egueb_Dom_Node *xml_doc;
	Ecore_Evas *ee;
	Evas *evas;
	Evas_Object *o;
	Eina_Bool damages;
	char *short_options = "dhw:e:n:f:";
	struct option long_options[] = {
		{ "help", 1, 0, 'h' },
		{ "damages", 0, 0, 'd' },
		{ "fps", 1, 0, 'f' },
		{ "width", 1, 0, 'w' },
		{ "height", 1, 0, 'e' },
		{ "engine", 1, 0, 'n' },
	};
	int option;
	int ret;
	const char *filename;
	char *engine;
	int width;
	int height;
	int fps;
	struct stat st;

	/* default options */
	width = 640;
	height = 480;
	fps = 30;
	damages = EINA_FALSE;
#ifdef _WIN32
	engine = "software_gdi";
#else
	engine = "software_x11";
#endif

	if (!ecore_evas_init())
		return -1;

	if (!efl_svg_init())
		goto shutdown_ecore_evas;

	while ((ret = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (ret)
		{
			case 'h':
			help(argv[0]);
			return 0;

			case 'd':
			damages = EINA_TRUE;
			break;

			case 'f':
			fps = atoi(optarg);
			break;

			case 'w':
			width = atoi(optarg);
			break;

			case 'e':
			height = atoi(optarg);
			break;

			case 'n':
			if (!strcmp(optarg, "list"))
			{
				Eina_List *engines;
				Eina_List *l;
				const char *e;

				engines = ecore_evas_engines_get();
				EINA_LIST_FOREACH (engines, l, e)
				{
					printf("%s\n", e);
				}
				return 0;
			}
			else
				engine = optarg;
			break;

			default:
			break;
		}
	}

	if (argc - 1 != optind)
	{
		help(argv[0]);
		return 0;
	}

	filename = argv[optind];
	if (stat(filename, &st) < 0)
	{
		help(argv[0]);
		return 0;
	}

	ee = ecore_evas_new(engine, 0, 0, 0, 0, NULL);
	if (!ee)
		goto shutdown_esvg;

	evas = ecore_evas_get(ee);
	if (!evas)
		goto free_ecore_evas;

	thiz = calloc(1, sizeof(Trantor));
	evas_data_attach_set(evas, thiz);

	ecore_evas_callback_delete_request_set(ee, _cb_delete);
	ecore_evas_callback_resize_set(ee, _cb_resize);

	/* create the main svg object */
	o = efl_svg_new(evas);
	efl_svg_file_set(o, filename);
	efl_svg_debug_damage_set(o, damages);
	efl_svg_fps_set(o, fps);
	evas_object_move(o, 0, 0);
	evas_object_resize(o, width/2, height);
	evas_object_show(o);
	evas_object_name_set(o, "svg");
	thiz->doc_svg = efl_svg_document_get(o);
	thiz->o_svg = o;

	/* create the xml object */
	o = efl_svg_new(evas);
	xml_doc = efl_svg_document_get(o);
	trantor_view_xml_new(thiz, xml_doc);
	evas_object_move(o, width/2, 0);
	evas_object_resize(o, width/2, height);
	evas_object_show(o);
	evas_object_name_set(o, "xml");
	thiz->o_xml = o;

	ecore_evas_resize(ee, width, height);
	ecore_evas_show(ee);

	ecore_main_loop_begin();

	ecore_evas_shutdown();
	efl_svg_shutdown();

	egueb_dom_node_unref(thiz->doc_svg);
	free(thiz);

	return 0;

free_ecore_evas:
	ecore_evas_free(ee);
shutdown_esvg:
	efl_svg_shutdown();
shutdown_ecore_evas:
	ecore_evas_shutdown();

	return -1;
}

Egueb_Dom_Node * trantor_svg_get(Trantor *thiz)
{
	Egueb_Dom_Node *ret;

	egueb_dom_document_element_get(thiz->doc_svg, &ret);
	return ret;
}

void trantor_element_select(Trantor *thiz, Egueb_Dom_Node *n)
{
	Eina_Rectangle bounds;

#if 0
	Egueb_Dom_Node *topmost;
	/* check that the element topmost element is the same as the main one */
	egueb_dom_document_element_get(thiz->doc_svg, &topmost);
#endif
	/* get the bounds and add a rectangle object with such bounds
	 * on the drawing area
	 */
	if (!egueb_svg_is_renderable(n))
		return;

	egueb_svg_renderable_user_bounds_get(n, &bounds);
	printf("bounds %" EINA_RECTANGLE_FORMAT, EINA_RECTANGLE_ARGS(&bounds));
}

void trantor_element_unselect(Trantor *thiz, Egueb_Dom_Node *n)
{
	Eina_Rectangle bounds;

	if (!egueb_svg_is_renderable(n))
		return;

	egueb_svg_renderable_user_bounds_get(n, &bounds);
	printf("bounds %" EINA_RECTANGLE_FORMAT, EINA_RECTANGLE_ARGS(&bounds));
}

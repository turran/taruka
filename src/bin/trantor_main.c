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
	printf("And FILE can be a SVG file or a directory\n");
}

static void _trantor_init(void)
{
	eon_init();
	TRANTOR_EVENT_ELEMENT_SELECTED = egueb_dom_string_new_with_string("TrantorElementSelected");
	TRANTOR_EVENT_ELEMENT_UNSELECTED = egueb_dom_string_new_with_string("TrantorElementUnselected");
}

static void _trantor_shutdown(void)
{
	egueb_dom_string_unref(TRANTOR_EVENT_ELEMENT_SELECTED);
	egueb_dom_string_unref(TRANTOR_EVENT_ELEMENT_UNSELECTED);
	eon_shutdown();
}

static void _trantor_ui_setup(Trantor *thiz)
{
	Egueb_Dom_Node *p, *n;

	/* setup one view */
	n = trantor_view_drawing_new(thiz);
	p = eon_element_stack_new();
	egueb_dom_node_child_append(p, n, NULL);
	n = p;

	p = eon_element_eon_new();
	egueb_dom_node_child_append(p, n, NULL);
	n = p;

	egueb_dom_node_child_append(thiz->doc, n, NULL);
}

static Trantor * _trantor_new(const char *filename, int width, int height)
{
	Trantor *thiz;
	Egueb_Dom_Node *doc = NULL;
	Enesim_Stream *s;

	thiz = calloc(1, sizeof(Trantor));
	s = enesim_stream_file_new(filename, "r");
	if (!s)
	{
		free(thiz);
		return NULL;
	}

	egueb_dom_parser_parse(s, &doc);
	if (!doc)
	{
		free(thiz);
		return NULL;
	}

	/* create our own eon widgets */
	thiz->doc = eon_document_new();
	thiz->svg = egueb_dom_document_document_element_get(doc);
	egueb_dom_node_unref(doc);
	_trantor_ui_setup(thiz);

	thiz->window = efl_egueb_window_auto_new(egueb_dom_node_ref(thiz->doc),
			0, 0, width, height);

	if (!thiz->window)
	{
		egueb_dom_node_unref(thiz->doc);
		free(thiz);
		return NULL;
	}


	return thiz;
}

static void _trantor_free(Trantor *thiz)
{
	egueb_dom_node_unref(thiz->doc);
	egueb_dom_node_unref(thiz->svg);
	efl_egueb_window_free(thiz->window);
	free(thiz);
}

int main(int argc, char *argv[])
{
	Trantor *thiz;
	Eina_Bool damages;
	char *short_options = "dhw:e:f:";
	struct option long_options[] = {
		{ "help", 1, 0, 'h' },
		{ "damages", 0, 0, 'd' },
		{ "fps", 1, 0, 'f' },
		{ "width", 1, 0, 'w' },
		{ "height", 1, 0, 'e' },
	};
	int option;
	int ret;
	const char *filename;
	int width;
	int height;
	int fps;

	/* default options */
	width = 640;
	height = 480;
	fps = 30;
	damages = EINA_FALSE;

	if (!efl_egueb_init())
		return -1;

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

	_trantor_init();
	thiz = _trantor_new(filename, width, height);
	if (!thiz)
	{
		_trantor_shutdown();
		return 0;
	}

	ecore_main_loop_begin();

	_trantor_free(thiz);
	_trantor_shutdown();

	efl_egueb_shutdown();

	return 0;
}

Egueb_Dom_String *TRANTOR_EVENT_ELEMENT_SELECTED;
Egueb_Dom_String *TRANTOR_EVENT_ELEMENT_UNSELECTED;

void trantor_element_select(Trantor *thiz, Egueb_Dom_Node *n)
{
	Egueb_Dom_Event *ev;
#if 0
	Egueb_Dom_Node *topmost;
	/* check that the element topmost element is the same as the main one */
	egueb_dom_document_document_element_get(thiz->doc_svg, &topmost);
	/* get the bounds and add a rectangle object with such bounds
	 * on the drawing area
	 */
#endif
#if 0
	ev = egueb_dom_event_external_new(TRANTOR_EVENT_ELEMENT_SELECTED,
			EINA_TRUE, EINA_FALSE, NULL, NULL);
	egueb_dom_node_event_dispatch(n, ev, NULL);
#endif
}

void trantor_element_unselect(Trantor *thiz, Egueb_Dom_Node *n)
{
#if 0
	Egueb_Dom_Event *ev;
	ev = egueb_dom_event_external_new(TRANTOR_EVENT_ELEMENT_UNSELECTED,
			EINA_TRUE, EINA_FALSE, NULL, NULL);
	egueb_dom_node_event_dispatch(n, ev, NULL);
#endif
}

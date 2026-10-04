/*
 * File "s_manifest.c" generated from compiled manifest
. * 2026-10-04 09:26:13
 * Do not modify this file.
 */

#include "s_manifest.h"

const char* SHADOW_ERROR_STRING_FAILURE = "Failed to %s %s.";
const char* SHADOW_ERROR_STRING_NULL_POINTER = "NULL pointer error: \"%s\".";

int32_t s_manifest_install_allegro(void)
{
	if (!al_init())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "initialize", "Allegro library");
		return -1;
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "install", "keyboard");
		return -1;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "install", "mouse");
		return -1;
	}

	if (!al_init_font_addon())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "initialize", "font");
		return -1;
	}

	if (!al_init_image_addon())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "initialize", "image");
		return -1;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "initialize", "primitives");
		return -1;
	}

	return 0;
}

void s_manifest_uninstall_allegro(void)
{
	if (al_is_font_addon_initialized())
	{
		al_shutdown_font_addon();
	}

	if (al_is_image_addon_initialized())
	{
		al_shutdown_image_addon();
	}

	if (al_is_primitives_addon_initialized())
	{
		al_shutdown_primitives_addon();
	}

	if (al_is_keyboard_installed())
	{
		al_uninstall_keyboard();
	}

	if (al_is_mouse_installed())
	{
		al_uninstall_mouse();
	}

	if (al_is_system_installed())
	{
		al_uninstall_system();
	}
}

void s_manifest_zero_data(s_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_NULL_POINTER, "data");
		return;
	}

	data->m_display = NULL;
	data->m_timer = NULL;
	data->m_event_queue = NULL;
}

int32_t s_manifest_initialize_data(s_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_NULL_POINTER, "data");
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_OPENGL|ALLEGRO_RESIZABLE|ALLEGRO_PROGRAMMABLE_PIPELINE);
	al_set_new_window_title(SHADOW_TITLE);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
	data->m_display = al_create_display(SHADOW_DISPLAY_WIDTH, SHADOW_DISPLAY_HEIGHT);
	if (NULL == data->m_display)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "create", "display");
		return -1;
	}

	data->m_timer = al_create_timer(1.0/SHADOW_TIMER_SPEED);
	if (NULL == data->m_timer)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "create", "timer");
		return -1;
	}

	data->m_event_queue = al_create_event_queue();
	if (NULL == data->m_event_queue)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_FAILURE, "create", "event queue");
		return -1;
	}

	al_register_event_source(data->m_event_queue, al_get_display_event_source(data->m_display));
	al_register_event_source(data->m_event_queue, al_get_timer_event_source(data->m_timer));
	al_register_event_source(data->m_event_queue, al_get_keyboard_event_source());
	al_register_event_source(data->m_event_queue, al_get_mouse_event_source());

	return 0;
}

void s_manifest_uninitialize_data(s_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, SHADOW_ERROR_STRING_NULL_POINTER, "data");
		return;
	}

	if (NULL != data->m_event_queue)
	{
		al_destroy_event_queue(data->m_event_queue);
		data->m_event_queue = NULL;
	}

	if (NULL != data->m_timer)
	{
		al_destroy_timer(data->m_timer);
		data->m_timer = NULL;
	}

	if (NULL != data->m_display)
	{
		al_destroy_display(data->m_display);
		data->m_display = NULL;
	}
}

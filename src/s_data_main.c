#include "s_common.h"

void s_data_main_zero(s_data_main_t* data)
{
	if (NULL == data)
	{
		return;
	}

    data->m_display = NULL;
    data->m_timer = NULL;
    data->m_event_queue = NULL;
    data->m_input = NULL;
    data->m_render = NULL;
    data->m_delta_time = 0.0;
    data->m_fps = 0.0;
    data->m_update = false;
    data->m_draw_stats = false;
    data->m_running = false;
    data->m_draw_mouse = false;
}

int32_t s_data_main_initialize(s_data_main_t* data)
{
	if (!data)
	{
		return -1;
	}

	if (s_manifest_install_allegro() != 0)
	{
		return -1;
	}

#ifdef _DEBUG
	ogle_log_open("output_log.txt");
#endif

	if (s_manifest_initialize_data((s_manifest_data_t*)data) != 0)
	{
		return -1;
	}

	ALLEGRO_BITMAP* icon = al_load_bitmap("assets/images/icon.png");
	if (icon)
	{
		al_convert_mask_to_alpha(icon, al_map_rgb(0xff, 0x00, 0xff));
		al_set_display_icon(data->m_display, icon);
		al_destroy_bitmap(icon);
	}
	al_clear_to_color(al_map_rgb(0x16, 0x16, 0x21));
	al_flip_display();

	srand((unsigned int)time(NULL));

	data->m_input = ogle_object_create(ogle_input_size(), &ogle_input_initializer, NULL);
	if (!data->m_input)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create input");
		return -1;
	}

	s_data_render_zero(data->m_render);
	if (s_data_render_initialize(data->m_render) != 0)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to initialize render data");
		return -1;
	}

	al_hide_mouse_cursor(data->m_display);

	return 0;
}


void s_data_main_uninitialize(s_data_main_t* data)
{
	if (!data)
	{
		return;
	}

	if (data->m_display)
	{
		al_show_mouse_cursor(data->m_display);
	}

	s_data_render_uninitialize(data->m_render);

	if (data->m_input)
	{
		ogle_object_destroy(data->m_input, &ogle_input_uninitializer);
		data->m_input = NULL;
	}

	s_manifest_uninitialize_data((s_manifest_data_t*)data);

	s_manifest_uninstall_allegro();

	ogle_log_print("\nTotal manual memory data:\n");
	ogle_log_printf("\tAllocations: %d (%zu)\n", ogle_object_total_allocations(), ogle_object_memory_allocated());
	ogle_log_printf("\tDeallocations: %d (%zu)\n", ogle_object_total_deallocations(), ogle_object_memory_freed());
	ogle_log_printf("\tNet: %zu\n", ogle_object_memory_allocated() - ogle_object_memory_freed());
	ogle_log_printf("\nGoodbye!\n\n");
}

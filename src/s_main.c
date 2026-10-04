#include "s_common.h"

static int32_t s_main_initialize(s_data_main_t* data_main)
{
	if (NULL == data_main)
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

	if (s_manifest_initialize_data((s_manifest_data_t*)data_main) != 0)
	{
		return -1;
	}

	ALLEGRO_BITMAP* icon = al_load_bitmap("assets/images/icon.png");
	if (icon)
	{
		al_convert_mask_to_alpha(icon, al_map_rgb(0xff, 0x00, 0xff));
		al_set_display_icon(data_main->m_display, icon);
		al_destroy_bitmap(icon);
	}
	al_clear_to_color(al_map_rgb(0x16, 0x16, 0x21));
	al_flip_display();

	srand((unsigned int)time(NULL));

	data_main->m_input = ogle_object_create(ogle_input_size(), &ogle_input_initializer, NULL);
	if (NULL == data_main->m_input)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create input");
		return -1;
	}

	s_data_render_zero(data_main->m_render);
	if (s_data_render_initialize(data_main->m_render) != 0)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to initialize render data_main");
		return -1;
	}

	al_hide_mouse_cursor(data_main->m_display);

	return 0;
}

static void s_main_cleanup(s_data_main_t* data_main)
{
	if (NULL == data_main)
	{
		return;
	}

	if (data_main->m_display)
	{
		al_show_mouse_cursor(data_main->m_display);
	}

	s_data_render_uninitialize(data_main->m_render);

	if (data_main->m_input)
	{
		ogle_object_destroy(data_main->m_input, &ogle_input_uninitializer);
		data_main->m_input = NULL;
	}

	s_manifest_uninitialize_data((s_manifest_data_t*)data_main);

	s_manifest_uninstall_allegro();

	ogle_log_print("\nTotal manual memory data_main:\n");
	ogle_log_printf("\tAllocations: %d (%zu)\n", ogle_object_total_allocations(), ogle_object_memory_allocated());
	ogle_log_printf("\tDeallocations: %d (%zu)\n", ogle_object_total_deallocations(), ogle_object_memory_freed());
	ogle_log_printf("\tNet: %zu\n", ogle_object_memory_allocated() - ogle_object_memory_freed());
	ogle_log_printf("\nGoodbye!\n\n");
}

static void s_main_input(s_data_main_t* data_main)
{
	if (NULL == data_main)
	{
		return;
	}

	ALLEGRO_EVENT event = { 0 };
	o_input_mouse_t* mouse_input = ogle_input_get_type(data_main->m_input, OGLE_INPUT_TYPE_MOUSE);
	o_input_keyboard_t* keyboard_input = ogle_input_get_type(data_main->m_input, OGLE_INPUT_TYPE_KEYBOARD);

	while (!al_is_event_queue_empty(data_main->m_event_queue))
	{
		al_get_next_event(data_main->m_event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			data_main->m_update = true;
		} break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			data_main->m_running = false;
		} break;
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			al_acknowledge_resize(data_main->m_display);
		} break;
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			mouse_input->m_position = (o_vector2_t){ (float)event.mouse.x, (float)event.mouse.y };
			mouse_input->m_changed = true;

		} break;
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
		{
			data_main->m_draw_mouse = false;
		} break;
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
		{
			data_main->m_draw_mouse = true;
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			int32_t button_index = event.mouse.button - 1;

			if (button_index >= 0 && button_index < OGLE_INPUT_MOUSE_MAX_BUTTONS)
			{
				mouse_input->m_button[button_index] = OGLE_INPUT_BUTTON_FLAG_CHANGED;
				mouse_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			int32_t button_index = event.mouse.button - 1;

			if (button_index >= 0 && button_index < OGLE_INPUT_MOUSE_MAX_BUTTONS)
			{
				mouse_input->m_button[button_index] = (OGLE_INPUT_BUTTON_FLAG_PRESSED | OGLE_INPUT_BUTTON_FLAG_CHANGED);
				mouse_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_KEY_UP:
		{
			int32_t button_index = event.keyboard.keycode;

			if (button_index >= 0 && button_index < OGLE_INPUT_KEYBOARD_MAX_BUTTONS)
			{
				keyboard_input->m_button[button_index] = OGLE_INPUT_BUTTON_FLAG_PRESSED;
				keyboard_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			int32_t button_index = event.keyboard.keycode;

			if (button_index >= 0 && button_index < OGLE_INPUT_KEYBOARD_MAX_BUTTONS)
			{
				keyboard_input->m_button[button_index] = (OGLE_INPUT_BUTTON_FLAG_PRESSED | OGLE_INPUT_BUTTON_FLAG_CHANGED);
				keyboard_input->m_changed = true;
			}

		} break;
		default: break;
		}
	}
}

static void s_main_update(s_data_main_t* data_main)
{
	if (NULL == data_main)
	{
		return;
	}

	const o_input_keyboard_t* keyboard_input = ogle_input_get_type(data_main->m_input, OGLE_INPUT_TYPE_KEYBOARD);

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_ESCAPE))
	{
		data_main->m_running = false;
	}

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_F2))
	{
		al_save_bitmap("shadow_map_debug.png", data_main->m_render->m_shadow_map);
	}

	ogle_input_reset(data_main->m_input);

	float x = cos(data_main->m_render->m_light_angle) * 20.0f;
	float z = sin(data_main->m_render->m_light_angle) * 20.0f;

	data_main->m_render->m_light.m_position = (o_vector3_t){ x, 20.0f, z };
	data_main->m_render->m_light_angle += 0.01f;
	if (data_main->m_render->m_light_angle > OGLE_MATH_TAU)
	{
		data_main->m_render->m_light_angle -= OGLE_MATH_TAU;
	}

}

static void s_main_render(const s_data_main_t* data_main)
{
	if (NULL == data_main)
	{
		return;
	}

	ALLEGRO_BITMAP* target = NULL;
	o_vector2_t resolution = { 0.0f, 0.0f };
	float aspect_ratio = 0.0f;
	o_vector2_t top_left = { 0.0f, 0.0f };
	o_vector2_t bottom_right = { 0.0f, 0.0f };
	o_vector3_t scale = { 1.0f, 1.0f, 1.0f };
	o_vector3_t position = { 0.0f, 0.0f, 0.0f };
	ALLEGRO_TRANSFORM light_projection_transform = { 0 };
	ALLEGRO_TRANSFORM light_view_transform = { 0 };
	o_transform_t model_transform;
	o_camera_t light_camera =
	{
		data_main->m_render->m_light.m_position,
		(o_vector3_t) { 0.0f, 0.0f, 0.0f },
		(o_vector3_t) { 0.0f, 1.0f, 0.0f }
	};

	target = al_get_target_bitmap();
	al_set_target_bitmap(data_main->m_render->m_shadow_map);

#define S_ORTHO_BOUNDS 20.0f
#define S_ORTHO_NEAR_PLANE 40.0f
#define S_ORTHO_FAR_PLANE -40.0f

	top_left = (o_vector2_t){ -S_ORTHO_BOUNDS, -S_ORTHO_BOUNDS };
	bottom_right = (o_vector2_t){ S_ORTHO_BOUNDS, S_ORTHO_BOUNDS };

	ogle_transform_identity(&light_projection_transform);
	ogle_transform_orthographic(&light_projection_transform, top_left, bottom_right, S_ORTHO_NEAR_PLANE, S_ORTHO_FAR_PLANE);
	ogle_transform_camera_build(&light_view_transform, &light_camera);
	ogle_transform_identity(&data_main->m_render->m_transform_model);	
	ogle_transform_identity(&model_transform);
	ogle_transform_scale_3d(&model_transform, scale);
	ogle_transform_translate_3d(&model_transform, position);
	ogle_transform_compose(&model_transform, &data_main->m_render->m_transform_model);

	al_clear_to_color(al_map_rgba(0xff, 0xff, 0xff, 0xff));
	al_clear_depth_buffer(1.0f);
	al_use_shader(data_main->m_render->m_shader[S_SHADER_ID_DEPTH]);
	ogle_light_set_shader("u_light", &data_main->m_render->m_light);
	ogle_camera_set_shader("u_camera", &light_camera);
	ogle_transform_set_shader("u_projection_matrix", &light_projection_transform);
	ogle_transform_set_shader("u_view_matrix", &light_view_transform);
	ogle_transform_set_shader("u_model_matrix", &model_transform);
	mod_model_render(data_main->m_render->m_vertex_decl, data_main->m_render->m_model_world, NULL);
	al_use_shader(NULL);

	al_set_target_bitmap(target);

	resolution = (o_vector2_t){ (float)al_get_bitmap_width(target), (float)al_get_bitmap_height(target) };
	aspect_ratio = resolution.m_y / resolution.m_x;
	top_left = (o_vector2_t){ -1.0f, aspect_ratio };
	bottom_right = (o_vector2_t){ 1.0f, -aspect_ratio };

	ogle_transform_identity(&data_main->m_render->m_transform_projection);
	ogle_transform_perspective(&data_main->m_render->m_transform_projection, top_left, bottom_right, 1.0f, 100.0f);
	ogle_transform_camera_build(&data_main->m_render->m_transform_view, &data_main->m_render->m_camera);
	ogle_transform_identity(&data_main->m_render->m_transform_model);

	ogle_transform_identity(&model_transform);
	ogle_transform_scale_3d(&model_transform, scale);
	ogle_transform_translate_3d(&model_transform, position);
	ogle_transform_compose(&model_transform, &data_main->m_render->m_transform_model);

	al_clear_to_color(al_map_rgb(0xff, 0xff, 0xff));
	al_clear_depth_buffer(1.0f);
	al_use_shader(data_main->m_render->m_shader[S_SHADER_ID_LIGHT]);
	al_set_shader_sampler("u_shadow_map", data_main->m_render->m_shadow_map, 1);
	ogle_transform_set_shader("u_light_projection", &light_projection_transform);
	ogle_transform_set_shader("u_light_view", &light_view_transform);
	ogle_light_set_shader("u_light", &data_main->m_render->m_light);
	ogle_camera_set_shader("u_camera", &data_main->m_render->m_camera);
	ogle_transform_set_shader("u_projection_matrix", &data_main->m_render->m_transform_projection);
	ogle_transform_set_shader("u_view_matrix", &data_main->m_render->m_transform_view);
	ogle_transform_set_shader("u_model_matrix", &model_transform);

	mod_model_render(data_main->m_render->m_vertex_decl, data_main->m_render->m_model_world, NULL);
	al_use_shader(NULL);

	al_flip_display();
}

static void s_main_loop(s_data_main_t* data_main)
{
	if (NULL == data_main)
	{
		return;
	}

	al_set_render_state(ALLEGRO_DEPTH_TEST, 1);

	al_start_timer(data_main->m_timer);

	double current_time = 0.0;
	double elapsed_time = 0;

	while (data_main->m_running)
	{
		current_time = al_get_time();

		s_main_input(data_main);

		if (data_main->m_update)
		{
			s_main_update(data_main);
		}

		s_main_render(data_main);

		al_rest(0.01);

		elapsed_time = al_get_time() - current_time;

		data_main->m_fps = 1.0 / elapsed_time;
	}

	al_stop_timer(data_main->m_timer);
	al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
}

int32_t main(int32_t argc, const char** argv)
{
	(void)argc;
	(void)argv;

	int32_t result = 0;

	s_data_render_t data_render = { 0 };
	s_data_main_t data_main = { 0 };

	s_data_main_zero(&data_main);

	data_main.m_render = &data_render;

	if ((result = s_main_initialize(&data_main)) == 0)
	{
		data_main.m_running = true;

		s_main_loop(&data_main);
	}

	s_main_cleanup(&data_main);

	return result;
}


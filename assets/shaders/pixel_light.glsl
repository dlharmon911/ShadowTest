#version 330 core

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Camera
{
	vec3 position;
	vec3 look_at;
	vec3 up;
};

in vec4 v_color;
in vec3 v_normal;
in vec3 v_frag_position;
in float v_meta;

uniform Light u_light;
uniform Camera u_camera;
uniform sampler2D u_shadow_map;
uniform mat4 u_light_view;
uniform mat4 u_light_projection;

out vec4 frag_color;

float unpack_depth(vec4 enc)
{
	return enc.r;
}

float shadow_calculation(vec3 world_pos, vec3 normal, vec3 light_dir)
{
	vec4 pos_ls = u_light_projection * u_light_view * vec4(world_pos, 1.0);
	vec3 proj = pos_ls.xyz / pos_ls.w;

	// NDC [-1,1] -> [0,1]; matches gl_FragCoord.z written in the depth pass
	proj = proj * 0.5 + 0.5;

	if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
	{
		return 0.0;
	}

	float stored_depth = unpack_depth(texture(u_shadow_map, proj.xy));
	float bias = max(0.001 * (1.0 - dot(normal, light_dir)), 0.001);

	return (proj.z - bias > stored_depth) ? 1.0 : 0.0;
}

void main()
{
	vec3 ambient = u_light.ambient;

	vec3 normal_vector = normalize(v_normal);
	vec3 light_direction = normalize(u_light.position - v_frag_position);
	float diffuse_intensity = max(dot(normal_vector, light_direction), 0.0);
	vec3 diffuse = u_light.diffuse * diffuse_intensity;

	vec3 view_direction = normalize(u_camera.position - v_frag_position);
	vec3 reflect_direction = reflect(-light_direction, normal_vector);
	float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), 32.0);
	vec3 specular = u_light.specular * specular_intensity;

	float shadow = shadow_calculation(v_frag_position, normal_vector, light_direction);

	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * v_color.rgb;
	frag_color = vec4(result, v_color.a);
}

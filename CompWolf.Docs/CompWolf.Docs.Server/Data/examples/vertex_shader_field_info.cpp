#include <dimensions> // float2
#include <shaders> // shader_field_info, new_shader_field
#include <utility> // offsetof

struct vertex
{
    float2 position;
    float2 uv;
};
template<> struct shader_field_info<vertex> : public new_shader_field<
    type_value_pair<float2, offsetof(vertex, position)>,
    type_value_pair<float2, offsetof(vertex, uv)>
> {};
uniform highp mat4 u_ProjectionMatrix;
uniform highp mat4 u_ViewMatrix;
uniform highp mat4 u_ModelMatrix;

attribute highp vec4 a_position;
attribute highp vec2 a_textcoord;
attribute highp vec3 a_normal;

varying highp vec4 v_position;
varying highp vec2 v_textcoord;
varying highp vec3 v_normal;

void main(void)
{
    mat4 mv_matrix = u_ViewMatrix * u_ModelMatrix;
    gl_Position = u_ProjectionMatrix * mv_matrix * a_position;
    v_textcoord = a_textcoord;
    v_normal = normalize(vec3(mv_matrix * vec4(a_normal, 0.0f)));
    v_position = mv_matrix * a_position;
}

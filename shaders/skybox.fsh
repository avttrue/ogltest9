uniform sampler2D u_Texture;
varying highp vec2 v_textcoord;

void main(void)
{
    gl_FragColor = texture2D(u_Texture, v_textcoord);
}

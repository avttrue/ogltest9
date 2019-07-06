struct MaterialProperty
{
    vec3 DiffuseColor;
    vec3 AmbienceColor;
    vec3 SpecularColor;
    float Shines;
};

uniform bool u_IsUseDiffuseMap;
uniform MaterialProperty u_MaterialProperty;
uniform sampler2D u_Texture;
uniform highp vec4 u_LightPosition;
uniform highp float u_LightPower; // сила освещения
varying highp vec4 v_position;
varying highp vec2 v_textcoord;
varying highp vec3 v_normal;

void main(void)
{
    vec4 resultColor = vec4(0.0, 0.0, 0.0, 0.0); // результирующий цвет чёрный
    vec4 eyePosition = vec4(0.0, 0.0, 0.0, 1.0); // позиция наблюдателя
    vec4 diffMatColor = texture2D(u_Texture, v_textcoord); // диффузный цвет
    vec3 eyeVec = normalize(v_position.xyz - eyePosition.xyz); // направление взгляда
    vec3 lightVec = normalize(v_position.xyz - u_LightPosition.xyz); // вектор освещения
    vec3 reflectLight = normalize(reflect(lightVec, v_normal)); // отражённый свет
    float len = length(v_position.xyz - eyePosition.xyz); // расстояние от наблюдателя до точки
    float specularFactor = u_MaterialProperty.Shines; // размер пятна блика
    float ambientFactor = 0.1; // светимость материала
    vec4 reflectionColor = vec4(1.0, 1.0, 1.0, 1.0); //цвет блика белый

    if(u_IsUseDiffuseMap == false) diffMatColor = vec4(u_MaterialProperty.DiffuseColor, 1.0);

    vec4 diffColor = diffMatColor * u_LightPower * max(0.0, dot(v_normal, -lightVec));
    resultColor += diffColor;

    vec4 ambientColor = ambientFactor * diffMatColor;
    resultColor += ambientColor * vec4(u_MaterialProperty.AmbienceColor, 1.0);

    vec4 specularColor = reflectionColor * u_LightPower *
            pow(max(0.0, dot(reflectLight, -eyeVec)), specularFactor);
    resultColor += specularColor * vec4(u_MaterialProperty.SpecularColor, 1.0);

    gl_FragColor = resultColor;
}

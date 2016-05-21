//uniform vec3 lightPos;
uniform vec4 eyepos;
uniform float roughness;
uniform mat4 g_rotation;
uniform samplerCube cubemap;
varying vec3 normal;
varying vec3 ecPos; //lightDir
varying vec3 reflectvec; //cube map reflect vector
varying vec3 Precolor;

void main()  
{  
    /* these are the new lines of code to compute the light's direction */  
    vec4 ecPos1 = gl_ModelViewMatrix * gl_Vertex;  
    ecPos = (ecPos1 / ecPos1.w).xyz;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();  

    vec4 pos = normalize(gl_ModelViewMatrix * gl_Vertex);
    pos = pos / pos.w;
	vec3 eyevec = normalize(eyepos.xyz - pos.xyz);
	reflectvec = reflect(-eyevec, normal);
    //vec4 veyePos = (gl_ModelViewMatrix * eyepos);
    //vec4 pos = ecPos / ecPos.w;
    //eyedir  = normalize(veyePos.xyz - pos.xyz);
    
    //CubeMap
    float NoRef = min(0.99,max(dot(normal,reflectvec),0.01));
    float miplev;
    if(roughness<=0.5)
        miplev = roughness*10.0;
    else if(roughness>0.5&&roughness<1.5)
        miplev = (roughness-0.5)+5.0;
    else
        miplev = roughness*2.0 + 3.0;
    vec3 rot_ref = vec3(g_rotation * vec4(reflectvec,0.0));
    Precolor = textureCubeLod(cubemap, rot_ref, miplev).rgb * NoRef;
}  

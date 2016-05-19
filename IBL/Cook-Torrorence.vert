//uniform vec3 lightPos;
uniform vec4 eyepos;
varying vec3 normal;
varying vec3 ecPos; //lightDir
varying vec3 reflectvec; //cube map reflect vector
  
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
}  

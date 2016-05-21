uniform vec3 rf,ambient;
uniform float roughness;
uniform int lightNum,iftex;
uniform sampler2D tex;
uniform sampler2D env_brdf;
//uniform mat4 g_rotation;
//uniform sampler2D env_map;
//uniform samplerCube cubemap;
varying vec3 reflectvec;
//uniform float density;
varying vec3 normal;
varying vec3 ecPos;
varying vec3 Precolor;

vec3 CTbrdf(vec3 n, vec3 l, vec3 v){
	if(dot(n,l)<0.0)
		return vec3(0.0,0.0,0.0);
	vec3 F,h,brdf,unit;
	float D,Den,G,GD;
	float a = roughness*roughness;//roughness;
	float ndotv,ndoth,ndotl,vdoth,ldoth;
	unit = vec3(1.0,1.0,1.0);
	h = normalize(v+l);
	ndotv = max(dot(n,v),0.0);
	ndoth = max(dot(n,h),0.0);
	ndotl = max(dot(n,l),0.0);
	vdoth = max(dot(v,h),0.0);
	ldoth = max(dot(l,h),0.0);
	Den = 4.0*ndotv;//*ndotl
	if(Den==0.0) Den=1.0;
	//Cook-Torrance
	if(vdoth!=0.0)
		G =min(1.0, min(2.0*ndoth*ndotv/vdoth,2.0*ndoth*ndotl/vdoth));
	else
		G = 1.0;
	if(ndotv!=0.0)
		GD = G/Den;
	else
		GD = ndoth/vdoth/2.0;
	//GGX
	if(ndoth!=0.0)
		D = pow((a/(pow(ndoth,2.0)*(a*a-1.0)+1.0)),2.0)/3.1415926;
	else
		D = 1.0;
    //Schilick Approximation
    F = rf + pow((1.0-ldoth),5.0)*(unit - rf);
    brdf = D*GD*F;//*ndotl
    return brdf;
}

void main(){
	vec3 view, norm, mapv;
    vec3 cf,ct;
    float af,at,Distance, Attenuation;
    float constant = 1.0;
    float linear = 0.5;
    float expo = 0.5;
    float invpi = 1.0/3.1415926;
    int i;
    vec3 brdf;
    vec3 aux,lightDir;
    vec4 texel;

    norm = normalize(normal);
    view = normalize(vec3(0.0,0.0,0.0)-ecPos);

    //vec3 Precolor = PrefilterEnvMap(roughness,reflectvec);

    //Enviornment Mapping Calculasion
    mapv = normalize(reflect(view,norm));

    //Diffuse term
    cf = gl_LightSource[0].diffuse.rgb*invpi;

    //Specular term
    lightDir = normalize(gl_LightSource[0].position.xyz); 
    brdf = CTbrdf(norm,lightDir,view);
    cf += brdf*gl_LightSource[0].diffuse.rgb;

    //Imaged based lighting
    float NoV = min(0.99,max(dot(norm,view),0.01));
    vec4 ibl =  texture2D(env_brdf, vec2(1.0-NoV,roughness*0.5));
    cf += gl_LightSource[0].ambient.rgb*(ibl.r*rf + ibl.g);

    cf = Precolor.rgb*cf;

    //Texture
    if(iftex==-1)
        gl_FragColor = vec4(cf,1.0);
    else{
        texel = texture2D(tex,vec2(gl_TexCoord[0].s,gl_TexCoord[0].t));
        ct = texel.rgb;
        at = texel.a;
        af = 1.0; 
        gl_FragColor = vec4(cf*ct, af*at);
    }
    //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
    //gl_FragColor = vec4(Precolor,1.0);
}
uniform vec3 rf,ambient;
uniform float roughness;
uniform int lightNum,iftex;
uniform sampler2D tex;
uniform sampler2D env_brdf;
//uniform sampler2D env_map;
uniform samplerCube cubemap;
varying vec3 reflectvec;
//uniform float density;
varying vec3 normal;
varying vec3 ecPos; 

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N){
    float a = Roughness * Roughness;
    float Phi = 2.0 * 3.1415926 * Xi.x;
    float CosTheta = sqrt( (1.0 - Xi.y) / ( 1.0 + (a*a - 1.0) * Xi.y ) );
    float SinTheta = sqrt( 1.0 - CosTheta * CosTheta );
    vec3 H;
    H.x = SinTheta * cos( Phi );
    H.y = SinTheta * sin( Phi );
    H.z = CosTheta;
    vec3 UpVector = abs(N.z) < 0.999 ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
    vec3 TangentX = normalize( cross( UpVector , N ) );
    vec3 TangentY = cross( N, TangentX );
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}

float saturate(float f){
    return min(1.0,max(0.0,f));
}

vec2 Hammersley(int i, int N){
    vec2 Xi;
    Xi.x = float(i)*1.0/float(N);
    Xi.y = 0.0;
    float base = 2.0;
    float next = float(i);
    float inv_base = 1.0/base;
    float f = inv_base;//Base is 2
    while(next>0.0){
        Xi.y += f*mod(next,2.0);
        next = floor(next/base);
        f *= inv_base;
    }
    return Xi;
}

vec3 PrefilterEnvMap(float Roughness , vec3 R){
    vec3 N = R;
    vec3 V = R;
    vec3 PrefilteredColor = vec3(0.0,0.0,0.0);
    int NumSamples = 512;
    float TotalWeight = 0.0;
    for( int i = 0; i < NumSamples; i++ )
    {
        vec2 Xi = Hammersley( i, NumSamples );
        vec3 H = ImportanceSampleGGX( Xi, Roughness , N);
        vec3 L = 2.0 * dot( V, H ) * H - V;
        float NoL = saturate( dot( N, L ) );
        if( NoL > 0.0 )
        {
            PrefilteredColor +=  textureCube(cubemap, L).rgb * NoL;
            TotalWeight += NoL;
        }
    }
    return PrefilteredColor / TotalWeight;
}


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

    //CubeMap
    //vec4 Precolor = textureCube(cubemap, reflectvec);
    vec3 Precolor = PrefilterEnvMap(roughness,reflectvec);

    //Precolor = Precolor / Precolor.w;

    norm = normalize(normal);
	view = normalize(vec3(0.0,0.0,0.0)-ecPos);

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

    //Prefilter Mapping
    //cf = cf*texture2D(env_map, vec2(cosx,cosy*0.5));
    //cf = cf*texture2D(env_map, vec2(acos(-mapv.x)*invpi, atan(mapv.y,mapv.z))*invpi*0.5+0.5);
    //############cf = cf*texture2D(env_map, vec2(mapv.x*0.25+0.25,-mapv.y*0.5+0.5), 2.0/(2.0-roughness));

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
    //gl_FragColor = Precolor;
}
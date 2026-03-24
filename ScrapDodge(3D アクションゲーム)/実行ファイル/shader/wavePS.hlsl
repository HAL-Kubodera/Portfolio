
#include "common.hlsl"


Texture2D		g_Texture : register(t0);
Texture2D       g_EnvTexture : register(t1);
SamplerState	g_SamplerState : register(s0);


static float PI = 3.141592653589;

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //法線ベクトル
    float3 normal = normalize(In.Normal.xyz);
    
    //反射ベクトル
    float3 refv = reflect(eyev.xyz, normal);
    refv = normalize(refv);
    
    //環境マッピングテクスチャ座標
    float2 envTexCoord;
    envTexCoord.x = atan2(refv.x, refv.z) / PI;
    envTexCoord.y = acos(refv.y) / PI;
    
    if (Material.TextureEnable)
    {
        outDiffuse = g_EnvTexture.Sample(g_SamplerState, envTexCoord);
        outDiffuse *= In.Diffuse;
    }
    else
    {
        outDiffuse = In.Diffuse;
    }
}


#include "common.hlsl"


Texture2D		g_Texture : register(t0);
Texture2D       g_TextureSoil : register(t1);

SamplerState	g_SamplerState : register(s0);



void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{

    if (Material.TextureEnable)
    {
        outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord) * (1.0f - In.Diffuse.a);
        outDiffuse += g_TextureSoil.Sample(g_SamplerState, In.TexCoord) * In.Diffuse.a;
        outDiffuse *= In.Diffuse;
    }
    else
    {
        outDiffuse = In.Diffuse;
    }
    
    float3 fogColor = float3(1.0, 1.0, 1.0);
    float fog = In.Depth * 0.01;
    outDiffuse.rgb = outDiffuse.rgb * (1.0 - fog) + fogColor * fog;
    outDiffuse.a = 1.0;
    outDiffuse.a = 1.0f;
}

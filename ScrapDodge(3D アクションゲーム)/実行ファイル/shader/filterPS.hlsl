
#include "common.hlsl"


Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);


void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //ラプラシアン
    float offset = 1.0 / 500;
    //float4 color[3][3];
    
    ////00 10 20
    ////01 11 21
    ////02 12 22
    //color[0][0] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(-offset, -offset));
    //color[1][0] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, -offset));
    //color[2][0] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(offset, -offset));
    
    //color[0][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(-offset, 0.0));
    //color[1][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, 0.0));
    //color[2][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(offset, 0.0));
    
    //color[0][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(-offset, offset));
    //color[1][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, offset));
    //color[2][1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(offset, offset));
    
    //平均化
    //float filter[3][3] =
    //{
    //    {
    //        { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 },
    //        { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 },
    //        { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 },
    //    }
    //};
    
    //ラプラシアン
    //float filter[3][3] =
    //{
    //    {
    //        { 0.0, 1.0, 0.0 },
    //         { 1.0, 4.0, 1.0 },
    //        { 0.0, 1.0, 0.0 },
    //    }
    //};
    
    //ラプラシアン
    //float filter[3][3] =
    //{
    //    {
    //        { 0.0, -1.0, 0.0 },
    //        { -1.0, 5.0, -1.0 },
    //        { 0.0, -1.0, 0.0 },
    //    }
    //};
    
    //ラプラシアン
    //float filter[3][3] =
    //{
    //    {
    //        { 1.0, 0.0, 0.0 },
    //        { 0.0, 0.0, 0.0 },
    //        { 0.0, 0.0, -1.0 },
    //    }
    //};
    
     //ガウシアン
    //float filter[3][3] =
    //{
    //    {
    //        { 1.0, 0.0, 0.0 },
    //        { 0.0, 0.0, 0.0 },
    //        { 0.0, 0.0, -1.0 },
    //    }
    //};
    
    //for (int y = -1; y <= 1; y++)
    //{
    //    for (int x = -1; x <= 1; x++)
    //    {
    //        color[y + 1][x + 1] = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(x, y) * offset) * filter[y + 1][x + 1];
    //    }
    //}
    
    //outDiffuse.a = 1.0;
    
    //ガウシアンフィルター

    float weight = 0.0;
    for (int y = -20; y <= 20; y++)
    {
        for (int x = -20; x <= 20; x++)
        {
            float lenSq = x * x + y * y;
            float gauss = exp(-lenSq * 0.1);
            outDiffuse += g_Texture.Sample(g_SamplerState, In.TexCoord + float2(x, y) * offset) * gauss;
            
            weight += gauss;
        }

    }
    
    outDiffuse /= weight;
    
    outDiffuse.a = 1.0;
    
    //ラプラシアン

    
    float4 colorX0 = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 colorX1 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(offset, 0.0));
    
    float4 colorDX = abs(colorX1 - colorX0);
    
    float4 colorY0 = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 colorY1 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, offset));
    
    float4 colorDY = abs(colorY1 - colorY0);
    
    float4 colorDXY = sqrt(colorDX * colorDX + colorDY * colorDY);
    
    outDiffuse = colorDXY;
    outDiffuse.a = 1.0;
    
    //float4 colorX0 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(-offset, 0.0));
    //float4 colorX1 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(offset, 0.0));
    //float4 colorY0 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, -offset));
    //float4 colorY1 = g_Texture.Sample(g_SamplerState, In.TexCoord + float2(0.0, offset));
    //float4 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    //float4 colorDXY = (colorX0 + colorX1 + colorY0 + colorY1) - 4.0 * color;
    
    //outDiffuse = colorDXY;
    //outDiffuse.a = 1.0;
} 

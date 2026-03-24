
#include "common.hlsl"

StructuredBuffer<float3> Position : register(t2);

void main(in VS_IN In, out PS_IN Out)
{

    Out.Position = mul(In.Position, World);
    Out.Position.xyz += Position[In.Instanced];
	
    Out.Position = mul(Out.Position, View);
    Out.Position = mul(Out.Position, Projection);

	Out.TexCoord = In.TexCoord;
	Out.Diffuse = In.Diffuse * Material.Diffuse;
    Out.Depth = Out.Position.z;
}


// Reoriented Normal Mapping
// http://blog.selfshadow.com/publications/blending-in-detail/
// Altered to take normals (-1 to 1 ranges) rather than unsigned normal maps (0 to 1 ranges)
vec3 BlendRNM(vec3 n1, vec3 n2)
{
	n1.z += 1;
	n2.xy = -n2.xy;

	return n1 * dot(n1, n2) / n1.z - n2;
}

// Sample normal map with triplanar coordinates
// Returned normal will be in obj/world space (depending whether pos/normal are given in obj or world space)
// Based on: medium.com/@bgolus/normal-mapping-for-a-triplanar-shader-10bf39dca05a
vec3 TriplanarNormal(vec3 vertPos, vec3 normal, vec3 scale, vec2 offset, sampler2D normalMap)
{
	vec3 absNormal = abs(normal);

	// Calculate triplanar blend
	vec3 blendWeight = clamp(pow(normal, vec3(2.0)), vec3(0.0), vec3(1.0));
	// Divide blend weight by the sum of its components. This will make x + y + z = 1
	blendWeight /= dot(blendWeight, vec3(1.0));

	// Calculate triplanar coordinates
	vec2 uvX = vertPos.zy * scale.xy + offset;
	vec2 uvY = vertPos.xz * scale.xy + offset;
	vec2 uvZ = vertPos.xy * scale.xy + offset;

	// Sample tangent space normal maps
	// UnpackNormal puts values in range [-1, 1] (and accounts for DXT5nm compression)
	vec3 tangentNormalX = texture(normalMap, uvX).xyz * 2.0 - 1.0;
	vec3 tangentNormalY = texture(normalMap, uvY).xyz * 2.0 - 1.0;
	vec3 tangentNormalZ = texture(normalMap, uvZ).xyz * 2.0 - 1.0;

	// Swizzle normals to match tangent space and apply reoriented normal mapping blend
	tangentNormalX = BlendRNM(vec3(normal.zy, absNormal.x), tangentNormalX);
	tangentNormalY = BlendRNM(vec3(normal.xz, absNormal.y), tangentNormalY);
	tangentNormalZ = BlendRNM(vec3(normal.xy, absNormal.z), tangentNormalZ);

	// Apply input normal sign to tangent space Z
	vec3 axisSign = sign(normal);
	tangentNormalX.z *= axisSign.x;
	tangentNormalY.z *= axisSign.y;
	tangentNormalZ.z *= axisSign.z;

	// Swizzle tangent normals to match input normal and blend together
	vec3 outputNormal = normalize(
		tangentNormalX.zyx * blendWeight.x +
		tangentNormalY.xzy * blendWeight.y +
		tangentNormalZ.xyz * blendWeight.z
	);

	return outputNormal;
}
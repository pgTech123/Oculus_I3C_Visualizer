__kernel void clearImage(__write_only image2d_t resultTexture)
{
    //This function fills the alpha channel with 0's
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 pixelValue = (float4)(1.0f, 1.0f, 1.0f, 1.0f);
    write_imagef(resultTexture, coord, pixelValue);
}

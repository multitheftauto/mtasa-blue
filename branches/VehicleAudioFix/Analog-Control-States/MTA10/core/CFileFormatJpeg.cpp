/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"         /* get library error codes too */
#include "cderror.h"        /* get application-specific error codes */


///////////////////////////////////////////////////////////////
//
// IsJpeg
//
// Check if probably jpeg
//
///////////////////////////////////////////////////////////////
bool IsJpeg ( const void* pData, uint uiDataSize )
{
    static uchar JpegHeader[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46 };
    static uchar JpegTail[] = { 0xFF, 0xD9 };

    if ( uiDataSize >= sizeof ( JpegHeader ) )
    {
        if ( memcmp ( pData, JpegHeader, sizeof ( JpegHeader ) ) == 0 &&
             memcmp ( (BYTE*)pData + uiDataSize - sizeof ( JpegTail ), JpegTail, sizeof ( JpegTail ) ) == 0 )
        {
            return true;
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// JpegDecode
//
// jpeg to XRGB
//
///////////////////////////////////////////////////////////////
bool JpegDecode ( const void* pData, uint uiDataSize, CBuffer& outBuffer, uint& uiOutWidth, uint& uiOutHeight )
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* Specify data source for decompression */
    jpeg_mem_src (&cinfo,(uchar*)pData,uiDataSize );

    /* Read file header, set default decompression parameters */
    (void) jpeg_read_header(&cinfo, TRUE);

    /* default decompression parameters */
    // TODO

    /* Start decompressor */
    (void) jpeg_start_decompress(&cinfo);

    /* Write output file header */
    JDIMENSION uiWidth = cinfo.output_width;	/* scaled image width */
    JDIMENSION uiHeight = cinfo.output_height;	/* scaled image height */

    uiOutWidth = uiWidth;
    uiOutHeight = uiHeight;

    outBuffer.SetSize ( uiWidth * uiHeight * 4 );
    char* pOutData = outBuffer.GetData ();

    /* Process data */
    JSAMPROW row_pointer[1];
    CBuffer rowBuffer;
    rowBuffer.SetSize ( uiWidth * 3 );
    char* pRowTemp = rowBuffer.GetData ();

    while (cinfo.output_scanline < cinfo.output_height)
    {
        BYTE* pRowDest = (BYTE*)pOutData + cinfo.output_scanline * uiWidth * 4;
        row_pointer[0] = (JSAMPROW)pRowTemp;

        JDIMENSION num_scanlines = jpeg_read_scanlines(&cinfo, row_pointer, 1);

        for ( uint i = 0 ; i < uiWidth ; i++ )
        {
            pRowDest[i*4+0] = pRowTemp[i*3+2];
            pRowDest[i*4+1] = pRowTemp[i*3+1];
            pRowDest[i*4+2] = pRowTemp[i*3+0];
            pRowDest[i*4+3] = 255;
        }
    }

    // Finish decompression and release memory.
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    if ( jerr.num_warnings )
        return false;
    return true;
}


///////////////////////////////////////////////////////////////
//
// JpegEncode
//
// XRGB to jpeg
//
///////////////////////////////////////////////////////////////
bool JpegEncode ( uint uiWidth, uint uiHeight, uint uiQuality, const void* pData, uint uiDataSize, CBuffer& outBuffer )
{
    // Validate
    if ( uiDataSize == 0 || uiDataSize != uiWidth * uiHeight * 4 )
        return false;

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned long memlen = 0;
    unsigned char *membuffer = NULL;
    jpeg_mem_dest (&cinfo,&membuffer,&memlen );

    cinfo.image_width = uiWidth;    /* image width and height, in pixels */
    cinfo.image_height = uiHeight;
    cinfo.input_components = 3;     /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, uiQuality, true);
    cinfo.dct_method = JDCT_IFAST;

    /* Start compressor */
    jpeg_start_compress(&cinfo, TRUE);

    /* Process data */
    CBuffer rowBuffer;
    rowBuffer.SetSize ( uiWidth * 3 );
    char* pRowTemp = rowBuffer.GetData ();

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height)
    {
        BYTE* pRowSrc = (BYTE*)pData + cinfo.next_scanline * uiWidth * 4;
        for ( uint i = 0 ; i < uiWidth ; i++ )
        {
            pRowTemp[i*3+0] = pRowSrc[i*4+2];
            pRowTemp[i*3+1] = pRowSrc[i*4+1];
            pRowTemp[i*3+2] = pRowSrc[i*4+0];
        }
        row_pointer[0] = (JSAMPROW)pRowTemp;
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    /* Finish compression and release memory */
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    // Copy out data and free memory
    outBuffer = CBuffer ( membuffer, memlen );
    free ( membuffer );

    return true;
}


///////////////////////////////////////////////////////////////
//
// JpegGetDimensions
//
// This needs optimizing
//
///////////////////////////////////////////////////////////////
bool JpegGetDimensions ( const void* pData, uint uiDataSize, uint& uiOutWidth, uint& uiOutHeight )
{
    CBuffer temp;
    return JpegDecode ( pData, uiDataSize, temp, uiOutWidth, uiOutHeight );
}

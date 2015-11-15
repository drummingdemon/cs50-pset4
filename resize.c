/*
 *  resize.c
 *
 *  Harvard CS50x3 - Problem Set 4
 *
 *  Resizes a 24-bit BMP bit-by-bit.
 *
 *  Gábor Hargitai <gabriel.hargitai@gmail.com>
 *
 */
       
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize scale_number infile outfile\n");
        return 1;
    }

    // get the scale factor
    int n = atoi(argv[1]);

    if (n > 100 || n < 1)
    {
        printf("The scale factor must be between 1 and 100!\n");
        return 5;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // construct the output headers
    BITMAPFILEHEADER obf;
    BITMAPINFOHEADER obi;
    
    // fill it up with initial data
    obf = bf;
    obi = bi;
    
    // scale width and height by n
    obi.biWidth = obi.biWidth * n;
    obi.biHeight = obi.biHeight * n;
    
    // determine padding for scanlines
    int inPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // determine outfile's padding for scanline
    int outPadding = (4 - (obi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // biWidth returns pixels and we need byte, so we multiply it with
    // whatever bytesize RGBTRIPLE holds and add padding to it
    obi.biSizeImage = ((obi.biWidth * sizeof(RGBTRIPLE) + outPadding) * abs(obi.biHeight));
    
    // file size is the pixel data plus the size of the headers
    obf.bfSize = obi.biSizeImage + sizeof(obf) + sizeof(obi);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&obf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&obi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int s = 0; s < n; s++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile factor time
                for (int k = 0; k < n; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over input image padding, if any
            fseek(inptr, inPadding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < outPadding; k++)
            {
                fputc(0x00, outptr);
            }
               
            // seek back
            fseek(inptr, -(bi.biWidth * 3 + inPadding ), SEEK_CUR);
        }
        
        // seek to the next line
        fseek(inptr, bi.biWidth * 3 + inPadding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

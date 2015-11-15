/*
 *  recover.c
 *
 *  Harvard CS50x3 - Problem Set 4
 *
 *  Recovers JPEGs from a forensic image.
 *
 *  Gábor Hargitai <gabriel.hargitai@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // definition of a byte for easier reference
    typedef uint8_t BYTE;

    // initialization of a buffer
    BYTE buffer[512];

    // input and output filenames
    char *infile = "card.raw";
    char outfile[7];
    
    // open the input file for reading
    FILE *file = fopen(infile, "r");
    FILE *outHandle = NULL;
       
    // error message and exit open opening failure
    if (file == NULL)
    {
        fclose(file);
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    // initialize a counter for the output file numbering
    int count = 0;
    
    // we read the input stream until we hit an end-of-file
    while (fread(&buffer, sizeof(BYTE), 512, file))
    {
        // check if we caught a known JPEG header
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            // first header in line
            if (count == 0)
            {
                sprintf(outfile, "%03d.jpg", count);
                outHandle = fopen(outfile, "w");
                fwrite(buffer, sizeof(BYTE), 512, outHandle);
                count++;
            }
            // next header in line
            else
            {
                fclose(outHandle);
                sprintf(outfile, "%03d.jpg", count);
                outHandle = fopen(outfile, "w");
                fwrite(buffer, sizeof(BYTE), 512, outHandle);
                count++;            
            }
        }
        else
        {
            // we'll only start writing data after the beginning
            // of the raw dump (as it does not contain picture data)
            if (count != 0)
            {
                fwrite(buffer, sizeof(BYTE), 512, outHandle);
            }
        }
    }
    
    //close the files
    fclose(outHandle);
    fclose(file);
   
    return 0;
}

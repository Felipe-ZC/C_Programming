/*********************************************************************
Purpose/Description:

A program to perfrom transformations on a bitmap image. The following
transformations are supported:

- Rotations (in multiples of 90)
- Image scaling (positive integers only)
- Flipping

Author's name: Felipe Zuniga
Author’s Panther ID: 5159067
Certification:
I hereby certify that this work is my own and none of it is the work of
any other person.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "bmplib.h"
#include <stdio.h>
#include <fcntl.h>

/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */
int enlarge(PIXEL* original, int rows, int cols, int scale, 
	    PIXEL** new, int* newrows, int* newcols) 
{ 
  // If the scale factor is not a positive integer...
  if(scale < 0)
    // ERROR: invalid scale factor!
    return -1;

  *newrows = rows * scale;
  *newcols = cols * scale;
  *new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));

  // Iterate through newrows
  int nrows;
  for (nrows = 0; nrows < (*newrows); ++nrows)
  {
    // Get new scaled row
    PIXEL* n = (*new) + (nrows * (*newcols));

    /*
    Copy a group of pixels from a row in the 
    original image to those of the new image. 
    */
    int i, j;
    for (i = 0; i < cols; i++)
    {
      for (j = 0; j < scale; j++)
      {     
        // Get unscaled row
        int r = nrows / scale;
        PIXEL* o = original + (r*cols) + i;
        *n++ = *o;
      }
    }
  }

  return 0;
}

/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read 
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 * 
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation, 
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows
 * newcols  - the new number of cols
 */
int rotate(PIXEL* original, int rows, int cols, int rotation,
	   PIXEL** new, int* newrows, int* newcols)
{
  // If the degree is not a multiple of 90...
  if((rotation % 90) != 0)
    // ERROR: Invalid degree!
    return -1;

  // If the degree is greater than one full rotation...
  while(rotation > 360 || rotation < (-360))
  {
  	// If the rotation is negative...
  	if(rotation < 0)
  		// Keep the rotation negative 
  		rotation %= -360;
  	else
  		rotation %= 360;
  }

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  if(rotation == 90 || rotation == -270 )
  {
    int row, col;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < cols; col++ )
        {
            PIXEL* o = original+ (row*cols) + col;
            PIXEL* n = (*new)+((cols-col-1)*rows) + row;
            *n = *o;
        }
    }
  *newcols = rows;
  *newrows = cols;
 }
 else if(rotation == -90 || rotation == 270)
 {
    int row, col;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < cols; col++ )
        {
            PIXEL* o = original+ (row*cols) + col;
            PIXEL* n = (*new)+(col*rows) + (rows-row-1);
            *n = *o;
        }
    }
  *newcols = rows;
  *newrows = cols;
 }
 else if(rotation == 180 || rotation == -180)
 {
    int row, col;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < cols; col++ )
        {
            PIXEL* o = original+ (row*cols) + col;
            PIXEL* n = (*new)+((rows-row-1)*cols) + (cols-col-1);
            *n = *o;
        }
    }
  *newcols = cols;
  *newrows = rows;
 }
 else if(rotation == 360 || rotation == -360 || rotation == 0)
 {	
 	int row,col;
    for(row = 0; row < rows; row++)
    {
        for(col = 0; col < cols; col++ )
        {
            PIXEL* o = original + (row*cols) + col;
            PIXEL* n = (*new) + (row*cols) + col;
            *n = *o;
        }
    }
 	*newcols = cols;
 	*newrows = rows;
 }
   return 0;
}

/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile(). 
 * 
 * THIS IS GIVEN TO YOU SOLELY TO LOOK AT AS AN EXAMPLE
 * TRY TO UNDERSTAND HOW IT WORKS
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols) 
{
  int row, col;

  if ((rows <= 0) || (cols <= 0)) return -1;

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  for (row=0; row < rows; row++)
    for (col=0; col < cols; col++) {
      PIXEL* o = original + row*cols + col;
      PIXEL* n = (*new) + row*cols + (cols-1-col);
      *n = *o;
    }

  return 0;
}

int main(int argc, char **argv)
{	
	extern char *optarg;
	extern int optind;
	int ch, err = 0;
	// Flags
	int rflag = 0, sflag = 0, fflag = 0, oflag = 0;
	// Filenames and other variables.
	int scaleFactor = 0 ; // Scale factor, must be a positive integer
	int degree = 0; // Rotate an image by a degree, must be a multiple of 90 (clockwise is positive)
	char outputfileName[4096] = ""; 
	static char usage[] = "usage: %s bmptool [-s scale | -r degree | -f ] [-o output_file] [input_file] \n";

	while ((ch = getopt(argc, argv, "s:r:fo:i")) != -1){
		switch (ch) {
		case 's':
			sflag = 1;
			scaleFactor = atoi(optarg);
			break;
		case 'r':
			rflag = 1;
			degree = atoi(optarg);
			break;
		case 'f':
			fflag = 1;
			break;
		case 'o':
			oflag = 1;
			strcpy(outputfileName,optarg);
			break;
		default:
			err = 1;
			break;
		}
	}

	// Check for valid input 
	if (err == 1) {	
		printf("optind = %d, argc=%d\n", optind, argc);
		fprintf(stderr, "%s: Invalid input!\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	// Check for input file...
	char* fNamePtr = argv[argc - 1];
	int r, c;
	int fp = 0;
	PIXEL* b;
	// If the file was not read succesfully....
	if((fp = open(fNamePtr,O_RDONLY)) < 0)
		// Input comes from stdin
		readFile(NULL,&r,&c,&b);
	else
		// Input comes from input file
		readFile(fNamePtr,&r,&c,&b);

	close(fp);

	// PERFROM IMAGE OPERATIONS

	// Holds transformed image
	PIXEL* nb;
	// Input comes from the inputfile
	// Scale image
	if(sflag == 1)
	{
		// Holds new rows and columns
		int nr, nc;
		// Perfrom transformation
		int success = enlarge(b,r,c,scaleFactor,&nb,&nr,&nc);

		// Check for errors...
		if(success != 0)
		{
			printf("Error! Unable to enlarge image!\n");
			return -1;
		}

		// If an output filename was provided...
		if(oflag == 1)
		{
			// Write to outputfile
			writeFile(outputfileName, nr, nc, nb);
		}
		else
			// Write to stdout
			writeFile(NULL, nr, nc, nb);
	}
	// Rotate image
	else if(rflag == 1)
	{
		// Holds new rows and columns
		int nr, nc;
		// Perfrom transformation
		int success = rotate(b,r,c,degree,&nb,&nr,&nc);

		// Check for errors...
		if(success != 0)
		{
			printf("Error! Unable to rotate image!\n");
			return -1;
		}

		// If an output filename was provided...
		if(oflag == 1)
		{
			// Write to outputfile
			writeFile(outputfileName, nr, nc, nb);
		}
		else
			// Write to stdout
			writeFile(NULL, nr, nc, nb);
	}
	// Flip image
	else if(fflag == 1)
	{	
		// Perfrom transformation
		int success = flip(b, &nb, r, c);

		// Check for errors...
		if(success != 0)
		{
			printf("Error! Unable to flip image!");
			return -1;
		}

		// If an output filename was provided...
		if(oflag == 1)
		{
			// Write to outputfile
			writeFile(outputfileName, r, c, nb);
		}
		else
			// Write to stdout
			writeFile(NULL, r, c, nb);
	}

	free(b);
	free(nb);
	return 0;
}

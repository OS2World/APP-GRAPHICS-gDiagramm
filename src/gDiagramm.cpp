/* gDiagramm.cpp */
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
 #include <time.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#if defined(__WATCOMC__)
#define _filelength filelength
#endif

#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"

#include "gDiagramm.hpp"

#define POKA 0

int ReadData(char *fname);
int ReadProject(char fname[]);
#define APPNAME   "gDiagramm"
#define VERS      "0.0.7"
#define SubVERS   ""

char *Head[]=
{ APPNAME " " VERS SubVERS " " __DATE__,
  "(c) Evgeny Kotsuba",
  NULL
};

class Diagramm diagramm;

int main (int n, char *par[])
{ int rc, t0;


  if( (n != 3) && (n != 5) )
  {  printf("%s\n%s\n\n",Head[0],Head[1]);
     printf("Usage: gDiagramm dataFile formatFile [startLine endLine | -oOffset NumOflines]\n");
     exit(1);
  }
  t0 = clock();
  rc = diagramm.Read(par[2]);
  if(rc)
  {    if(rc == 1) printf("Error reading form file %s",par[2]);
       else        printf("Bad form file %s",par[2]);
       exit(1);
  }
  if(n == 5)
  { char *pstr = par[3];
    if(!strnicmp(pstr,"-o",2))
    {  diagramm.startLineSeek = atoi(pstr+2);
       diagramm.startLine = 0;
       diagramm.endLine = atoi(par[4]);
    } else {
       diagramm.startLine = atoi(par[3]);
       diagramm.endLine = atoi(par[4]);
    }
  }
//  rc = ReadProject(par[2]);
  rc = ReadData(par[1]);
  if(rc)
  {    printf("Error reading datafile %s",par[1]);
       exit(1);
  }
  diagramm.PlotData();
  printf("Done in %.2f sec\n",(clock()-t0)*0.001);

  return 0;

#if POKA
/* Create output image, 256 by 256 pixels, true color. */
  im_out = gdImageCreateTrueColor (256, 256);
  /* First color allocated is background. */
  white = gdImageColorAllocate (im_out, 255, 255, 255);
  red = gdImageColorAllocate(im_out, 255, 0, 0);
       /* Set transparent color. */
  gdImageColorTransparent (im_out, white);

  gdImageFilledEllipse(im_out, 50, 25, 98, 48, red );

  gdImageFilledEllipse(im_out, 100, 100, 50, 50, white );

  //# if POKA
  /* Try to load demoin.png and paste part of it into the
     output image. */
  in = fopen ("demoin.png", "rb");
  if (!in)
    {
      fprintf (stderr, "Can't load source image; this demo\n");
      fprintf (stderr, "is much more impressive if demoin.png\n");
      fprintf (stderr, "is available.\n");
      im_in = 0;
    }
  else
    {
      im_in = gdImageCreateFromPng (in);
      fclose (in);
      /* Now copy, and magnify as we do so */
      gdImageCopyResized (im_out, im_in,
                         32, 32, 0, 0, 192, 192, 255, 255);
    }
  red = gdImageColorAllocate (im_out, 255, 0, 0);
  green = gdImageColorAllocate (im_out, 0, 255, 0);
  blue = gdImageColorAllocate (im_out, 0, 0, 255);
  /* Rectangle */
  gdImageLine (im_out, 16, 16, 240, 16, green);
  gdImageLine (im_out, 240, 16, 240, 240, green);
  gdImageLine (im_out, 240, 240, 16, 240, green);
  gdImageLine (im_out, 16, 240, 16, 16, green);
  /* Circle */
  gdImageArc (im_out, 128, 128, 60, 20, 0, 720, blue);
  /* Arc */
  gdImageArc (im_out, 128, 128, 40, 40, 90, 270, blue);
  /* Flood fill: doesn't do much on a continuously
     variable tone jpeg original. */
  gdImageFill (im_out, 8, 8, blue);
  /* Polygon */
  points[0].x = 64;
  points[0].y = 0;
  points[1].x = 0;
  points[1].y = 128;
  points[2].x = 128;
  points[2].y = 128;
  gdImageFilledPolygon (im_out, points, 3, green);
  /* Brush. A fairly wild example also involving a line style! */
  if (im_in)
    {
      int style[8];
      brush = gdImageCreateTrueColor (16, 16);
      gdImageCopyResized (brush, im_in,
                         0, 0, 0, 0,
                         gdImageSX (brush), gdImageSY (brush),
                         gdImageSX (im_in), gdImageSY (im_in));
      gdImageSetBrush (im_out, brush);
      /* With a style, so they won't overprint each other.
         Normally, they would, yielding a fat-brush effect. */
      style[0] = 0;
      style[1] = 0;
      style[2] = 0;
      style[3] = 0;
      style[4] = 0;
      style[5] = 0;
      style[6] = 0;
      style[7] = 1;
      gdImageSetStyle (im_out, style, 8);
      /* Draw the styled, brushed line */
      gdImageLine (im_out, 0, 255, 255, 0, gdStyledBrushed);
    }
  /* Text */
  gdImageString (im_out, gdFontGiant, 32, 32,
                (unsigned char *) "hi", red);
  gdImageStringUp (im_out, gdFontSmall, 64, 64,
                  (unsigned char *) "hi", red);
  /* Make output image interlaced (progressive, in the case of JPEG) */
  gdImageInterlace (im_out, 1);
  out = fopen ("demoout.png", "wb");
  /* Write PNG */
//  gdImagePng (im_out, out);
  gdImagePngEx(im_out, fp,9);
  fclose (out);
  gdImageDestroy (im_out);
  if (im_in)
    {
      gdImageDestroy (im_in);
    }
//# e ndif //POKA

  return 0;
#endif //POKA

}

int Diagramm::PlotData(void)
{
  /* Input and output files */
  FILE *in;
  FILE *out;
  /* Input and output images */
  gdImagePtr im_in = 0, im_out = 0;
  /* Brush image */
  gdImagePtr brush;
  /* Color indexes */
  int white;
  int blue;
  int red, bred;
  int green;
  int Backgr,col, col1, gridColor2;
  int colAxes, color[64],i,j,ix,iy,ix1,iy1,ixold,ixoldH,iyold,n;
  /* gd fonts for font test */
  gdFontPtr Fonts[5];
  /* Points for polygon */
  gdPoint points[3];
  char str[80];
  double x,y, xold, x1,y1;
  int shiftX=16 ,shiftY=16;
  int styleDotted[2], styleDashed[6];

   CalcMinMax();

   if(isDataTimeX)
   { time_t t0, t1;
     struct tm *timeptr;
     char str2[128];
     t0 =  pfields[nfieldUsedAsX].time_min + MinX;
     t1 =  pfields[nfieldUsedAsX].time_min + MaxX;
     timeptr = localtime(&t0);
      strftime(str, sizeof(str),"%c", timeptr);
     timeptr = localtime(&t1);
      strftime(str2, sizeof(str2),"%c", timeptr);
       printf("X: %s -> %s,\n",str,str2);
       printf("MinY=%f, MaxY=%f\n", MinY,MaxY);
   } else {
       printf("MinX=%f, MaxX=%f,\t",MinX,MaxX);
       printf("MinY=%f, MaxY=%f\n", MinY,MaxY);
   }

  /* Text (non-truetype; see gdtestft for a freetype demo) */
  Fonts[0] = gdFontGetTiny ();
  Fonts[1] = gdFontGetSmall ();
  Fonts[2] = gdFontGetMediumBold ();
  Fonts[3] = gdFontGetLarge ();
  Fonts[4] = gdFontGetGiant ();
  if(TitleFont < 0 || TitleFont > 4) TitleFont = 2;
  if(AxesNumFont < 0 || AxesNumFont > 4) AxesNumFont = 1;
  shiftX = 16 + (AxesNumFont-1) * 2;
  shiftY = 16 + (AxesNumFont-1) * 2;
  if(isDataTimeX > 1) shiftY += 16;
/* Create output image, dx by dy pixels, true color. */
  im_out = gdImageCreateTrueColor (DimX, DimY);
  /* First color allocated is background. */
  white = gdImageColorAllocate (im_out, 255, 255, 255);
  red = gdImageColorAllocate(im_out, 255, 0, 0);
  bred =gdImageColorAllocate(im_out, 160, 0, 0);

  Backgr = gdImageColorAllocate(im_out, 0xb0, 0xb0, 0xb0);
  if(BackColor != -1)
  Backgr = gdImageColorAllocate(im_out, BackColor>>16, (BackColor>>8)&0xff, BackColor&0xff);

/* fone */
  gdImageFilledRectangle(im_out, 0, 0, DimX-1,DimY-1, Backgr);
  gdImageRectangle(im_out, 0, 0, DimX-1,DimY-1, white);

/* Axes */
   colAxes = gdImageColorAllocate(im_out, 0x30, 0x30, 0x30);
   if(AxesColor == -1) AxesColor = colAxes;
   gdImageLine(im_out, shiftX- shiftX/4, shiftY, DimX-shiftX, shiftY,     AxesColor);
   gdImageLine(im_out, shiftX,(shiftY- shiftY/4), shiftX , DimY - shiftY, AxesColor);


//  gdImageStringUp (im_out, gdFontSmall, 64, 64,
//                  (unsigned char *) "hi", red);

/*
   sprintf(str,"%.f", MinX);
   gdImageString(im_out, Fonts[0], shiftX- shiftX/4, (shiftY- shiftY/8),(unsigned char*) str, bred);
   sprintf(str,"%.f", MaxX);
   gdImageString(im_out, Fonts[0],  DimX-shiftX-16, (shiftY- shiftY/8),(unsigned char*) str, bred);

   sprintf(str,"%.2f", MinY);
   gdImageStringUp(im_out, Fonts[0], 4, (shiftY+16),(unsigned char*) str, bred);
   sprintf(str,"%.2f", MaxY);
   gdImageDrawTextUp(im_out, Fonts[0], 4, (DimY - shiftY),(unsigned char*) str, bred,gdDT_LEFT|gdDT_VCENTER);
*/

/* Title*/
   gdImageDrawText(im_out, Fonts[TitleFont], DimX/2, (DimY - shiftY/4),(unsigned char*) Title, TitleColor,gdDT_CENTER|gdDT_TOP);

   mashtX = 1.;
   if(MaxX > MinX)
         mashtX = (DimX-shiftX*2) / double(MaxX - MinX);
   mashtY = 1.;
   if(MaxY > MinY)
          mashtY = (DimY-shiftY*2) / double(MaxY - MinY);

   n = (int)((MaxX-MinX)/GridXStep);
   col = gdImageColorAllocate(im_out,  GridColor>>16, (GridColor>>8)&0xff,  GridColor&0xff);
   gridColor2 = gdImageColorAllocate(im_out,  ((GridColor>>16)+(BackColor>>16))/2,
                                              ( ((GridColor>>8)&0xff) +((BackColor>>8)&0xff)  )/2,
                                               ((GridColor&0xff) + (BackColor&0xff))/2. );
   col1 = gdImageColorAllocate(im_out,  AxesColor>>16, (AxesColor>>8)&0xff,  AxesColor&0xff);
   ixold = ixoldH = 0;

   for(i=0;i<=n+10;i++)
   {  x = GridXStart + i*GridXStep;
      if(x < MinX) continue;
      if(x > MaxX) continue;
       x = (x - MinX)* mashtX + shiftX;
      y = shiftY;
      ix = int(x);
      iy = int(y);
      iy1 = iy - 2;
      if(!(i%5)) iy1 -= 2;
      if(!(i%10)) iy1 -= 2;
      gdImageLine(im_out, ix,iy,ix,iy1, col1);
      if(!(i%10))
      {
         if(isDataTimeX)
         { time_t t0;
           struct tm *timeptr;
           t0 =  pfields[nfieldUsedAsX].time_min +GridXStart + i*GridXStep;
           timeptr = localtime(&t0);
           str[0] = 0;
        //   if(!(i%20))
           if(isDataTimeX == 2) //hh:mm:ss + days
           {
              if(showDataTimeXseconds)
              {  j = strftime(str, sizeof(str),"%H:%M:%S", timeptr);
//               if(!(i%20))
                 if( x > ixoldH)
                 {  gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
                    ixoldH = ix + (j+1) * Fonts[AxesNumFont]->w;
                 }
//              if(!(i%40))
                if( x > ixold)
                { j = strftime(str, sizeof(str),"%x", timeptr);
                  gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1-14,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
                  ixold = ix + (j+1+4) * Fonts[AxesNumFont]->w;
                }
              }  else {
                 j = strftime(str, sizeof(str),"%H:%M", timeptr);
                 if( x > ixoldH)
                 {  gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
                    ixoldH = ix + (j+1) * Fonts[AxesNumFont]->w;
                 }
//                if(!(i%20))
                if( x > ixold)
                { j = strftime(str, sizeof(str),"%x", timeptr);
                   gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1-14,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
                   ixold = ix + (j+1+4) * Fonts[AxesNumFont]->w;
                }
              }
           } else
               if(isDataTimeX == 1) {  // days
                 j= strftime(str, sizeof(str),"%x", timeptr);
                 if( x > ixold)
                 {  gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
                    ixold = ix + (j+2) * Fonts[AxesNumFont]->w;
                 }
           }

         } else {
             sprintf(str,FormatX,(GridXStart + i*GridXStep));
             gdImageDrawText(im_out, Fonts[AxesNumFont], ix, iy1+1,(unsigned char*) str, AxesNumColor,gdDT_CENTER|gdDT_TOP);
         }

/* Set up dotted style. Leave every other pixel alone. */
        styleDotted[0] = col;
        styleDotted[1] = gdTransparent;
     /* Set up dashed style. Three on, three off. */
/*     styleDashed[0] = red;
     styleDashed[1] = red;
     styleDashed[2] = red;
     styleDashed[3] = gdTransparent;
     styleDashed[4] = gdTransparent;
     styleDashed[5] = gdTransparent;
*/
     /* Set dotted style. Note that we have to specify how many pixels are  in the style! */
         gdImageSetStyle(im_out, styleDotted, 2);
     /* Draw a line from the upper left corner to the lower right corner. */
         gdImageLine(im_out, ix, iy, ix, DimY-shiftY, gdStyled);

//
     /* Now the dashed line. */
//     gdImageSetStyle(im, styleDashed, 6);
//     gdImageLine(im, ix, 99, 0, 99, gdStyled);

      }
      if(!(i%5) && (i%10))
      {
          styleDashed[0] = gridColor2 ;
          styleDashed[1] = gdTransparent;
          styleDashed[2] = gdTransparent;
          styleDashed[3] = gdTransparent;
/*
     styleDashed[4] = gdTransparent;
     styleDashed[5] = gdTransparent;
*/
           gdImageSetStyle(im_out, styleDashed, 4);
           gdImageLine(im_out, ix, iy, ix, DimY-shiftY, gdStyled);
      }

   }

   n = (int)((MaxY-MinY)/GridYStep);
   for(i=0;i<=n+10;i++)
   {  y1 = GridYStart + i*GridYStep;
      if(y1 < MinY) continue;
      if(y1 > MaxY) continue;
       y = ( y1 - MinY)* mashtY + shiftY;
      x = shiftX;
      ix = int(x);
      iy = int(y);
      ix1 = ix - 2;
      if(!(i%5)) ix1 -= 2;
      if(!(i%10)) ix1 -= 2;
      gdImageLine(im_out, ix,iy,ix1,iy, col);
      if(!(i%10))
      {  if(fabs(y1) < 0.00000001 && fabs(MaxY-MinY) > 1.) y1 = 0.; //защита от почти нуля и формата %g
         sprintf(str,FormatY,y1);
         gdImageDrawTextUp(im_out, Fonts[AxesNumFont], ix1+1, iy,(unsigned char*) str, AxesNumColor,gdDT_RIGHT|gdDT_VCENTER);
        styleDotted[0] = col;
        styleDotted[1] = gdTransparent;
         gdImageSetStyle(im_out, styleDotted, 2);
     /* Draw a line from the upper left corner to the lower right corner. */
         gdImageLine(im_out, ix, iy, DimX-shiftX, iy, gdStyled);

      }
      if(!(i%5) && (i%10))
      {
          styleDashed[0] = gridColor2 ;
          styleDashed[1] = gdTransparent;
          styleDashed[2] = gdTransparent;
          styleDashed[3] = gdTransparent;
          gdImageSetStyle(im_out, styleDashed, 4);
          gdImageLine(im_out, ix, iy, DimX-shiftX, iy, gdStyled);
      }

   }


  /* Write lines */
/* Выводим данные как графики */
   for(i=0;i<nfields;i++)
   { if(!pfields[i].use) continue;
      if(i == nfieldUsedAsX) continue;
      if(pfields[i].vectorType) continue;
      color[i] = gdImageColorAllocate(im_out, rand()%256, rand()%256, rand()%256);
      if(pfields[i].color != -1)
           color[i] = gdImageColorAllocate(im_out, pfields[i].color>>16, (pfields[i].color>>8)&0xff, pfields[i].color&0xff);

     for(j=0;j<NumRec;j++)
     {
         if(nfieldUsedAsX >=0)
         {     x = mashtX * (GetValue(j,nfieldUsedAsX) - MinX);
         } else {
               x = j * mashtX;
         }
         y = mashtY * (GetValue(j,i) - MinY);
         ix = int(x);
         iy = int(y);
         if(j)
         {  if(ix != ixold || iy != iyold)
            {    if( (nfieldUsedAsX < 0) && (ix < ixold) ) ;
                 else
                     gdImageLine(im_out, ixold+shiftX,(iyold+shiftY),ix+shiftX,(iy+shiftY),  color[i]);
            }
         }
         ixold = ix;
         iyold = iy;
     }
   }
/******************************************/
/* Выводим данные как 2D графики */
   for(i=0;i<nfields;i++)
   {  int ix0F, iy0F, ix1F, iy1F, is;
      if(!pfields[i].use) continue;
      if(!pfields[i].vectorType) continue;
      if( ((pfields[i].vectorType-1)&0x03) != 0) continue;

      color[i] = gdImageColorAllocate(im_out, rand()%256, rand()%256, rand()%256);
      if(pfields[i].color != -1)
           color[i] = gdImageColorAllocate(im_out, pfields[i].color>>16, (pfields[i].color>>8)&0xff, pfields[i].color&0xff);
      ix0F = i;
 /* найдем дргие поля, которые соотв. 2D вектору */
      is = 0;
      for(j=0;j<nfields;j++)
      {  if(pfields[j].vectorType == pfields[i].vectorType+1)
         { is++;  iy0F = j;
           break;
         }
      }
      for(j=0;j<nfields;j++)
      {  if(pfields[j].vectorType == pfields[i].vectorType+2)
         { is++;  ix1F = j;
           break;
         }
      }
      for(j=0;j<nfields;j++)
      {  if(pfields[j].vectorType == pfields[i].vectorType+3)
         { is++;  iy1F = j;
           break;
         }
      }
      if(is != 3)
            break;
       color[iy0F] = gdImageColorAllocate(im_out, pfields[iy0F].color>>16, (pfields[iy0F].color>>8)&0xff, pfields[iy0F].color&0xff);
       color[ix1F] = gdImageColorAllocate(im_out, pfields[ix1F].color>>16, (pfields[ix1F].color>>8)&0xff, pfields[ix1F].color&0xff);
     for(j=0;j<NumRec;j++)
     {
          x  = mashtX * (GetValue(j,ix0F) - MinX);
          y  = mashtY * (GetValue(j,iy0F) - MinY);
          x1 = mashtX * (GetValue(j,ix1F) - MinX);
          y1 = mashtY * (GetValue(j,iy1F) - MinY);
         ix  = int(x);
         iy  = int(y);
         ix1 = int(x1);
         iy1 = int(y1);

         gdImageLine(im_out, ix+shiftX,(iy+shiftY),ix1+shiftX,(iy1+shiftY),  color[i]);
         gdImageSetPixel(im_out, ix+shiftX,(iy+shiftY),   color[iy0F]);
         gdImageSetPixel(im_out, ix1+shiftX,(iy1+shiftY), color[ix1F]);

     }
   }
/******************************************/
  /* Write marks */
   for(i=0;i<nfields;i++)
   { if(!pfields[i].use) continue;
     if(!pfields[i].markType) continue;
     if(i == nfieldUsedAsX) continue;
     if(pfields[i].vectorType) continue;
      color[i] = gdImageColorAllocate(im_out, rand()%256, rand()%256, rand()%256);
      if(pfields[i].markColor != -1)
           color[i] = gdImageColorAllocate(im_out, pfields[i].markColor>>16, (pfields[i].markColor>>8)&0xff, pfields[i].markColor&0xff);
     for(j=0;j<NumRec;j++)
     {
         if(nfieldUsedAsX >=0)
         {     x = mashtX * (GetValue(j,nfieldUsedAsX) - MinX);
         } else {
               x = j * mashtX;
         }

         y = mashtY * (GetValue(j,i) - MinY);
         ix = int(x);
         iy = int(y);
         if(j)
         {   PlotMark(im_out, ix+shiftX,(iy+shiftY),pfields[i].markType, color[i]);
         }
         ixold = x;
         iyold = y;
     }
   }

/* Label axes */
/* X axe */
   for(i=0;i<nfields;i++)
   { if(!pfields[i].use) continue;
      if(i == nfieldUsedAsX)
      {
         gdImageDrawText(im_out, Fonts[1], DimX-shiftX, shiftY+10,(unsigned char*) pfields[i].name,
                         bred,gdDT_RIGHT|gdDT_TOP);
         break;
      }
   }
/* Legend */
   if(LegendUse)
   {  int nf=0;
      int lx,ly;
      char str00[256];
      char form_cv[20];
/* count number of active fields for legend */
      for(i=0;i<nfields;i++)
      { if(!pfields[i].use) continue;
        if(i == nfieldUsedAsX) continue;
        if(pfields[i].vectorType &&
           ((pfields[i].vectorType-1)&0x03) != 0) continue;

        nf++;
      }
      if(LegendStartX == -1)
      {  LegendStartX = DimX - 30;
      }
      if(LegendStartY == -1)
      {  if(DimY > 100) LegendStartY = DimY - (nf+1) * 10;
         else           LegendStartY = int(DimY * 0.8);
      }
      nf = 0;
      strcpy(form_cv,"%f"); //формат вывода последнего значения в легенде
      {  double mmmY;
         mmmY = fabs(MaxY);
         if(fabs(MinY) > mmmY) mmmY = fabs(MinY);
         if(mmmY < 0.01) strcpy(form_cv,"%g");
         else if(mmmY < 0.1) strcpy(form_cv,"%.4f");
         else if(mmmY < 2.)  strcpy(form_cv,"%.3f");
         else if(mmmY < 20.) strcpy(form_cv,"%.2f");
         else if(mmmY < 200.) strcpy(form_cv,"%.1f");
         else if(mmmY < 2000.) strcpy(form_cv,"%.f");
         else  strcpy(form_cv,"%g");
      }
      strcat(form_cv," %s");

      for(i=0;i<nfields;i++)
      { if(!pfields[i].use) continue;
         if(i == nfieldUsedAsX) continue;
         if(pfields[i].vectorType &&
           ((pfields[i].vectorType-1)&0x03) != 0) continue;

         lx = LegendStartX;
         ly = LegendStartY+nf*10;
         nf++;
         color[0] = gdImageColorAllocate(im_out, rand()%256, rand()%256, rand()%256);
         if(pfields[i].color != -1)
            color[0] = gdImageColorAllocate(im_out, pfields[i].color>>16, (pfields[i].color>>8)&0xff, pfields[i].color&0xff);

         color[1] = gdImageColorAllocate(im_out, rand()%256, rand()%256, rand()%256);
         if(AxesNumColor != -1)
            color[1] = gdImageColorAllocate(im_out, AxesNumColor>>16, (AxesNumColor>>8)&0xff, AxesNumColor&0xff);

         gdImageLine(im_out, lx, ly ,lx+20,ly,  color[0]);

         sprintf(str00,form_cv, GetValue(NumRec-1,i)/pfields[i].amplification - pfields[i].shift,pfields[i].name);
         gdImageDrawText(im_out, Fonts[1], lx-5, ly,(unsigned char*) str00 /* pfields[i].name */,
                                     color[1],gdDT_RIGHT|gdDT_VCENTER);
//         gdImageDrawText(im_out, Fonts[1], lx-5, ly,(unsigned char*) pfields[i].name,
//                                     color[1],gdDT_RIGHT|gdDT_VCENTER);
      }
   }


  /* Write PNG */
  out = fopen (FileOut, "wb");
  if(strstr(FileOut,".jpg"))
  {     gdImageJpeg (im_out, out,90); /* 70% */
  } else if(strstr(FileOut,".gif")) {
       gdImageTrueColorToPalette (im_out,  0 /*int ditherFlag */,  256);

       gdImageGif (im_out, out);
  } else {
       gdImageTrueColorToPalette (im_out,  0 /*int ditherFlag */,  256);
       gdImagePng (im_out, out);
  }
  fclose (out);
   return 0;
}

int Diagramm::PlotMark(gdImagePtr im, int x, int y, int type, int color)
{
   switch(type)
   {  case 1: /* pure pixel */
          gdImageSetPixel(im, x, y, color);
        break;
     case 2: /* X - like */
          gdImageLine(im, x-2, y-2, x+2,y+2,  color);
          gdImageLine(im, x-2, y+2, x+2,y-2,  color);
        break;
   }
   return 0;
}

int ReadData(char *fname)
{
  FILE *fp;
  char str[512], *pstr;
  int i,nstr=0,flen, flen0, rc;

/* Count strings */
  fp = fopen(fname,"r");
  if(fp == NULL)
     return 1;
  flen0 = _filelength(fileno(fp));
  if(flen0 > 1024*1024*100) diagramm.BigFile = 1;
  if(diagramm.startLineSeek)
       fseek(fp, diagramm.startLineSeek,SEEK_SET );
  do
  {  i = ftell(fp);
     pstr = fgets(str,511,fp);
     if(pstr && *pstr != ';')
     {  if(diagramm.startLine == nstr)
             diagramm.startLineSeek = i;
        nstr++;
     }
    if(nstr >= diagramm.endLine && diagramm.endLine > 0)
                break;
    if(diagramm.BigFile && ((nstr%1024) == 0) )
    {
        printf("Read %i%% \r", (int)(i*100./flen0)); fflush(stdout);
    }
  } while( !feof(fp));
  diagramm.endLineSeek  = ftell(fp);
  fclose(fp);

  if(diagramm.startLine == diagramm.endLine)
  {      printf("File %s, %i strings\n",fname,nstr);
        flen = flen0;
  }  else  {
     nstr = nstr - diagramm.startLine;
     printf("File %s, read %i strings, (start at %i)\n",fname,nstr,diagramm.startLineSeek);
     flen = diagramm.endLineSeek - diagramm.startLineSeek;
  }
  strcpy(diagramm.FileData,fname);
  diagramm.Lendatabuf = diagramm.recordLen * (nstr+2);
  if(diagramm.Lendatabuf > 1024*1024*200) diagramm.BigFile = 1;
  else                                    diagramm.BigFile = 0;
  if(!diagramm.BigFile)
  {
    diagramm.databuff = (char *) calloc(diagramm.recordLen+1, nstr+2);
    if(diagramm.databuff == NULL)
    {  printf("Can't alloc memory %i bytes (line %i)\n", diagramm.recordLen * nstr, __LINE__);
       exit(1);
    }
  } else {
    diagramm.databuff = NULL;
    diagramm.Lendatabuf = flen;
  }
  for(i=0;i<diagramm.nfields;i++)
  {   diagramm.strptrbuf[i] = (char *)calloc( sizeof(char),256);
     if(diagramm.strptrbuf[i] == NULL)
     {  printf("Can't alloc memory %i bytes (line %i)\n", 256, __LINE__);
        exit(1);
     }
  }


  if(!diagramm.BigFile)
  {
         diagramm.strBuff = (char *) calloc(sizeof(char), flen+16);
         if(diagramm.strBuff == NULL)
         {  printf("Can't alloc memory %i bytes (line %i)\n", flen, __LINE__);
              exit(1);
         }
         diagramm.pstrBuff = diagramm.strBuff;
/* Read data */
  fp = fopen(fname,"r");
  if(fp == NULL)
         return 2;
   if(diagramm.startLineSeek)
       fseek(fp, diagramm.startLineSeek,SEEK_SET );
   for(i=0; i < nstr; i++)
   {  pstr = fgets(str,511,fp);
      if(pstr && *pstr != ';')
        diagramm.AddData(i,str);
   }
   fclose(fp);
   printf("File %s, data was read\n",fname);

  } else {
     diagramm.NumRec = nstr;
  }

  return 0;
}


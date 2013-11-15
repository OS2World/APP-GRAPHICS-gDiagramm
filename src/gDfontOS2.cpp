/* gDfontOS2.cpp */
#define INCL_DOSMISC       /* DOS Miscellaneous values */
#define INCL_DOSERRORS     /* DOS Error values         */
#include <os2.h>
#include <stdio.h>
#include <string.h>


#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define HAVE_LIBPNG

#endif

#include <math.h>
#include "gd.h"
#define DEGTORAD(x) ( (x) * (2.0 * 3.14159265) / 360.0 )

#if 0
/* fontwheeltest.cpp */
void
doerr (FILE * err, const char *msg)
{
  if (err)
    {
      fprintf (err, "%s\n", msg);
      fflush (err);
    }
}

void
dowheel (gdImagePtr im, int color, char *fontfile, int fontsize,
       double angle, int x, int y, int offset, char *string)
{
  int brect[8];
  FILE *err;
  double curangrads, curang, x0, y0;
  char *cp;

  err = fopen ("err.out", "a");
  doerr (err, "------------- New fontwheel --------------");
  doerr (err, fontfile);
  doerr (err, string);
  doerr (err, "------------------------------------------");

  for (curang = 0.0; curang < 360.0; curang += angle)
    {
      curangrads = DEGTORAD(curang);
      x0 = x + cos (curangrads) * offset;
      y0 = y - sin (curangrads) * offset;

      /* The case of newlines is taken care of in the gdImageStringTTF call */
#if defined(OLDER_GD)
      cp = gdImageStringTTF (im, brect, color, fontfile, fontsize,
                         curangrads, x0, y0, string);
#else
      cp = gdImageStringFT (im, brect, color, fontfile, fontsize,
                        curangrads, x0, y0, string);
#endif
      if (cp)
       doerr (err, cp);

      gdImagePolygon (im, (gdPointPtr)brect, 4, color);
    }

  fclose (err);
}

void
dotest (char *font, int size, double incr,
       int w, int h, char *string, const char *filename)
{
  gdImagePtr im;
  FILE *out;
  int bg;
  int fc;
  int xc = w / 2;
  int yc = h / 2;

  im = gdImageCreate (w, h);
  bg = gdImageColorAllocate (im, 0, 0, 0);

  gdImageFilledRectangle (im, 1, 1, w - 1, h - 1, bg);

  fc = gdImageColorAllocate (im, 255, 192, 192);

  out = fopen (filename, "wb");

  dowheel (im, fc, font, size, incr, xc, yc, 20, string);

#if defined(HAVE_LIBPNG)
  gdImagePng (im, out);
#elif defined(HAVE_LIBJPEG)
  gdImageJpeg (im, out, -1);
#endif

  fclose (out);
}

#endif //0

char *font_files[16];

static char *font_files_00[]=
{
"E:\\PSFONTS\\COUR.PFB",
"E:\\PSFONTS\\COURB.PFB",
"E:\\PSFONTS\\COURBI.PFB",
"E:\\PSFONTS\\COURI.PFB",
"E:\\PSFONTS\\HELV.PFB",
"E:\\PSFONTS\\HELVB.PFB",
"E:\\PSFONTS\\HELVBI.PFB",
"E:\\PSFONTS\\HELVI.PFB",
//"E:\\PSFONTS\\MARKSYM.PFB",
//"E:\\PSFONTS\\SYMB.PFB",
"E:\\PSFONTS\\TNR.PFB",
"E:\\PSFONTS\\TNRB.PFB",
"E:\\PSFONTS\\TNRBI.PFB",
"E:\\PSFONTS\\TNRI.PFB",
"E:\\PSFONTS\\mtsans_w.ttf",
"E:\\PSFONTS\\mtsansdj.ttf",
"E:\\PSFONTS\\tnrwt_j.ttf",
NULL
};

#include "Unicode.h"

int Convert866toUnicode(char *in, char *out, int lout)
{  int i, j, n, code;
   char str[80];

   if(in == NULL)
       return -1;
   if(in == NULL)
       return -2;

   for(i=n=0; in[i]; i++)
   {  if(in[i] > 127) n++;
   }
   if(i > lout) return;
   for(i=n=0; in[i]; i++)
   {  if(in[i] <= 127)
      {  out[n++] = in[i];
           if(n >= lout)
           {  out[n-1] = 0;
              return 1;
           }

      } else {
        code = UnicodeTable[in[i]];
        sprintf(str,"&#x%x;",code);
        for(j=0;str[j];j++)
        {  out[n++] = str[j];
           if(n >= lout)
           {  out[n-1] = 0;
              return 1;
           }
        }
      }
   }
   if(n < lout)
   {  out[n] = 0;
   }
   return 0;
}

int GetBootDrive(void)
{ ULONG   nbootdrive = 0;
  APIRET  rc;

  rc = DosQuerySysInfo(QSV_BOOT_DRIVE ,    /* Request Boot drive   */
                       QSV_BOOT_DRIVE ,
                       (PVOID)&nbootdrive,
                       sizeof(ULONG));

  if (rc != NO_ERROR) {
     printf("DosQuerySysInfo error: return code = %u\n", rc);
     return -1;
  }

   return nbootdrive;
}

int InitFonts(void)
{  int i, nboot;
   char *pchr, pstr;

   nboot = GetBootDrive();
   for(i=0; font_files_00[i]; i++)
   {   font_files[i] = strdup(font_files_00[i]);
       pchr = font_files[i];
       *pchr = (char)('A' + nboot-1);
   }
   return 0;
}

#if 0

int
main (int argc, char **argv)
{
   int i, nboot=0;
   char fname[256];
   char str[256], strUnicode[256], *pchr;

   InitFonts();


   for(i=0; font_files[i]; i++)
   {   sprintf(fname, "test\\fontwheeltest%i.png",i);
       sprintf(str,"123 %s >”›‚€¡­ï<Hello, there !",font_files[i]);
       Convert866toUnicode(str, strUnicode, sizeof(strUnicode));
 printf("Font %s -> file %s\n",font_files[i], fname);
       dotest (font_files[i], 32, 30, 1024, 1024, strUnicode,fname);

   }
//#if 0
#if defined(HAVE_LIBPNG)

  dotest ("times", 16, 20.0, 400, 400, "Hello, there!",
         "fontwheeltest1.png");
  dotest ("times", 16, 30.0, 400, 400, "Hello, there!",
         "fontwheeltest2.png");
  dotest ("arial", 16, 45.0, 400, 400, "Hello, there!",
         "fontwheeltest3.png");
  dotest ("arial", 16, 90.0, 400, 400, "Hello\nthere!",
         "fontwheeltest4.png");
#elif defined(HAVE_LIBJPEG)
  dotest ("times", 16, 20.0, 400, 400, "Hello, there!",
         "fontwheeltest1.jpeg");
  dotest ("times", 16, 30.0, 400, 400, "Hello, there!",
         "fontwheeltest2.jpeg");
  dotest ("arial", 16, 45.0, 400, 400, "Hello, there!",
         "fontwheeltest3.jpeg");
  dotest ("arial", 16, 90.0, 400, 400, "Hello\nthere!",
         "fontwheeltest4.jpeg");
#else
  fprintf (stderr, "no PNG or JPEG support\n");
#endif
//#endif //0
  return 0;
}
#endif //0

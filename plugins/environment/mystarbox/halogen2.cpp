/**
 * Idea for this kind of halo has been partially borrowed from GIMP:
 * SuperNova plug-in
 * Copyright (C) 1997 Eiichi Takamori <taka@ma1.seikyou.ne.jp>,
 *                     Spencer Kimball, Federico Mena Quintero
 */

#include <cssysdef.h>
#include <csqint.h>
#include <csutil/randomgen.h>

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <csgfx/rgbpixel.h>

namespace PT /* required to avoid naming conflicts when building static */
{

unsigned char*
csGenerateNova(int iSize, int iSeed, int iNumSpokes, float iRoundness)
{
  csRandomGen rnd (iSeed);
  unsigned char *image = new unsigned char [iSize * iSize];

  const int radius = iSize / 2;
  const int center = radius;

  CS_ALLOC_STACK_ARRAY (float, spoke, iNumSpokes + 2);
  int i;
  for (i = 0; i < iNumSpokes; i++)  spoke [i] = rnd.Get ();//iRoundness + rnd.Get () * (1 - iRoundness);
  spoke [iNumSpokes] = spoke [0];
  spoke [iNumSpokes + 1] = spoke [1];

  int y;
  for (y = 0; y < iSize; y++)
  {
    int x;
    for (x = 0; x < iSize; x++)
    {
      // u is in -1..1 interval
      float u = static_cast<float>(x - center) / radius;
      // v is in -1..1 interval
      float v = static_cast<float>(y - center) / radius;
      // l will never exceed sqrt(2) e.g. 1.4142...
      float l = u * u + v * v;

      // c is a float from 0..iNumSpokes
      float c = (atan2(u, v) / TWO_PI + 0.5f) * iNumSpokes;
      // i is the spoke number
      int i = csQint(c);
      // c is the float part of former c
      c -= i;

      // w1 is the pixel intensity depending on spokes, 0..1
      float w1 = spoke[i] * (1 - c) + spoke[i + 1] * c;
      float w = 1.1f - pow(l, iRoundness);
      if (w < 0.0f) w = 0.0f;
      if (w > 1.0f) w = 1.0f;
      image[x + y * iSize] = csQint(w * (w + (1.0f - w) * w1) * 255.9f);

    }
  }

  return image;
}

unsigned char* csGenerateNovaRGB(int iSize, int iSeed, int iNumSpokes,
                                 float iRoundness, csRGBcolor color)
{
  csRandomGen rnd(iSeed);
  unsigned char *image = new unsigned char[(iSize * iSize) * 4];

  const int radius = iSize / 2;
  const int center = radius;

  CS_ALLOC_STACK_ARRAY (float, spoke, iNumSpokes + 2);
  int i;
  for (i = 0; i < iNumSpokes; i++)
  spoke [i] = rnd.Get ();//iRoundness + rnd.Get () * (1 - iRoundness);
  spoke [iNumSpokes] = spoke [0];
  spoke [iNumSpokes + 1] = spoke [1];

  int y;
  for (y = 0; y < iSize; y++)
  {
    int x;
    for (x = 0; x < iSize; x++)
    {
      // u is in -1..1 interval
      float u = static_cast<float> (x - center) / radius;
      // v is in -1..1 interval
      float v = static_cast<float> (y - center) / radius;
      // l will never exceed sqrt(2) e.g. 1.4142...
      float l = u * u + v * v;

      // c is a float from 0..iNumSpokes
      float c = (atan2(u, v) / TWO_PI + 0.5f) * iNumSpokes;
      // i is the spoke number
      int i = csQint(c);
      // c is the float part of former c
      c -= i;

      // w1 is the pixel intensity depending on spokes, 0..1
      float w1 = spoke[i] * (1 - c) + spoke [i + 1] * c;
      float w = 1.1f - pow (l, iRoundness);
      if (w < 0.0f) w = 0.0f;
      if (w > 1.0f) w = 1.0f;
      int pixel = csQint (w * (w + (1.0f - w) * w1) * 255.9f);

      // Add color
      float color_adjust =  (w - 0.5);
      int r = static_cast<int>(color.red + (color.red * color_adjust));
      int g = static_cast<int>(color.green + (color.green * color_adjust));
      int b = static_cast<int>(color.blue + (color.blue * color_adjust));
      if (r < 0 ) r = 0;
      if (g < 0 ) g = 0;
      if (b < 0 ) b = 0;
      if (r > 255 ) r = 255;
      if (g > 255 ) g = 255;
      if (b > 255 ) b = 255;

      image [((x + y * iSize)*4)] = r ;
      image [((x + y * iSize)*4)+1] = g ;
      image [((x + y * iSize)*4)+2] = b ;
      image [((x + y * iSize)*4)+3] = pixel ;

    }
  }

  return image;
}


// This doesn't look too realistic
//#define HALO_HAS_OUTER_RIM

/**
 * Generate an halo image given halo iSize (iSize x iSize) and
 * halo intensity factor (0..1) (this is NOT the intensity itself!).
 * The iCross argument is an 0..1 float that defines how much the
 * hallo ressembles a cross (0) or a circle (1).
 */
unsigned char *csGenerateHalo(int iSize, float iFactor, float iCross)
{
  unsigned char *image = new unsigned char[iSize * iSize];
  memset(image, 0, iSize * iSize);

  const int s1 = iSize - 1;  // size - 1
  const int s2 = iSize / 2;  // halo diameter
#ifdef HALO_HAS_OUTER_RIM
  int s3 = (s2 / 2);    // outer rim diameter squared
  s3 *= s3;
#endif

  int x;
  for (x = 0; x <= s2; x++)
  {
    int y;
    for (y = /*s2*/x; y <= s2; y++)
    {
      int dx = (s2 - x);
      int dy = (s2 - y);
      float a = pow((pow (float (dx) / float (s2), iCross) +
                      pow (float (dy) / float (s2), iCross)), iFactor);

#ifdef HALO_HAS_OUTER_RIM
      float rim = ABS(s3 - (dx * dx + dy * dy)) / 64.0;
      if (rim < 1)
        a = a * 0.7 + rim * 0.3;
#endif

      if (a <= 1)
      {
        unsigned char uca = 255 - int (255 * a);
        image[     x + (     y) * iSize] = uca;
        image[     y + (     x) * iSize] = uca;
        image[s1 - x + (     y) * iSize] = uca;
        image[s1 - y + (     x) * iSize] = uca;
        image[     x + (s1 - y) * iSize] = uca;
        image[     y + (s1 - x) * iSize] = uca;
        image[s1 - x + (s1 - y) * iSize] = uca;
        image[s1 - y + (s1 - x) * iSize] = uca;
      }
    }
  }

  return image;
}


/**
 * Generate an halo alpha image given halo iSize (iSize x iSize) and
 * halo intensity factor (0..1) (this is NOT the intensity itself!).
 * The iCross argument is an 0..1 float that defines how much the
 * hallo ressembles a cross (0) or a circle (1).
 */
unsigned char *csGenerateHaloAlpha(int iSize, float iFactor, float iCross)
{
  unsigned char *image = new unsigned char[iSize * iSize];
  memset(image, 0, iSize * iSize);

  const int s1 = iSize - 1;  // size - 1
  const int s2 = iSize / 2;  // halo diameter
#ifdef HALO_HAS_OUTER_RIM
  int s3 = (s2 / 2);    // outer rim diameter squared
  s3 *= s3;
#endif

  int x;
  for (x = 0; x <= s2; x++)
  {
    int y;
    for (y = /*s2*/x; y <= s2; y++)
    {
      int dx = (s2 - x);
      int dy = (s2 - y);
      float a = pow((pow (float (dx) / float (s2), iCross) +
                      pow (float (dy) / float (s2), iCross)), iFactor);

#ifdef HALO_HAS_OUTER_RIM
      float rim = ABS (s3 - (dx * dx + dy * dy)) / 64.0;
      if (rim < 1)
        a = a * 0.7 + rim * 0.3;
#endif

      if (a <= 1)
      {
        //unsigned char uca = 255 - int (255 * a);
        unsigned char uca = int (255 * a);
        image[     x + (     y) * iSize] = uca;
        image[     y + (     x) * iSize] = uca;
        image[s1 - x + (     y) * iSize] = uca;
        image[s1 - y + (     x) * iSize] = uca;
        image[     x + (s1 - y) * iSize] = uca;
        image[     y + (s1 - x) * iSize] = uca;
        image[s1 - x + (s1 - y) * iSize] = uca;
        image[s1 - y + (s1 - x) * iSize] = uca;
      }
    }
  }

  return image;
}

}
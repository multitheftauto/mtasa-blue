/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/gentable/gentable_objmodels.cpp
*  PURPOSE:     Object model table generator
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*               KWKSND         <kwksnd@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <stdio.h>
#include <string.h>

#define MAX_OBJECT_ID 20000

static inline void Allow(unsigned int* table, unsigned int pos, bool state)
{
  unsigned int chunk = pos / 32;
  unsigned int shift = pos - (chunk * 32);
  unsigned int bit = 1 << shift;

  if (state)
    table[chunk] |= bit;
  else
    table[chunk] &= ~bit;
}

static inline void AllowRange(unsigned int* table, unsigned int first, unsigned int last, bool state)
{
  for (unsigned int i = first; i <= last; i++)
  {
    Allow(table, i, state);
  }
}

int main()
{
  unsigned int table[MAX_OBJECT_ID / (sizeof(unsigned int) * 8) + 1];
  memset(table, 0, sizeof(table));

  size_t tableSize = sizeof(table) / sizeof(unsigned int);

  Allow(table, 1, true);
  AllowRange(table, 300, 328, true);
  AllowRange(table, 330, 331, true);
  AllowRange(table, 333, 339, true);
  AllowRange(table, 341, 373, true);
  AllowRange(table, 384, 397, true);
  Allow(table, 392, false);
  AllowRange(table, 615, 661, true);
  Allow(table, 664, true);
  AllowRange(table, 669, 698, true);
  AllowRange(table, 700, 792, true);
  AllowRange(table, 800, 906, true);
  AllowRange(table, 910, 998, true);
  AllowRange(table, 1000, 1085, true);
  AllowRange(table, 1088, 1193, true);
  AllowRange(table, 1207, 1372, true);
  AllowRange(table, 1374, 1381, true);
  AllowRange(table, 1383, 1384, true);
  AllowRange(table, 1386, 1395, true);
  AllowRange(table, 1407, 1698, true);
  AllowRange(table, 1700, 2882, true);
  AllowRange(table, 2885, 3117, true);
  AllowRange(table, 3119, 3178, true);
  Allow(table, 3187, true);
  Allow(table, 3193, true);
  Allow(table, 3214, true);
  Allow(table, 3221, true);
  AllowRange(table, 3241, 3244, true);
  Allow(table, 3246, true);
  AllowRange(table, 3249, 3250, true);
  AllowRange(table, 3252, 3253, true);
  AllowRange(table, 3255, 3265, true);
  AllowRange(table, 3267, 3347, true);
  AllowRange(table, 3350, 3415, true);
  AllowRange(table, 3417, 3428, true);
  AllowRange(table, 3430, 3552, true);
  AllowRange(table, 3554, 3558, true);
  AllowRange(table, 3560, 3609, true);
  AllowRange(table, 3612, 3869, true);
  AllowRange(table, 3872, 3882, true);
  AllowRange(table, 3884, 3888, true);
  AllowRange(table, 3890, 3973, true);
  AllowRange(table, 3975, 4541, true);
  AllowRange(table, 4550, 4762, true);
  AllowRange(table, 4806, 5084, true);
  AllowRange(table, 5086, 5089, true);
  AllowRange(table, 5105, 5375, true);
  AllowRange(table, 5390, 5682, true);
  AllowRange(table, 5703, 6010, true);
  AllowRange(table, 6035, 6253, true);
  AllowRange(table, 6255, 6257, true);
  AllowRange(table, 6280, 6347, true);
  AllowRange(table, 6349, 6525, true);
  AllowRange(table, 6863, 7392, true);
  AllowRange(table, 7415, 7973, true);
  AllowRange(table, 7978, 9193, true);
  AllowRange(table, 9205, 9478, true);
  AllowRange(table, 9482, 10310, true);
  AllowRange(table, 10315, 10744, true);
  AllowRange(table, 10750, 11417, true);
  AllowRange(table, 11420, 11681, true);
  AllowRange(table, 12800, 13563, true);
  AllowRange(table, 13590, 13667, true);
  AllowRange(table, 13672, 13890, true);
  AllowRange(table, 14383, 14528, true);
  AllowRange(table, 14530, 14554, true);
  Allow(table, 14556, true);
  AllowRange(table, 14558, 14643, true);
  AllowRange(table, 14650, 14657, true);
  AllowRange(table, 14660, 14695, true);
  AllowRange(table, 14699, 14728, true);
  AllowRange(table, 14735, 14765, true);
  AllowRange(table, 14770, 14856, true);
  AllowRange(table, 14858, 14883, true);
  AllowRange(table, 14885, 14898, true);
  AllowRange(table, 14900, 14903, true);
  AllowRange(table, 15025, 15064, true);
  AllowRange(table, 16000, 16328, true);
  AllowRange(table, 16332, 16790, true);
  AllowRange(table, 17000, 17474, true);
  AllowRange(table, 17500, 17974, true);
  Allow(table, 17976, true);
  Allow(table, 17978, true);
  AllowRange(table, 18000, 18036, true);
  AllowRange(table, 18038, 18102, true);
  AllowRange(table, 18104, 18105, true);
  Allow(table, 18109, true);
  Allow(table, 18112, true);
  AllowRange(table, 18200, 18630, true);

  printf("static unsigned int g_uiValidObjectModels[] = {\n"
         "    ");
  for (unsigned int i = 0; i < tableSize; i++)
  {
    if ( ! (i % 20) && i > 0)
    {
      printf("\n    ");
    }
    printf("%d, ", table[i]);
  }
  printf("\n};\n");

  return 0;
}

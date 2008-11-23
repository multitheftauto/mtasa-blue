/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/gentable/gentable_objmodels.cpp
*  PURPOSE:     Object model table generator
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
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
  AllowRange(table, 615, 661, true);
  Allow(table, 664, true);
  AllowRange(table, 669, 698, true);
  AllowRange(table, 700, 792, true);
  AllowRange(table, 800, 906, true);
  AllowRange(table, 910, 964, true);
  AllowRange(table, 966, 998, true);
  AllowRange(table, 1000, 1193, true);
  AllowRange(table, 1207, 1325, true);
  AllowRange(table, 1327, 1572, true);
  AllowRange(table, 1574, 1656, true);
  Allow(table, 1660, true);
  AllowRange(table, 1662, 1698, true);
  AllowRange(table, 1700, 2882, true);
  AllowRange(table, 2885, 3119, true);
  AllowRange(table, 3121, 3135, true);
  AllowRange(table, 3167, 3175, true);
  Allow(table, 3178, true);
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
  AllowRange(table, 3417, 3424, true);
  AllowRange(table, 3426, 3428, true);
  AllowRange(table, 3430, 3609, true);
  AllowRange(table, 3612, 3783, true);
  AllowRange(table, 3785, 3869, true);
  AllowRange(table, 3873, 3882, true);
  AllowRange(table, 3884, 3888, true);
  AllowRange(table, 3890, 3973, true);
  AllowRange(table, 3975, 4211, true);
  AllowRange(table, 4223, 4541, true);
  AllowRange(table, 4550, 4714, true);
  AllowRange(table, 4718, 4722, true);
  Allow(table, 4724, true);
  AllowRange(table, 4726, 4738, true);
  AllowRange(table, 4753, 4762, true);
  AllowRange(table, 4806, 5056, true);
  AllowRange(table, 5060, 5084, true);
  AllowRange(table, 5086, 5089, true);
  AllowRange(table, 5105, 5375, true);
  AllowRange(table, 5390, 5464, true);
  AllowRange(table, 5466, 5660, true);
  AllowRange(table, 5663, 5664, true);
  AllowRange(table, 5666, 5682, true);
  AllowRange(table, 5703, 5775, true);
  AllowRange(table, 5777, 5785, true);
  AllowRange(table, 5787, 5790, true);
  AllowRange(table, 5792, 5871, true);
  AllowRange(table, 5873, 5875, true);
  AllowRange(table, 5877, 5989, true);
  AllowRange(table, 5993, 6000, true);
  AllowRange(table, 6002, 6009, true);
  AllowRange(table, 6035, 6191, true);
  AllowRange(table, 6197, 6253, true);
  AllowRange(table, 6255, 6256, true);
  AllowRange(table, 6280, 6347, true);
  AllowRange(table, 6349, 6411, true);
  AllowRange(table, 6413, 6517, true);
  AllowRange(table, 6522, 6525, true);
  AllowRange(table, 6863, 7070, true);
  AllowRange(table, 7073, 7096, true);
  AllowRange(table, 7098, 7181, true);
  AllowRange(table, 7184, 7205, true);
  AllowRange(table, 7209, 7219, true);
  AllowRange(table, 7223, 7225, true);
  AllowRange(table, 7227, 7229, true);
  AllowRange(table, 7231, 7262, true);
  Allow(table, 7267, true);
  AllowRange(table, 7269, 7273, true);
  AllowRange(table, 7276, 7277, true);
  AllowRange(table, 7281, 7288, true);
  AllowRange(table, 7291, 7313, true);
  AllowRange(table, 7315, 7330, true);
  AllowRange(table, 7334, 7386, true);
  AllowRange(table, 7388, 7392, true);
  AllowRange(table, 7415, 7891, true);
  AllowRange(table, 7893, 7941, true);
  AllowRange(table, 7945, 7970, true);
  AllowRange(table, 7978, 8369, true);
  AllowRange(table, 8373, 8394, true);
  AllowRange(table, 8396, 8620, true);
  AllowRange(table, 8622, 8700, true);
  AllowRange(table, 8702, 8873, true);
  Allow(table, 8875, true);
  Allow(table, 8877, true);
  Allow(table, 8879, true);
  AllowRange(table, 8881, 9087, true);
  AllowRange(table, 9090, 9093, true);
  AllowRange(table, 9098, 9099, true);
  AllowRange(table, 9102, 9103, true);
  AllowRange(table, 9106, 9120, true);
  AllowRange(table, 9130, 9174, true);
  AllowRange(table, 9176, 9191, true);
  Allow(table, 9193, true);
  AllowRange(table, 9205, 9267, true);
  AllowRange(table, 9269, 9478, true);
  AllowRange(table, 9482, 9884, true);
  AllowRange(table, 9887, 9897, true);
  AllowRange(table, 9899, 9931, true);
  AllowRange(table, 9935, 10056, true);
  AllowRange(table, 10059, 10145, true);
  AllowRange(table, 10148, 10309, true);
  AllowRange(table, 10315, 10743, true);
  AllowRange(table, 10750, 10975, true);
  AllowRange(table, 10977, 11393, true);
  AllowRange(table, 11395, 11409, true);
  AllowRange(table, 11413, 11416, true);
  AllowRange(table, 11420, 11676, true);
  AllowRange(table, 11678, 11681, true);
  AllowRange(table, 12800, 13460, true);
  AllowRange(table, 13462, 13483, true);
  AllowRange(table, 13486, 13492, true);
  AllowRange(table, 13494, 13561, true);
  Allow(table, 13563, true);
  AllowRange(table, 13590, 13667, true);
  AllowRange(table, 13672, 13890, true);
  AllowRange(table, 14383, 14495, true);
  AllowRange(table, 14497, 14528, true);
  AllowRange(table, 14530, 14540, true);
  AllowRange(table, 14542, 14554, true);
  Allow(table, 14556, true);
  AllowRange(table, 14558, 14614, true);
  AllowRange(table, 14616, 14630, true);
  AllowRange(table, 14632, 14633, true);
  AllowRange(table, 14635, 14643, true);
  AllowRange(table, 14650, 14657, true);
  AllowRange(table, 14660, 14695, true);
  AllowRange(table, 14699, 14728, true);
  AllowRange(table, 14735, 14762, true);
  AllowRange(table, 14764, 14765, true);
  AllowRange(table, 14770, 14856, true);
  AllowRange(table, 14858, 14883, true);
  AllowRange(table, 14885, 14898, true);
  AllowRange(table, 14900, 14903, true);
  AllowRange(table, 15025, 15043, true);
  AllowRange(table, 15046, 15064, true);
  AllowRange(table, 16000, 16790, true);
  AllowRange(table, 17000, 17474, true);
  AllowRange(table, 17500, 17578, true);
  AllowRange(table, 17580, 17953, true);
  AllowRange(table, 17958, 17974, true);
  Allow(table, 17976, true);
  Allow(table, 17978, true);
  AllowRange(table, 18000, 18036, true);
  AllowRange(table, 18038, 18075, true);
  AllowRange(table, 18077, 18086, true);
  AllowRange(table, 18088, 18092, true);
  AllowRange(table, 18094, 18095, true);
  AllowRange(table, 18098, 18101, true);
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

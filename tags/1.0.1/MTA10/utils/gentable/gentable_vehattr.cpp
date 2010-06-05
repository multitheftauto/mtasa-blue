/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/gentable/gentable_vehattr.cpp
*  PURPOSE:     Vehicle attribute table generator
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <stdio.h>
#include <string.h>

enum eVehicleAttribute
{
  VEHICLE_HAS_TURRENT = 0x0001,
  VEHICLE_HAS_SIRENS  = 0x0002,
  VEHICLE_HAS_LANDING_GEARS = 0x0004,
  VEHICLE_HAS_ADJUSTABLE_PROPERTY = 0x0008,
  VEHICLE_HAS_SMOKE_TRAIL = 0x0010
};

void SetAttribute(unsigned long* table, unsigned long ulModel, eVehicleAttribute attribute)
{
  table[ulModel - 400] |= attribute;
}

int main ()
{
  unsigned long table[611 - 400];
  unsigned long tableSize = sizeof(table) / sizeof(unsigned long);

  memset(table, 0, sizeof(table));

  SetAttribute(table, 432, VEHICLE_HAS_TURRENT);
  SetAttribute(table, 407, VEHICLE_HAS_TURRENT);
  SetAttribute(table, 601, VEHICLE_HAS_TURRENT);

  SetAttribute(table, 523, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 407, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 544, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 416, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 427, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 490, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 528, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 596, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 597, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 598, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 599, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 601, VEHICLE_HAS_SIRENS);
  SetAttribute(table, 423, VEHICLE_HAS_SIRENS);

  SetAttribute(table, 592, VEHICLE_HAS_LANDING_GEARS);
  SetAttribute(table, 577, VEHICLE_HAS_LANDING_GEARS);
  SetAttribute(table, 553, VEHICLE_HAS_LANDING_GEARS);
  SetAttribute(table, 476, VEHICLE_HAS_LANDING_GEARS);
  SetAttribute(table, 519, VEHICLE_HAS_LANDING_GEARS);
  SetAttribute(table, 520, VEHICLE_HAS_LANDING_GEARS);

  SetAttribute(table, 520, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 525, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 524, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 486, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 406, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 443, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 531, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 530, VEHICLE_HAS_ADJUSTABLE_PROPERTY);
  SetAttribute(table, 592, VEHICLE_HAS_ADJUSTABLE_PROPERTY);

  SetAttribute(table, 512, VEHICLE_HAS_SMOKE_TRAIL);
  SetAttribute(table, 513, VEHICLE_HAS_SMOKE_TRAIL);


  printf("unsigned long g_ulVehicleAttributes [] = {\n"
         "    ");
  for ( int i = 0; i < tableSize; i++ )
  {
    if ( ! ( i % 25 ) && i > 0 )
    {
      printf("\n    ", i-25+400, i+400-1);
    }
    printf("%lu, ", table[i]);
  }
  printf("\n};\n");

  return 0;
}

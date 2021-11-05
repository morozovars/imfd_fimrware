#ifndef RTD_H_
#define RTD_H_


/******************************************************************/
/****************** RTD Information *******************************/
/******************************************************************/

/*  RTDs are categorized by their nominal resistance at 0°C. 
    A Pt100 sensor has a temperature coefficient of approximately
    0.385 ?/°C and a Pt1000 has a temperature coefficient that is a 
    factor of 10 greater than the Pt100. Many system designers use 
    these coefficients to get an approximate resistance to 
    temperature translation, but the Callendar-Van Dusen equations 
    provide a more accurate translation.
    Link: https://en.wikipedia.org/wiki/Callendar%E2%80%93Van_Dusen_equation
    */


/* TABLES */
#define RTD_TABLE_SIZE                  41    // Table variable range is -50 degrees to 251 degrees.
#define PT1000_TABLE                    { 1000.0,                                                                           \
                                          1003.9, 1007.8, 1011.7, 1015.6, 1019.5, 1023.4, 1027.3, 1031.2, 1035.1, 1039.0,   \
                                          1042.9, 1046.8, 1050.7, 1054.6, 1058.5, 1062.4, 1066.3, 1070.2, 1074.0, 1077.9,   \
                                          1081.8, 1085.7, 1089.6, 1093.5, 1097.3, 1101.2, 1105.1, 1109.0, 1112.9, 1116.7,   \
                                          1120.6, 1124.5, 1128.3, 1132.2, 1136.1, 1140.0, 1143.8, 1147.7, 1151.5, 1155.4,   \
                                        }
#define PT1000_OFFSET                   0.0f  // RTD offset resistance [ohm].
#define PT1000_TABLE_TOP_TEMPERATURE    0.0f  // First data of table variable is -50 degrees.
#define PT1000_RREF                     5020  // Fref resistance [ohm].

typedef const struct 
{
    const float table[RTD_TABLE_SIZE];
    const float table_top_temp;
} rtd_table_t;


#define RTD_PT1000_TABLE                                                          \
{                                                                                 \
    .table              = PT1000_TABLE,                                           \
    .table_top_temp     = PT1000_TABLE_TOP_TEMPERATURE,                           \
}
/* TABLES */


/**
  * @brief  binary search
  * @param  *p_data_table - pointer of table to search
  * @param  table_size - number of elements of table to search
  * @param  data - data to search
  * @Return  index
  */
static inline uint16_t table_binary_search(const float * p_data_table, uint16_t table_size, float data)
{
  uint16_t low = 0;
  uint16_t mid = 0;
  uint16_t high = 0;

  // Binary search Initialization

  high = table_size - 1;  // Maximum suffix of table. Table size is 2 or more, uint16_t range
  low = 0;                // Minimum suffix of table

  // binary search
  while ((high - low) > 1)
  {
    // Cast to integer type
    mid = (uint16_t) ((high + low) / 2);    // Select midpoint of search range
    if (p_data_table[mid] <= data)          // Search range update
    {
      low = mid;
    }
    else
    {
      high = mid;
    }
  }

  return low;
}


/**
 * @brief  Linear interpolation function
 * @param  x0 - the value of x0
 * @param  y0 - the value of y0
 * @param  x1 - the value of x1
 * @param  y1 - the value of y1
 * @param  x - the value of x
 * @Return  y
 */
static inline float linear_interpolation(float x0, float y0, float x1, float y1, float x)
{
  float y = 0;
  if (0 != (x1 - x0))
  {
    y = y0 + (((y1 - y0) * (x - x0)) / (x1 - x0));
  }
  return y;
}


/**
 * @brief  Convert RTD resistance to temperature (Celsius).
 * @param  p_rtd_table      - pointer to rtd table
 * @param  table_size       - size of the rtd_table
 * @param  resistance       - resistance of the sensors, needs to be converted to temperature
 * @Return temperature
 */
static float rtd_R2Temp(rtd_table_t * p_rtd_table, float resistance)
{
    uint16_t idx = table_binary_search((float *)(p_rtd_table->table), sizeof(p_rtd_table->table), resistance);

    float x0 = p_rtd_table->table[idx];          /// x0:Temperature range lower limit resistance [ohm].
    float x1 = p_rtd_table->table[idx + 1];      /// x1:Temperature range upper limit resistance [ohm].

    float y0 = (float)idx + p_rtd_table->table_top_temp;            /// Convert idx to temperature.
    float y1 = (float)(idx + 1)+ p_rtd_table->table_top_temp;       /// Convert idx + 1 to temperature.

    float temperature = linear_interpolation(x0, y0, x1, y1, resistance);   /// Calculated temperature [degree].

    return temperature;
}


#endif // RTD_H_
#include "Gassensors.h"

Gassensors::Gassensors(uint8_t pin, uint8_t type, uint8_t Gas_Name) {
  _pin = pin;
  _type = type;
  //_Gas_Name = Gas_Name;
  switch (_type) {
    case 2:
      {
        //mqgases = {0,1,2,3,4,5,6};
        RL_VALUE = RL_VALUE_MQ2;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ2;
      }
      break;
    case 3:
      { RL_VALUE = RL_VALUE_MQ3;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ3;
      }
      break;
    case 4:
      { RL_VALUE = RL_VALUE_MQ4;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ4;
      }
      break;
    case 5:
      { RL_VALUE = RL_VALUE_MQ5;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ5;
      }
      break;
    case 6:
      { RL_VALUE = RL_VALUE_MQ6;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ6;
      }
      break;
    case 7:
      { RL_VALUE = RL_VALUE_MQ7;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ7;
      }
      break;
    case 8:
      { RL_VALUE = RL_VALUE_MQ8;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ8;
      }
      break;
    case 9:
      { RL_VALUE = RL_VALUE_MQ9;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ9;
      }
      break;
    case 135:
      { RL_VALUE = RL_VALUE_MQ135;
        RO_CLEAN_AIR_FACTOR = RO_CLEAN_AIR_FACTOR_MQ135;
      }
      break;
      //default:
      //break;
  }
}

/*Gassensors::Gassensors(mq_pin pin) {
  _pin = pin;
  }

  /****************** MQResistanceCalculation ****************************************
  Input:   raw_adc - raw value read from adc, which represents the voltage
  Output:  the calculated sensor resistance
  Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
float Gassensors::MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/***************************** MQCalibration ****************************************
  Input:   mq_pin - analog channel
  Output:  Ro of the sensor
  Remarks: This function assumes that the sensor is in clean air. It use
         MQResistanceCalculation to calculates the sensor resistance in clean air
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
         10, which differs slightly between different sensors.
************************************************************************************/
float Gassensors::MQCalibration(int mq_pin)
{
  int i, r0;
  float RS_AIR_val = 0;

  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {              //take multiple samples
    RS_AIR_val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  RS_AIR_val = RS_AIR_val / CALIBARAION_SAMPLE_TIMES;           //calculate the average value

  r0 = RS_AIR_val / RO_CLEAN_AIR_FACTOR;                        //RS_AIR_val divided by RO_CLEAN_AIR_FACTOR yields the Ro
  //according to the chart in the datasheet
  //Serial.print("r0 value");
  //Serial.println(r0);
  switch (_type) {
    case 2:
      //_Ro2 = r0;
      _Rovalues[0] = r0;
      //Serial.print("r02 value:  ");
      //Serial.println(_Ro2);
      break;
    case 3:
      //_Ro3 = r0;
      _Rovalues[1] = r0;
      break;
    case 4:
      //_Ro4 = r0;
      _Rovalues[2] = r0;
      //Serial.print("r04 value:  ");
      //Serial.println(_Ro4);
      break;
    case 5:
      //_Ro5 = r0;
      _Rovalues[3] = r0;
      break;
    case 6:
      //_Ro6 = r0;
      _Rovalues[4] = r0;
      break;
    case 7:
      //_Ro7 = r0;
      _Rovalues[5] = r0;
      break;
    case 8:
      //_Ro8 = r0;
      _Rovalues[6] = r0;
      break;
    case 9:
      //_Ro9 = r0;
      _Rovalues[7] = r0;
      break;
    case 135:
      //_Ro135 = r0;
      _Rovalues[8] = r0;
      //Serial.print("r0135 value:  ");
      //Serial.println(_Ro135);
      break;
  }
  return r0;
}

/*****************************  MQRead *********************************************
  Input:   mq_pin - analog channel
  Output:  Rs of the sensor
  Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/
float Gassensors::MQRead(int mq_pin)
{
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 


  rs = rs / READ_SAMPLE_TIMES;
//Serial.print("rs value:  ");
// Serial.print(rs);
  return rs;
}

/*****************************  MQGetGasPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
  Output:  ppm of the target gas
  Remarks: This function uses different equations representing curves of each gas to
         calculate the ppm (parts per million) of the target gas.
************************************************************************************/
int Gassensors::MQGetGasPercentage(float rs_ro_ratio, int gas_id )
{
  switch (_type) {

    case 2:
      Serial.print("calculating mq2 gases: xxxxxx     ");
      Serial.print(gas_id);
            Serial.print("gas_id      ");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq2 GAS_HYDROGEN");
          return (pow(10, ((-2.109 * (log10(rs_ro_ratio))) + 2.983)));
        } else if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq2 GAS_LPG");
          return (pow(10, ((-2.123 * (log10(rs_ro_ratio))) + 2.758)));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq2 GAS_METHANE");
          return (pow(10, ((-2.622 * (log10(rs_ro_ratio))) + 3.635)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq2 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-2.955 * (log10(rs_ro_ratio))) + 4.457)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq2 GAS_ALCOHOL");
          return (pow(10, ((-2.692 * (log10(rs_ro_ratio))) + 3.545)));
        } else if ( gas_id == GAS_SMOKE ) {
          Serial.print("calculating mq2 GAS_SMOKE");
          return (pow(10, ((-2.331 * (log10(rs_ro_ratio))) + 3.596)));
        } else if ( gas_id == GAS_PROPANE ) {
          Serial.print("calculating mq2 GAS_PROPANE");
          return (pow(10, ((-2.174 * (log10(rs_ro_ratio))) + 2.799)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, ((-2.109 * (log10(rs_ro_ratio))) + 2.983)));
        } else if ( gas_id == GAS_LPG ) {
          return (pow(10, ((-2.123 * (log10(rs_ro_ratio))) + 2.758)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, ((-2.622 * (log10(rs_ro_ratio))) + 3.635)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, ((-2.955 * (log10(rs_ro_ratio))) + 4.457)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, ((-2.692 * (log10(rs_ro_ratio))) + 3.545)));
        } else if ( gas_id == GAS_SMOKE ) {
          return (pow(10, (-0.976 * pow((log10(rs_ro_ratio)), 2) - 2.018 * (log10(rs_ro_ratio)) + 3.617)));
        } else if ( gas_id == GAS_PROPANE ) {
          return (pow(10, ((-2.174 * (log10(rs_ro_ratio))) + 2.799)));
        }
      }
      break;

    case 3:
      Serial.print("calculating mq3 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq3 GAS_ALCOHOL");
          return (pow(10, ((-1.487 * (log10(rs_ro_ratio))) - 0.401)));
        } else if ( gas_id == GAS_BENZENE ) {
          Serial.print("calculating mq3 GAS_BENZENE");
          return (pow(10, ((-2.659 * (log10(rs_ro_ratio))) + 0.659)));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq3 GAS_METHANE");
          return (pow(10, ((-17.95 * (log10(rs_ro_ratio))) + 29.59)));
        } else if ( gas_id == GAS_HEXANE ) {
          Serial.print("calculating mq3 GAS_HEXANE");
          return (pow(10, ((-2.851 * (log10(rs_ro_ratio))) + 3.889)));
        } else if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq3 GAS_LPG");
          return (pow(10, ((-3.386 * (log10(rs_ro_ratio))) + 4.915)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq3 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-3.947 * (log10(rs_ro_ratio))) + 5.924)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, ((-1.487 * (log10(rs_ro_ratio))) - 0.401)));
        } else if ( gas_id == GAS_BENZENE ) {
          return (pow(10, (-2.877 * pow((log10(rs_ro_ratio)), 3) + 2.084 * pow((log10(rs_ro_ratio)), 2) - 2.925 * (log10(rs_ro_ratio)) + 0.643)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, ((-17.95 * (log10(rs_ro_ratio))) + 29.59)));
        } else if ( gas_id == GAS_HEXANE ) {
          return (pow(10, (1.795 * pow((log10(rs_ro_ratio)), 3) - 7.343 * pow((log10(rs_ro_ratio)), 2) + 7.020 * (log10(rs_ro_ratio)) - 0.467)));
        } else if ( gas_id == GAS_LPG ) {
          return (pow(10, (-2.335 * pow((log10(rs_ro_ratio)), 2) + (3.263 * (log10(rs_ro_ratio))) + 0.253)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (-14.40 * pow((log10(rs_ro_ratio)), 3) + 59.52 * pow((log10(rs_ro_ratio)), 2) - 84.98 * (log10(rs_ro_ratio)) + 42.26)));
        }
      }
      break;

    case 4:
      Serial.print("calculating mq4 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq4 gas lpg");
          Serial.print(pow(10, ((-3.123 * (log10(rs_ro_ratio))) + 3.565)));
          return (pow(10, ((-3.123 * (log10(rs_ro_ratio))) + 3.565)));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq4 gas GAS_METHANE");
          return (pow(10, ((-2.849 * (log10(rs_ro_ratio))) + 2.997)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq4 gas GAS_HYDROGEN");
          return (pow(10, ((-5.661 * (log10(rs_ro_ratio))) + 5.566 )));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq4 gas GAS_CARBON_MONOXIDE");
          return (pow(10, ((-19.54 * (log10(rs_ro_ratio))) + 14.5)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq4 gas GAS_ALCOHOL");
          return (pow(10, ((-13.17 * (log10(rs_ro_ratio))) + 10.35)));
        } else if ( gas_id == GAS_SMOKE ) {
          Serial.print("calculating mq4 gas GAS_SMOKE");
          return (pow(10, ((-9.016 * (log10(rs_ro_ratio))) + 7.823)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_LPG ) {
          return (pow(10, ((-3.123 * (log10(rs_ro_ratio))) + 3.565)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, ((-2.849 * (log10(rs_ro_ratio))) + 2.997)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, ((-5.661 * (log10(rs_ro_ratio))) + 5.566 )));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (-12533 * pow((log10(rs_ro_ratio)), 4) + 29290 * pow((log10(rs_ro_ratio)), 3) - 25632 * pow((log10(rs_ro_ratio)), 2) + 99520 * (log10(rs_ro_ratio)) - 14459)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, (613.3 * pow((log10(rs_ro_ratio)), 3) - 1021 * pow((log10(rs_ro_ratio)), 2) + 551.9 * (log10(rs_ro_ratio)) - 93.51)));
        } else if ( gas_id == GAS_SMOKE ) {
          return (pow(10, (1055 * pow((log10(rs_ro_ratio)), 4) - 2234 * pow((log10(rs_ro_ratio)), 3) + 1729 * pow((log10(rs_ro_ratio)), 2) - 589.1 * (log10(rs_ro_ratio)) + 78.94)));
        }
      }
      break;

    case 5:
      Serial.print("calculating mq5 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq5 GAS_HYDROGEN");
          return (pow(10, ((-3.986 * (log10(rs_ro_ratio))) + 3.075)));
        } else if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq5 GAS_LPG");
          return (pow(10, ((-2.513 * (log10(rs_ro_ratio))) + 1.878)));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq5 GAS_METHANE");
          return (pow(10, ((-2.554 * (log10(rs_ro_ratio))) + 2.265 )));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq5 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-6.900 * (log10(rs_ro_ratio))) + 6.241)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq5 GAS_ALCOHOL");
          return (pow(10, ((-4.590 * (log10(rs_ro_ratio))) + 4.851)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, (-22.89 * pow((log10(rs_ro_ratio)), 3) + 8.873 * pow((log10(rs_ro_ratio)), 2) - 3.587 * (log10(rs_ro_ratio)) + 2.948)));
        } else if ( gas_id == GAS_LPG ) {
          return (pow(10, ((-2.513 * (log10(rs_ro_ratio))) + 1.878)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, (-0.428 * pow((log10(rs_ro_ratio)), 2) - 2.867 * (log10(rs_ro_ratio)) + 2.224)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (1401 * pow((log10(rs_ro_ratio)), 4) - 2777 * pow((log10(rs_ro_ratio)), 3) + 2059 * pow((log10(rs_ro_ratio)), 2) - 682.5 * (log10(rs_ro_ratio)) + 88.81)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, (14.90 * pow((log10(rs_ro_ratio)), 3) - 19.26 * pow((log10(rs_ro_ratio)), 2) + 3.108 * (log10(rs_ro_ratio)) + 3.922)));
        }
      }
      break;

    case 6:
      Serial.print("calculating mq6 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq6 GAS_LPG");
          return (pow(10, ((-2.351 * (log10(rs_ro_ratio))) + 3.014)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq6 GAS_HYDROGEN");
          return (pow(10, ((-3.613 * (log10(rs_ro_ratio))) + 4.962)));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq6 GAS_METHANE");
          return (pow(10, ((-2.501 * (log10(rs_ro_ratio))) + 3.341 )));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq6 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-12.51 * (log10(rs_ro_ratio))) + 14.21)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq6 GAS_ALCOHOL");
          return (pow(10, ((-5.885 * (log10(rs_ro_ratio))) + 7.643)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_LPG ) {
          return (pow(10, ((-2.351 * (log10(rs_ro_ratio))) + 3.014)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, (73.80 * pow((log10(rs_ro_ratio)), 4) - 172.7 * pow((log10(rs_ro_ratio)), 3) + 150.3 * pow((log10(rs_ro_ratio)), 2) - 60.77 * (log10(rs_ro_ratio)) + 12.87)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, ((-2.501 * (log10(rs_ro_ratio))) + 3.341 )));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (-5659 * pow((log10(rs_ro_ratio)), 4) + 20365 * pow((log10(rs_ro_ratio)), 3) - 27456 * pow((log10(rs_ro_ratio)), 2) + 16423 * (log10(rs_ro_ratio)) - 3671)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, (-26.73 * pow((log10(rs_ro_ratio)), 3) + 62.35 * pow((log10(rs_ro_ratio)), 2) - 54.01 * (log10(rs_ro_ratio)) + 19.93)));
        }
      }
      break;

    case 7:
      Serial.print("calculating mq7 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq7 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-1.525 * (log10(rs_ro_ratio))) + 1.994)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq7 GAS_HYDROGEN");
          return (pow(10, ((-1.355 * (log10(rs_ro_ratio))) + 1.847)));
        } else if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq7 GAS_LPG");
          return (pow(10, ((-7.622 * (log10(rs_ro_ratio))) + 8.919 )));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq7 GAS_METHANE");
          return (pow(10, ((-11.01 * (log10(rs_ro_ratio))) + 14.32)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq7 GAS_ALCOHOL");
          return (pow(10, ((-14.72 * (log10(rs_ro_ratio))) + 19.31)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, ((-1.525 * (log10(rs_ro_ratio))) + 1.994)));
        } else if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, ((-1.355 * (log10(rs_ro_ratio))) + 1.847)));
        } else if ( gas_id == GAS_LPG ) {
          return (pow(10, (61.00 * pow((log10(rs_ro_ratio)), 3) - 148.7 * pow((log10(rs_ro_ratio)), 2) + 112.6 * (log10(rs_ro_ratio)) - 23.30)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, (3480 * pow((log10(rs_ro_ratio)), 4) - 14193 * pow((log10(rs_ro_ratio)), 3) + 21627 * pow((log10(rs_ro_ratio)), 2) - 14606 * (log10(rs_ro_ratio)) + 3694)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, (-25729 * pow((log10(rs_ro_ratio)), 4) + 11746 * pow((log10(rs_ro_ratio)), 3) - 20087 * pow((log10(rs_ro_ratio)), 2) + 15250 * (log10(rs_ro_ratio)) - 43357)));
        }
      }
      break;

    case 8:
     Serial.print("calculating mq8 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          Serial.print("calculating mq8 GAS_HYDROGEN");
          return (pow(10, ((-2.568 * (log10(rs_ro_ratio))) + 0.360)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          Serial.print("calculating mq8 GAS_ALCOHOL");
          return (pow(10, ((-14.45 * (log10(rs_ro_ratio))) + 2.001)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq8 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-32.24 * (log10(rs_ro_ratio))) + 1.126 )));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq8 GAS_METHANE");
          return (pow(10, ((-16.16 * (log10(rs_ro_ratio))) + 1.093)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_HYDROGEN ) {
          return (pow(10, ((-2.568 * (log10(rs_ro_ratio))) + 0.360)));
        } else if ( gas_id == GAS_ALCOHOL ) {
          return (pow(10, (-16078 * pow((log10(rs_ro_ratio)), 4) - 5066 * pow((log10(rs_ro_ratio)), 3) - 541.7 * pow((log10(rs_ro_ratio)), 2) - 36.87 * (log10(rs_ro_ratio)) + 1.717)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (-30000000 * pow((log10(rs_ro_ratio)), 4) - 4000000 * pow((log10(rs_ro_ratio)), 3) - 20282 * pow((log10(rs_ro_ratio)), 2) - 4515 * (log10(rs_ro_ratio)) - 35.30)));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, (-281.3 * pow((log10(rs_ro_ratio)), 3) - 12.26 * pow((log10(rs_ro_ratio)), 2) - 7.925 * (log10(rs_ro_ratio)) + 1.668)));
        }
      }
      break;

    case 9:
      Serial.print("calculating mq9 gases");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_LPG ) {
          Serial.print("calculating mq9 GAS_LPG");
          return (pow(10, ((-2.132 * (log10(rs_ro_ratio))) + 2.992)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          Serial.print("calculating mq9 GAS_CARBON_MONOXIDE");
          return (pow(10, ((-2.199 * (log10(rs_ro_ratio))) + 2.766 )));
        } else if ( gas_id == GAS_METHANE ) {
          Serial.print("calculating mq9 GAS_METHANE");
          return (pow(10, ((-2.636 * (log10(rs_ro_ratio))) + 3.646)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_LPG ) {
          return (pow(10, ((-2.132 * (log10(rs_ro_ratio))) + 2.992)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, ((-2.199 * (log10(rs_ro_ratio))) + 2.766 )));
        } else if ( gas_id == GAS_METHANE ) {
          return (pow(10, (-0.670 * pow((log10(rs_ro_ratio)), 2) - 2.399 * (log10(rs_ro_ratio)) + 3.650)));
        }
      }
      break;

    case 135:
      Serial.print("calculating mq135 gases: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
     Serial.print(rs_ro_ratio);
    //  Serial.print(gas_id);
        //          Serial.print("gas_id      ");

      if ( accuracy == 0 ) {
        if ( gas_id == GAS_CARBON_DIOXIDE ) {
                Serial.println("calculating mq135 co2 gases");

          return (pow(10, ((-2.890 * (log10(rs_ro_ratio))) + 2.055)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
               /*Serial.println("calculating mq135 co gases"); 
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
               Serial.print("log of rs_ro_ratio: ");
               Serial.println(log10(rs_ro_ratio));
               Serial.print("3.891* log of rs_ro_ratio: ");
               Serial.println((-3.891 * (log10(rs_ro_ratio))));
               Serial.print("pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println(pow(10, ((-3.891 * (log10(rs_ro_ratio))))));
               Serial.print("2.75+pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println((pow(10, ((-3.891 * (log10(rs_ro_ratio))) + 2.750))));*/
          return (pow(10, ((-3.891 * (log10(rs_ro_ratio))) + 2.750)));
        } else if ( gas_id == GAS_3by4AE ) {
                Serial.println("calculating mq135 3by4ae gases");

          return (pow(10, ((-3.181 * (log10(rs_ro_ratio))) + 1.895)));
        } else if ( gas_id == GAS_AMMONIUM ) {
                Serial.println("calculating mq135 ammonium gases");

          return (pow(10, ((-2.469 * (log10(rs_ro_ratio))) + 2.005)));
        } else if (gas_id == GAS_1by4 ) {
                Serial.println("calculating mq135 1by4 gases");

          return (pow(10, ((-3.479 * (log10(rs_ro_ratio))) + 1.658)));
        } else if ( gas_id == GAS_NAME ) {
                Serial.println("calculating mq135 name gases");

          return (pow(10, ((-3.452 * (log10(rs_ro_ratio))) + 1.542)));
        }
      }
      else if ( accuracy == 1 ) {
        if ( gas_id == GAS_CARBON_DIOXIDE ) {
          return (pow(10, ((-2.890 * (log10(rs_ro_ratio))) + 2.055)));
        } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
          return (pow(10, (1.457 * pow((log10(rs_ro_ratio)), 2) - 4.725 * (log10(rs_ro_ratio)) + 2.855)));
        } else if ( gas_id == GAS_3by4AE ) {
          return (pow(10, ((-3.181 * (log10(rs_ro_ratio))) + 1.895)));
        } else if (gas_id == GAS_AMMONIUM ) {
          return (pow(10, ((-2.469 * (log10(rs_ro_ratio))) + 2.005)));
        } else if ( gas_id == GAS_1by4 ) {
          return (pow(10, ((-3.479 * (log10(rs_ro_ratio))) + 1.658)));
        } else if ( gas_id == GAS_NAME ) {
          return (pow(10, (-1.004 * pow((log10(rs_ro_ratio)), 2) - 3.525 * (log10(rs_ro_ratio)) + 1.553)));
        }
      }
      break;
  }
  return 0;
}

float Gassensors::rs_ro_ratio(uint8_t pin)
{
  switch (_type) {

    case 2:
      /*{Serial.print("   rs2 value now: ");
      float x = MQRead(pin);
      Serial.print(x);
      Serial.print("    r02 value now:  ");
      Serial.print(_Ro2);
       Serial.print("   rs_ro2_ratio now: ");
      Serial.print(x/_Ro2);
      return x/_Ro2;}
      //Serial.print(MQRead(pin) / _Ro2);*/
      return MQRead(pin) / _Rovalues[0];
      break;

    case 3:
      return MQRead(pin) / _Rovalues[1];
      break;

    case 4:
      /*{
      Serial.print("rs_ro4_ratio now");
      float x = MQRead(pin)/_Ro4;
      Serial.print(x);}*/
      return MQRead(pin) / _Rovalues[2];
      break;

    case 5:
      return MQRead(pin) / _Rovalues[3];
      break;

    case 6:
      return MQRead(pin) / _Rovalues[4];
      break;

    case 7:
      return MQRead(pin) / _Rovalues[5];
      break;

    case 8:
      return MQRead(pin) / _Rovalues[6];
      break;

    case 9:
      return MQRead(pin) / _Rovalues[7];
      break;

    case 135:
          Serial.print("rs_ro135_ratio");
      Serial.print(MQRead(pin) / _Rovalues[8]);
      return MQRead(pin)/_Rovalues[8];
      break;

    default:
      Serial.print("rs_ro default case executed");

  }
}
  int Gassensors::gasppm(uint8_t pin, uint8_t type, int mqgases_id, uint8_t Gas_Name)
  {
    return (MQGetGasPercentage(rs_ro_ratio(pin), mqgases_id ));
  }

  void Gassensors::printallgases(uint8_t type)
  {
    switch (_type) {

        case 2:
        {int mq2gases[]={0,1,2,3,4,5,6};
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq2gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq2gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq2gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq2gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq2gases[4]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("SMOKE:");
        Serial.print(gasppm(_pin, _type, mq2gases[5]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("PROPANE:");
        Serial.print(gasppm(_pin, _type, mq2gases[6]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

     /* case 3:
        {int mq3gases[] = {4,7,2,8,1,3};
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq3gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("BENZENE:");
        Serial.print(gasppm(_pin, _type, mq3gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq3gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("HEXANE:");
        Serial.print(gasppm(_pin, _type, mq3gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq3gases[4]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq3gases[5]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 4:
        {int mq4gases[] = {1,2,0,3,4,5};
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq4gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq4gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq4gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq4gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq4gases[4]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("SMOKE:");
        Serial.print(gasppm(_pin, _type, mq4gases[5]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 5:
        {int mq5gases[] = {0,1,2,3,4};
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq5gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq5gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq5gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq5gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq5gases[4]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 6:
        {int mq6gases[] = {1,0,2,3,4};
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq6gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq6gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq6gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq6gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq6gases[4]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 7:
        {int mq7gases[] = {3,0,1,2,4};
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq7gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq7gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq7gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq7gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq7gases[4]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 8:
        {int mq8gases[] = {0,4,3,2};
        Serial.print("HYDROGEN:");
        Serial.print(gasppm(_pin, _type, mq8gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("ALCOHOL:");
        Serial.print(gasppm(_pin, _type, mq8gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq8gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq8gases[3]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 9:
        {int mq9gases[] = {1,3,2};
        Serial.print("LPG:");
        Serial.print(gasppm(_pin, _type, mq9gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq9gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("METHANE:");
        Serial.print(gasppm(_pin, _type, mq9gases[2]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;

      case 135:
        {int mq135gases[] = {9,3,10,11,12,13};
        Serial.print("CARBON_DIOXIDE:");
        Serial.print(gasppm(_pin, _type, mq135gases[0]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("CARBON_MONOXIDE:");
        Serial.print(gasppm(_pin, _type, mq135gases[1]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("3/4AE:");
        Serial.print(gasppm(_pin, _type, mq135gases[2]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("AMMONIUM:");
        Serial.print(gasppm(_pin, _type, mq135gases[3]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("1/4:");
        Serial.print(gasppm(_pin, _type, mq135gases[4]));
        Serial.print( "ppm" );
        Serial.print("    ");
        Serial.print("NAME:");
        Serial.print(gasppm(_pin, _type, mq135gases[5]));
        Serial.print( "ppm" );
        Serial.print("\n");}
        break;*/
    }
  }


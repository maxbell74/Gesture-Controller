





typedef enum
{
  P_RESET,
  P_IN_PROGRESS,
  P_ABORTED,
  P_FOUND,
  P_NOTIFIED
}
eSEARCH_STAT;


typedef struct
{
  float         old_q0;
  float         old_q1;
  eSEARCH_STAT  q0Stat;
  eSEARCH_STAT  q1Stat;
  uint32_t      count;
  
  eSEARCH_STAT  vStat;
  float old_pitch;
}
VertSwipeStruct;


typedef struct
{
  uint32_t      count;
  
  eSEARCH_STAT  tStat;
  float old_pitch;
}
TapStruct;


typedef struct
{
  float         old_q;
  float         old_q0;
  float         old_pitch;
  eSEARCH_STAT  qStat;
  eSEARCH_STAT  q1Stat;
  uint8_t       preZeroResetCondition;
  uint32_t      count;
  float         zero_q0;
  float         zero_q;
  
  float old_yaw;
  float zero_yaw;
  float old_roll;
}
OrizSwipeStruct;


/*------------------------------------------- global variables -----------------------------------------*/

VertSwipeStruct gVertSwipe = {1, 0, P_RESET, P_RESET,0, P_RESET, 0};
OrizSwipeStruct gOrizSwipe = {1, 0, 0, P_RESET, P_RESET, 0, 0, 0, 0, 0, 0};
TapStruct        gTap       = {0, P_RESET, 0}; 

float roll, pitch, yaw;    //Euler angles in degree











/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
uint8_t checkPattern_VertSwipe(float roll_v, float pitch_v, float yaw_v)
{

  if( (inWindow(pitch_v, 0, 20)) && inWindow(roll_v, -90, 25) )
  {
    //orizz position
    gVertSwipe.vStat = P_RESET;
    gVertSwipe.old_pitch = pitch_v;
  }
  else
  {
    if( (gVertSwipe.vStat == P_RESET) || (gVertSwipe.vStat == P_IN_PROGRESS) )
    {
      if(pitch_v > gVertSwipe.old_pitch)
      {
        gVertSwipe.old_pitch = pitch_v;       
        gVertSwipe.vStat = P_IN_PROGRESS;       
        if( pitch_v > 40)
        {
          gVertSwipe.vStat = P_FOUND;       
        }
      }
      else
        gVertSwipe.vStat = P_ABORTED;
    }
  }
  
  //
  if(gVertSwipe.vStat == P_FOUND)
  {
    gVertSwipe.vStat = P_NOTIFIED;
    return 1;
  }
  else
    return 0;
}



/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
uint8_t checkPattern_Tap(float roll_t, float pitch_t, float yaw_t)
{
  
  if( (inWindow(pitch_t, -70, 15)) /*&& inWindow(roll_t, -90, 20)*/ )
  {
    //orizz position
    gTap.tStat = P_RESET;
    gTap.old_pitch = pitch_t;
  }
  else
  {
    if( (gTap.tStat == P_RESET) || (gTap.tStat == P_IN_PROGRESS) )
    {
      if(pitch_t > gTap.old_pitch)
      {
        gTap.old_pitch = pitch_t;       
        gTap.tStat = P_IN_PROGRESS;       
        if( pitch_t > -15)
        {
          gTap.tStat = P_FOUND;       
        }
      }
      else
        gTap.tStat = P_ABORTED;
    }
  }
  
  //
  if(gTap.tStat == P_FOUND)
  {
    gTap.tStat = P_NOTIFIED;
    return 1;
  }
  else
    return 0;  
}




/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
uint8_t checkPattern_OrizSwipe(float roll_o, float pitch_o, float yaw_o)
{
  float delta_o;
  
  if( (inWindow(pitch_o, 0, 20)) && inWindow(roll_o, -90, 25) )
  {
    //orizz position
    gOrizSwipe.preZeroResetCondition = 1;
    gOrizSwipe.old_roll = roll_o;
  }
  else if(gOrizSwipe.preZeroResetCondition == 1) 
  {
//    if( (inWindow(pitch_o, 0, 20)) && (roll_o < gOrizSwipe.old_roll) )
//    {
//      gOrizSwipe.old_roll = roll_o;
//      if(roll_o < -150)
//      {
//        //zero position found
//        gOrizSwipe.preZeroResetCondition = 2;
//        gOrizSwipe.zero_yaw = yaw_o;
//      }
//    }
    
    
    if( (inWindow(pitch_o, 0, 20)) && (angleUnroll(roll_o) < angleUnroll(gOrizSwipe.old_roll)) )
    {
      gOrizSwipe.old_roll = roll_o;
      if(angleUnroll(roll_o) < 210)
      {
        //zero position found
        gOrizSwipe.preZeroResetCondition = 2;
        gOrizSwipe.zero_yaw = yaw_o;
      }
    }
    
//    if( (inWindow(pitch_o, 0, 40)) && (roll_o < -155) )
//    {
//      //zero position found
//      gOrizSwipe.preZeroResetCondition = 2;
//      gOrizSwipe.zero_yaw = yaw_o;
//    }
  }
  else if(gOrizSwipe.preZeroResetCondition == 2) 
  {
    if( inWindow(yaw_o, gOrizSwipe.zero_yaw, 20) )
    {
      gOrizSwipe.qStat = P_RESET;
      gOrizSwipe.old_yaw = gOrizSwipe.zero_yaw;
    }
    else
    {
      if( (gOrizSwipe.qStat == P_RESET) || (gOrizSwipe.qStat == P_IN_PROGRESS) )
      {
        if(angleUnroll(yaw_o) < angleUnroll(gOrizSwipe.old_yaw))
        {
          gOrizSwipe.old_yaw = yaw_o;       
          gOrizSwipe.qStat = P_IN_PROGRESS;     
          delta_o = fabs(angleUnroll(gOrizSwipe.zero_yaw) - angleUnroll(yaw_o));
          if(  delta_o > 40 )
          {
            gOrizSwipe.qStat = P_FOUND;       
          }
        }
        else
          gOrizSwipe.qStat = P_ABORTED;
      }
    }
  }
  
  ///
  if(gOrizSwipe.qStat == P_FOUND)
  {
    gOrizSwipe.qStat = P_NOTIFIED;
    gOrizSwipe.qStat = P_NOTIFIED;
    return 1;
  }
  else
    return 0;
  
}




/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
void eulerAngles(float q0, float q1, float q2, float q3)
{  
  float t0, t1, t2, t3, t4, ysqr;  
  
        
  //euler angles
  ysqr = q2*q2;
  
  //roll (x-axis rotation)
  t0 = 2 * (q0*q1 + q2*q3);
  t1 = 1 - 2 * (q1*q1 + ysqr);
  roll = atan2(t1, t0);
  
  //pitch (y-axis rotation)
  t2 = 2 * (q0*q2 - q3*q1);
  if(t2 > 1) 
    t2 = 1; 
  if(t2 < -1) 
    t2 = -1;
  pitch = asin(t2);
  
  //yaw (z-axis rotation)
  t3 = 2 * (q0*q3 + q1*q2);
  t4 = 1 - 2 * (ysqr + q3*q3);
  yaw = atan2(t4, t3);
  

  //convert to degree
  roll = roll * 360/6.28;
  pitch = pitch * 360/6.28;
  yaw = yaw * 360/6.28;
}




/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
static uint8_t inWindow(float val, float centerWindow, float halfWindow)
{
  if( (val < (centerWindow + halfWindow)) && (val > (centerWindow - halfWindow)) )
    return 1;
  else
    return 0;
}



/*-------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------*/
static float angleUnroll(float a)
{
  //float b;
  
  //b = (a + 360) % 360;
  

  if(a >= 0)
    return a;
  else
    return a  + 360;

}




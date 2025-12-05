int16 Min_2_Steps(INT16 min)
{
   RETURN min * 34 + 1000;
}

int16 hr_2_Steps(INT16 hr)
{
   RETURN hr * 170 + 1000;
}

int16 Steps_2_min(INT16 steps)
{
   If (steps >= 3040)
   {
   Return 0;
   }
   Else
   
   RETURN (steps-1000)/34;
}

int16 Steps_2_hr(INT16 steps)
{
   If (Steps < 1170)
   {
   Return 12;
   }
   Else 
   
   Return (steps-1000)/170;
   
}
   
int16 hr_min_2_Steps(INT16 min)
{
   min = min / 6;
   RETURN min * 17;
}

int16 hours(int16 hours, int16 min)
{
return (hr_2_Steps (hours) + hr_min_2_Steps (min));
}


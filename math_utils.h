float minmax(float value, float upper, float lower){

  if(upper<lower){
    float temp = lower;
    lower = upper;
    upper = lower;
  }
  if(value > upper) return upper;
  else if (value < lower) return lower;
  else return value;
}

//Basically it is a function to calculate the difference between 2 headings. Because the differents between 355 and 5 is not -250 but +10
float heading_add(float angle, float offset=0){
  angle += offset;
  if(angle>=360) angle -=360;
  if(angle<0) angle +=360;   
  return angle;
}

//get the difference between 2 angles in degrees.
//arduino only supports integer with the mod  % function.
float headingDiff(float heading1, float heading2){
    float diff = heading1 - heading2;
     if (diff >  180) diff -= 360;
     if (diff < -180) diff += 360;
     return diff;
}

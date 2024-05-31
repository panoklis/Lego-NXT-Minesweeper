#include "aclock.h"

void AclockDisplayFrame(UBYTE cx, UBYTE cy, UBYTE r){
	
	double x_c;
	for (x_c = -r; x_c <= r; x_c += 0.005){
		//radius :: r
		DisplaySetPixel( round(x_c) + cx,round(sqrt((r*r) - (x_c * x_c) )) + cy );
		DisplaySetPixel( round(x_c) + cx, -(round(sqrt((r*r) - (x_c * x_c) ))) + cy );
	}
	/*for (x_c = -(r-1); x_c <= (r-1); x_c += 0.005){
		//radius :: r-1
		DisplaySetPixel( round(x_c) + cx,round(sqrt((r-1)*(r-1) - (x_c * x_c) )) + cy );
		DisplaySetPixel( round(x_c) + cx, -(round(sqrt((r-1)*(r-1) - (x_c * x_c) ))) + cy );
	}*/
	DisplaySetPixel(cx,cy);
}

void AclockDisplayFrameSymbol(UBYTE cx, UBYTE cy, UBYTE r, UWORD hh){
   hh=hh%360;
      if(hh==0||hh==180){
        DisplayLineY(cx,cy-r,cy-0.8*r);
        DisplayLineY(cx,cy+0.8*r,cy+r);
        return;
      }
  for(int i = 0 ; i<DISPLAY_WIDTH;i++){
    //for(int j = 0 ; j<DISPLAY_HEIGHT;j++){
      int j= abs((tan((90+hh)*3.14/180)*(i-cx))+cy);
      //if(abs((-1)*(j-cy)+(tan((90+hh)*3.14/180)*(i-cx)))==0){//90+hh cause 90 - - hh second - is cause fliped angle and first cause we start from 12 o clock instead of 3
        double distanceFromCenter = sqrt( pow((cx - i),2) + pow((cy - j),2) );
        if(distanceFromCenter >= 0.8*r&&distanceFromCenter <= 1*r){
          //if(((j<=cy)&&(hh>270||hh<90))||((j>=cy)&&(hh>90||hh<270))){DisplaySetPixel(i, j);}
          DisplaySetPixel(i, j);
          DisplaySetPixel(i, -j);
        }
      //}

      //}
      }
}

void AclockDisplayHand(UBYTE cx, UBYTE cy, UBYTE r, UWORD hh, double thickness , double height){
	double line = tan(((hh+90)%360)*3.141592/180);
	double a,b;
	a=-r;
	b=r;
	if ((hh%360) == 0) {DisplayLineY(cx,cy,cy-r*height); return;}
	else if ((hh%360) == 180) {DisplayLineY(cx,cy,cy+r*height);return;}
	else if ((hh%360) == 90) {DisplayLineX(cx,cx+r*height,cy);return;}
	else if ((hh%360) == 270) {DisplayLineX(cx,cx-r*height,cy);return;}
	else if((hh%360)>0 && (hh%360)<180) {a=0;}
	else if((hh%360)>180 && (hh%360)<360) {b=0;}
	for(double t_i= -thickness/2; t_i<=thickness/2; t_i+=0.5){
		for(double x_i = a; x_i <= b; x_i += 0.2){
			double y_i = line * x_i + t_i;
			if( sqrt( pow(x_i,2) + pow(y_i,2) ) <= height*r ){
				DisplaySetPixel( round(x_i) + cx,round(y_i) + cy );
			}
		}
	}
}
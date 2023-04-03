#include <allegro.h>

#define MAX_X_RES 1024  
#define MAX_Y_RES 768
//#define RISOLUZIONE 3

#define PI   3.141592654

#define PRECALCOLI 0
#define PRECALC_MAX_RES 7200 /* 360 * 4 */

double RISOLUZIONE = 1;

double SIN_VET[PRECALC_MAX_RES];
double COS_VET[PRECALC_MAX_RES];

int _ZOOM = 5;    // Fattore di zoom
int FUNZIONE = 1; // Numero della funzione da plottare
double XO,YO;     //Coordinate dell'origine


void init_precalc_trigon(){
int i;
    for(i=0;i<=PRECALC_MAX_RES;i++){
       SIN_VET[i] = sin(i/(PRECALC_MAX_RES / 360));
       COS_VET[i] = cos(i/(PRECALC_MAX_RES / 360));
    }
}


double COS (double angolo){
  int _COS;
  
  if (PRECALCOLI == 1) {  
     _COS = (int)((angolo * PRECALC_MAX_RES) / 360);
     return COS_VET[_COS];
  } else return cos(angolo);
  
}


double SIN (double angolo){
  int _sin;
  
  if (PRECALCOLI == 1) {  
     _sin = (int)((angolo * PRECALC_MAX_RES) / 360) - (PRECALC_MAX_RES/2);
     //_sin = angolo - ((angolo*angolo*angolo)/6) + ((angolo*angolo*angolo*angolo*angolo)/120); 
     return SIN_VET[_sin];
  } else return sin(angolo);
  
}

double zeta(double x, double y, int *colore)
{
 
 double _zeta;      
 double x1,y1,tmp;

 switch (FUNZIONE){
   case 1:
        x1 = x*x;
        y1 = y*y;
        tmp = sqrt(x1+y1);
       _zeta = 25*(1.1*sin(tmp)+0.1)/(0.3*tmp+0.1);// SOMBRERO
        break;
   case 2:
        _zeta =.02*x*x-.02*y*y;  // Sella        
        break;
   case 3:
        _zeta = .05*x*x+.05*y*y;  // Paraboloide     
        break;
   case 4:
        _zeta = 3*sqrt(abs(3*x)); // Radice Quadrata
        break;
   case 5:
        _zeta = 10*tan(.15*y);    // Tangente        
        break;
   case 6:
        x1 = x*x;
        y1 = y*y;
        tmp = sqrt(x1+y1);
       _zeta = 25*(sin(tmp)+0.1)/(tmp+0.1);// SOMBRERO

        //_zeta = SIN(-(sqrt(abs(3*x))+sqrt(abs(3*y))));        
        break;
   case 8:
        _zeta = log(1+x*x*y*y);
        break;
   case 9:
        _zeta = exp(abs(SIN(x)-SIN(y)));
        break;
   case 10:
       _zeta = SIN(x)+abs(y);
        break;
   case 11:
      _zeta = 10 * sin(0.08 * x*y);
        break;
 }
 
 if ((x>0)&&(y>0)) *colore = makecol(255,255,50);
 if ((x>0)&&(y<0)) *colore = makecol(50,255,255);
 if ((x<0)&&(y>0)) *colore = makecol(155,255,50);
 if ((x<0)&&(y<0)) *colore = makecol(50,155,255);
 if (_zeta<0) *colore = makecol(255,255,255);

 return _zeta;
}


/* Converte in 2D un punto 3D */
void converti_punto(double x, double y, double z, \
                    double *xp2D, double *yp2D, \
                    double sinX, double cosX, \
                    double sinY, double cosY, \
                    double sinZ, double cosZ) { 

  double xp,yp;

  xp=(x * cosX + XO) + (y * cosY) + (z * cosZ);
  yp=(YO - x * sinX) - (y * sinY) - (z * sinZ);
 
  *xp2D = xp;
  *yp2D = yp;
}


/* Disegna i tre assi cartesiani */
void plot_axes(BITMAP *bmp, double angleX, double angleY, double angleZ){
     int r = 350;
     double x,y;
     
     /* Asse y */
     x=r*COS(angleY);
     y=r*SIN(angleY);
     fastline(bmp,XO,YO,XO+x,YO-y,makecol32(255,255,255));
     fastline(bmp,XO,YO,XO-x,YO+y,makecol32(255,255,255));

     /* Asse x */
     x=r*COS(angleX);
     y=r*SIN(angleX);
     fastline(bmp,XO,YO,XO+x,YO-y,makecol32(255,0,0));
     fastline(bmp,XO,YO,XO-x,YO+y,makecol32(255,0,0));
     
     /* Asse z */
     x=r*COS(angleZ);
     y=r*SIN(angleZ);
     fastline(bmp,XO,YO,XO+x,YO-y,makecol32(0,255,0));
}

/* Inizializza la grafica e altre funzioni accessiorie*/
void init_graphic() {
	int depth, res;
	allegro_init();
	depth = desktop_color_depth();
	if (depth == 0) depth = 32;
	set_color_depth(depth);

    //res = set_gfx_mode(GFX_AUTODETECT, MAX_X_RES, MAX_Y_RES, 0, 0);
	res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, MAX_X_RES, MAX_Y_RES, 0, 0);
	if (res != 0) {
		allegro_message(allegro_error);
		exit(-1);
	}
	
//	GFX_AUTODETECT
//	GFX_AUTODETECT_WINDOWED
	
	install_keyboard();
   
   /* Detect mouse presence */
   if (install_mouse() < 0) {
      textout_centre_ex(screen, font, "No mouse detected, but you need one!",
			SCREEN_W/2, SCREEN_H/2, makecol(0, 0, 0),
			makecol(255, 255, 255));
      readkey();
      exit(-1);
   }
}


/* Indica se un punto si trova nell'area dello schermo o è fuori */
char plottabile(int x,int y){
     if ((x<-100) || (x>MAX_X_RES + 100))
        return 0; //False
        
     if ((y<-100) || (y>MAX_Y_RES + 100))
        return 0; //False

     return 1; //True     
}


/* Calcola e visualizza nella bitmap la funzione 3D */
void plot_3D (BITMAP *bmp, double X_Angle, double Y_Angle, float Z_Angle, char display_pixel, char display_line)
{
 double i,j;
 double xp2D,yp2D,xp2D1,yp2D1,xp2D2,yp2D2;    
 
 double sinX,cosX,sinY,cosY,sinZ,cosZ;
 
 int colore;

 sinX = sin(X_Angle) * (double)_ZOOM;
 cosX = cos(X_Angle) * (double)_ZOOM;
 sinY = sin(Y_Angle) * (double)_ZOOM;
 cosY = cos(Y_Angle) * (double)_ZOOM;
 sinZ = sin(Z_Angle) * (double)_ZOOM;
 cosZ = cos(Z_Angle) * (double)_ZOOM;

     
 for (i=-30;i<30;i=i+RISOLUZIONE*2)
     for(j=-30;j<30;j=j+(RISOLUZIONE*2))

     {
      converti_punto(i,j+RISOLUZIONE,zeta(i,j+RISOLUZIONE,&colore),&xp2D1,&yp2D1,sinX,cosX,sinY,cosY,sinZ,cosZ);
      converti_punto(i,j+(2*RISOLUZIONE),zeta(i,j+(2*RISOLUZIONE),&colore),&xp2D2,&yp2D2,sinX,cosX,sinY,cosY,sinZ,cosZ);
      converti_punto(i,j,zeta(i,j,&colore),&xp2D,&yp2D,sinX,cosX,sinY,cosY,sinZ,cosZ);

      if (plottabile(xp2D1, yp2D1) && plottabile(xp2D2, yp2D2) && plottabile(xp2D, yp2D)) {

         if (display_line) {
            fastline(bmp,xp2D1, yp2D1,xp2D2, yp2D2,colore);
            fastline(bmp,xp2D, yp2D,xp2D1, yp2D1,colore);
         }

         if (display_pixel) {
            putpixel(bmp, xp2D, yp2D,makecol32(255,0,0));
            putpixel(bmp, xp2D1, yp2D1,makecol32(255,0,0));
            putpixel(bmp, xp2D2, yp2D2,makecol32(255,0,0));
         }
      }
 }
    
 for (i=-30;i<30;i=i+RISOLUZIONE*2)
     for(j=-30;j<30;j=j+(RISOLUZIONE*2))

     {
      converti_punto(j+RISOLUZIONE,i,zeta(j+RISOLUZIONE,i,&colore),&xp2D1,&yp2D1,sinX,cosX,sinY,cosY,sinZ,cosZ);
      converti_punto(j+(2*RISOLUZIONE),i,zeta(j+(2*RISOLUZIONE),i,&colore),&xp2D2,&yp2D2,sinX,cosX,sinY,cosY,sinZ,cosZ);
      converti_punto(j,i,zeta(j,i,&colore),&xp2D,&yp2D,sinX,cosX,sinY,cosY,sinZ,cosZ);

      if (plottabile(xp2D1, yp2D1) && plottabile(xp2D2, yp2D2) && plottabile(xp2D, yp2D)) {

         if (display_line) {
            fastline(bmp,xp2D1, yp2D1,xp2D2, yp2D2,colore);
            fastline(bmp,xp2D, yp2D,xp2D1, yp2D1,colore);
         }

         if (display_pixel) {
            putpixel(bmp, xp2D, yp2D,makecol32(255,0,0));
            putpixel(bmp, xp2D1, yp2D1,makecol32(255,0,0));
            putpixel(bmp, xp2D2, yp2D2,makecol32(255,0,0));
         }
      }
 }
    
}



void OSD (BITMAP *bmp, double X_Angle, double Y_Angle, double Z_Angle) {

double tmp;

   tmp = (double)180/PI;    

   textprintf_ex(bmp, font, 350,  10, makecol(255, 255, 255), -1, "TEST - 3D");
   textprintf_ex(bmp, font, 290,  550, makecol(255, 255, 255), -1, "Copyright CAPORALE SIMONE(C)");

   textprintf_ex(bmp, font, 10,  10, makecol(255, 255, 255), -1, "Angolo X = %.1lf",(X_Angle * tmp));
   textprintf_ex(bmp, font, 10,  20, makecol(255, 255, 255), -1, "Angolo Y = %.1lf",(Y_Angle * tmp));
   textprintf_ex(bmp, font, 10,  30, makecol(255, 255, 255), -1, "Angolo Z = %.1lf",(Z_Angle * tmp));
   textprintf_ex(bmp, font, 10,  40, makecol(255, 255, 255), -1, "Numero punti = %.2lf",RISOLUZIONE);
   textprintf_ex(bmp, font, 10,  50, makecol(255, 255, 255), -1, "Zoom = %d",_ZOOM);

   textprintf_ex(bmp, font, 10,  60, makecol(255, 255, 255), -1, "MOUSE DX / MOUSE SX");
   textprintf_ex(bmp, font, 10,  70, makecol(255, 255, 255), -1, "PG_UP / PG_DOW");
   textprintf_ex(bmp, font, 10,  80, makecol(255, 255, 255), -1, "HOME = Resetta tutto");
   textprintf_ex(bmp, font, 10,  90, makecol(255, 255, 255), -1, "KEYPAD +/- = Zoom");
   textprintf_ex(bmp, font, 10, 100, makecol(255, 255, 255), -1, "F = Cambia Funzione");
   textprintf_ex(bmp, font, 10, 110, makecol(255, 255, 255), -1, "INS = Ruota");
   
   textprintf_ex(bmp, font, 10, 120, makecol(255, 255, 255), -1, "r / R = Cambia il numero di punti");
}


int main() {

    BITMAP *bmp;
 
    double X_Angle ;
    double Y_Angle ;
    double Z_Angle ;

    char update = 1;
    char display_pixel = 1;
    char display_line = 1;


    init_graphic();
    init_precalc_trigon();

    bmp = create_bitmap_ex(32,MAX_X_RES,MAX_Y_RES);
    if (!bmp) {
       allegro_message(allegro_error);
       exit(-1);
    }
          
    enable_hardware_cursor();
    select_mouse_cursor(MOUSE_CURSOR_ALLEGRO);
    show_mouse(screen);
    
    XO = MAX_X_RES/2;
    YO = MAX_Y_RES/2;

    X_Angle = 05 * PI / 04;
    Y_Angle = 07 * PI / 04;
    Z_Angle = 01 * PI / 02;
    
    
    while (!key[KEY_ESC]) {

      if (mouse_b & 1) {
         X_Angle += PI/200;
         Y_Angle += PI/200;
         Z_Angle -= PI/200;         
         update = 1;
      }
 
      if (mouse_b & 2)  {
         X_Angle -= PI/200;
         Y_Angle -= PI/200;
         Z_Angle += PI/200;         
         update = 1;                          
      }

      if (key[KEY_PGUP]) {
         X_Angle += PI/200;
         Z_Angle += PI/(200*2);  
         Y_Angle += PI/200;       
         update = 1;                           
      }

      if (key[KEY_PGDN]) {
         X_Angle -= PI/200;
         Z_Angle -= PI/(200*2);         
         update = 1;
      }
      
      if (key[KEY_HOME]) {
         X_Angle = 05 * PI / 04;
         Y_Angle = 07 * PI / 04;
         Z_Angle = 01 * PI / 02;   
         XO = MAX_X_RES/2;
         YO = MAX_Y_RES/2;    
         _ZOOM = 5;
         RISOLUZIONE = 1;
         display_line = 1;
         display_pixel = 0;
         update = 1;
      }
      
      if (key[KEY_PLUS_PAD]) {
         _ZOOM ++;  
          //rest(1);                                                                             
         update = 1;
      }
      
      if (key[KEY_MINUS_PAD]) {
         _ZOOM --;                                                                               
         //rest(1);
         update = 1;
      }

      if (key[KEY_INSERT]) {
         X_Angle -= PI/200;
         Y_Angle -= PI/200;
         update = 1;
      }

      if (key[KEY_RIGHT]) {
         if (key_shifts & KB_SHIFT_FLAG) 
           XO = XO + _ZOOM;
         else 
           X_Angle -= PI/200;
         update = 1;                           
      }

      if (key[KEY_LEFT]) {
         if (key_shifts & KB_SHIFT_FLAG) 
           XO = XO - _ZOOM;
         else 
           X_Angle += PI/200;
         update = 1;                           
      }
      
      if (key[KEY_UP]) {
         if (key_shifts & KB_SHIFT_FLAG) 
           YO = YO - _ZOOM;
         else 
           Y_Angle -= PI/200;
         update = 1;
      }
      
      if (key[KEY_DOWN]) {
         if (key_shifts & KB_SHIFT_FLAG) 
           YO = YO + _ZOOM;
         else 
           Y_Angle += PI/200;
         update = 1;
      }
      
      if (key[KEY_Z]) {
         if (key_shifts & KB_SHIFT_FLAG) 
           Z_Angle += PI/200;
         else 
           Z_Angle -= PI/200;
         update = 1;                           
      }
      
      if (key[KEY_R]) {
         if (key_shifts & KB_SHIFT_FLAG) {
           RISOLUZIONE += 0.01;
           if (RISOLUZIONE > 25) RISOLUZIONE = 25;
         }  
         else {
           RISOLUZIONE -= 0.01;
           if (RISOLUZIONE < 0.02) RISOLUZIONE = 0.02;
         }  
         update = 1;                           
      }

      if (key[KEY_P]) {
         if (key_shifts & KB_SHIFT_FLAG) 
            display_pixel = 0;
         else 
            display_pixel = 1;
         update = 1;                           
      }
      
      if (key[KEY_L]) {
         if (key_shifts & KB_SHIFT_FLAG) 
            display_line = 0;
         else 
            display_line = 1;
         update = 1;                           
      }

      if (key[KEY_F]) {
         FUNZIONE++;
         if (FUNZIONE > 11)
            FUNZIONE = 1;

         X_Angle = 05 * PI / 04;
         Y_Angle = 07 * PI / 04;
         Z_Angle = 01 * PI / 02;       
         _ZOOM = 5;
         update = 1;
         rest(100);
      }


      if (update == 1) {

           clear_to_color(bmp,makecol32(0,0,0));                                    

          plot_3D(bmp,X_Angle, Y_Angle, Z_Angle,display_pixel,display_line);
          plot_axes(bmp,X_Angle, Y_Angle, Z_Angle);

          OSD(bmp,X_Angle, Y_Angle, Z_Angle);
          
          blit(bmp,screen,0,0,0,0,MAX_X_RES,MAX_Y_RES);  
          
          if(X_Angle > 2*PI) X_Angle=0;
          if(X_Angle < 0)    X_Angle=2*PI;
          if(Y_Angle > 2*PI) Y_Angle=0;
          if(Y_Angle < 0)    Y_Angle=2*PI;
          if(Z_Angle > 2*PI) Z_Angle=0;
          if(Z_Angle < 0)    Z_Angle=2*PI;
          
          update = 0;
      }

    }

    destroy_bitmap(bmp);  /* Distrugge la bitmap */
  	clear_keybuf();
    disable_hardware_cursor(); /* Disattiva il mouse */

	return 0;
}
END_OF_MAIN()

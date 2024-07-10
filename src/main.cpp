#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

#define LV_DEG_TO_ANGLE(x) ((x) * 10)

/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   9600-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
HardwareSerial ss(2);

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

lv_obj_t* ui_compass;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 1
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

// Global variables to hold the compass reading string and direction
int compassReading;
char compassReadingStr[6]; // Need extra space for the degree symbol and space
const char* direction;

// Function to update compass position based on the compass reading
void updateCompassPosition(lv_timer_t *timer) {

    // Generate a random compass reading (0 to 359 degrees)
    // compassReading = random(0, 360);
    bool gpsCourseValid = gps.course.isValid();

    // Serial.print("GPS course valid: ");
    // Serial.println(gpsCourseValid);
    // Increment the compass reading by a small amount
    if (gpsCourseValid){
        compassReading =  gps.course.deg();
        // Ensure that compassReading stays within the range of 0 to 359 degrees
        if (compassReading >= 360) {
            compassReading -= 360;
        }
        // Calculate the new X position based on the compass reading
        int newPos = 0 - (compassReading / 5) * 10; // Adjusted based on new zero location and width
        int nextPos = 0 - ((compassReading + 5) / 5) * 10; // Adjusted based on new zero location and width
        int newXPos = newPos + ((nextPos - newPos) * (compassReading % 5) / 5);

        // Convert the compass reading to a string
        sprintf(compassReadingStr, "%d°", compassReading); // Add degree symbol and space

        // TODO This could make the delay between the label and the compass different
        // Serial.print("New X position: ");
        // Serial.println(newXPos);

        // Smoothly animate the transition to the new X position
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, ui_compass);
        lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
        // Serial.print("Current X position: ");
        // Serial.println(lv_obj_get_x(ui_compass));
        lv_anim_set_values(&anim, lv_obj_get_x(ui_compass), newXPos);
        // if (compassReading == 0) {
        //     lv_anim_set_time(&anim, 0); // Animation duration: 1000 milliseconds
        // } else {
        //     lv_anim_set_time(&anim, 500); // Animation duration: 1000 milliseconds
        // }
        lv_anim_set_time(&anim, 10); // Animation duration: 1000 milliseconds
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out); //  lv_anim_path_linear // lv_anim_path_overshoot // lv_anim_path_bounce // lv_anim_path_ease_in_out
        lv_anim_start(&anim);
    } else {
        // compassReading = "***.**°";
        sprintf(compassReadingStr, "**°");
    }




    // Update the label to display the compass reading
    lv_label_set_text(ui_lbl_degree, compassReadingStr);

    // Determine the direction based on the compass reading
    const char* directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int sector = (compassReading + 22) / 45;
    const char* direction = directions[sector % 8];

    // Update the label to display the direction
    lv_label_set_text(ui_lbl_direction, direction);

    // Serial.print("Compass reading: ");
    // Serial.println(compassReading);

    bool gpsSpeedValid = gps.speed.isValid();

    char speedStr[6];

    if (gpsSpeedValid){
        // Serial.print("Speed: ");
        // Serial.println(gps.speed.knots());
        sprintf(speedStr, "%.1f", gps.speed.knots());
        lv_label_set_text(ui_lbl_speed, speedStr);
    } else {
        // sprintf(speedStr, "**.**");
    }
}


void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 2 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register( &indev_drv );


    ui_init();

    Serial.println( "Setup done" );

    // Set up a timer to update the compass position every 5 seconds
    lv_timer_create(updateCompassPosition, 100, NULL);
}

// void loop()
// {
//     lv_timer_handler(); // let the GUI do its work
//     delay(5);

//     unsigned long startTime = millis(); // Get the current time

//     do {
//         // Non-blocking read of GPS data
//         if (ss.available() > 0) {
//             if (gps.encode(ss.read())) {
//             }
//         }
//     } while (millis() - startTime < 1000);

//     if (millis() > 5000 && gps.charsProcessed() < 10)
//         Serial.println(F("No GPS data received: check wiring"));

//     Serial.print("Course: ");
//     Serial.println(gps.course.deg());
// }

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}


void loop()
{
    lv_timer_handler(); // let the GUI do its work
    delay(5);

    Serial.print("Speed: ");
    Serial.println(gps.speed.knots());
    // Serial.print("Course: ");
    // Serial.println(gps.course.deg());
    // printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
    // Serial.println();
//  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

//   printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
//   printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
//   printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
//   printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
//   printInt(gps.location.age(), gps.location.isValid(), 5);
//   printDateTime(gps.date, gps.time);
//   printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
//   printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
//   printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
//   printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

//   unsigned long distanceKmToLondon =
//     (unsigned long)TinyGPSPlus::distanceBetween(
//       gps.location.lat(),
//       gps.location.lng(),
//       LONDON_LAT, 
//       LONDON_LON) / 1000;
//   printInt(distanceKmToLondon, gps.location.isValid(), 9);

//   double courseToLondon =
//     TinyGPSPlus::courseTo(
//       gps.location.lat(),
//       gps.location.lng(),
//       LONDON_LAT, 
//       LONDON_LON);

//   printFloat(courseToLondon, gps.location.isValid(), 7, 2);

//   const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

//   printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

//   printInt(gps.charsProcessed(), true, 6);
//   printInt(gps.sentencesWithFix(), true, 10);
//   printInt(gps.failedChecksum(), true, 9);
//   Serial.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}
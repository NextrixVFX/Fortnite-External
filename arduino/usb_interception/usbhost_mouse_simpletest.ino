#include "Adafruit_TinyUSB.h"
#include <pio_usb.h>
#include "class/hid/hid.h"
#include "tusb.h"
#include "lib/usbh_helper.h"

// Create USB HID Mouse device to send reports to PC
Adafruit_USBD_HID usb_hid;

// Structure for mouse report
typedef struct {
  uint8_t buttons; // bits: 0-left,1-right,2-middle,3-forward,4-back
  int8_t x;
  int8_t y;
  int8_t wheel; // scroll up/down
  int8_t pan;   // horizontal scroll
} HID_MouseReport;

HID_MouseReport mouseReport = {0, 0, 0, 0, 0};

#define MOUSE_SENSITIVITY 4.0f

// Use built-in TinyUSB mouse descriptor
static const uint8_t hid_report_descriptor[] = {
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(1))
};

void setup() {
  Serial.begin(9600);

  // Configure PIO USB host (for input mouse)
  rp2040_configure_pio_usb();
  USBHost.begin(1);

  // Setup HID device to act as mouse output
  usb_hid.setReportDescriptor(hid_report_descriptor, sizeof(hid_report_descriptor));
  usb_hid.begin();
  usb_hid.enableOutEndpoint(false); // we don't need input from PC

  delay(3000);
  Serial.println("Ready.");
}

void loop() {
  USBHost.task();

  // Only send report if something changed
  if(mouseReport.x != 0 || mouseReport.y != 0 || mouseReport.wheel != 0 || mouseReport.buttons != 0) {
    usb_hid.sendReport(1, &mouseReport, sizeof(mouseReport));
  }

  // Reset deltas after sending to prevent drift
  mouseReport.x = 0;
  mouseReport.y = 0;
  mouseReport.wheel = 0;
  mouseReport.pan = 0;

  delay(1); // faster updates
}

//--------------------------------------------------------------------+
// HID Host Callback Functions (receive mouse input)
//--------------------------------------------------------------------+
extern "C" {

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const*, uint16_t) {
  tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
  if(len >= 3) {
    int8_t dx = (int8_t)report[1];
    int8_t dy = (int8_t)report[2];

    // Apply sensitivity
    dx = (int8_t)constrain((int)(dx * MOUSE_SENSITIVITY), -127, 127);
    dy = (int8_t)constrain((int)(dy * MOUSE_SENSITIVITY), -127, 127);

    if (Serial.available())
    {
      String s = Serial.readStringUntil('\n');
      s.trim();
      s.toLowerCase();

      int got_dx, got_dy;
      got_dx = 0; got_dy = 0;

      if (s.startsWith("move"))
      {
        int firstSpace = s.indexOf(' ');
        int secondSpace = s.indexOf(' ', firstSpace + 1);
        
        if (firstSpace > 0 && secondSpace > firstSpace)
        {
          got_dx = s.substring(firstSpace + 1, secondSpace).toInt();
          got_dy = s.substring(secondSpace + 1).toInt();
          
        } 
      }

      dx = (int8_t)constrain((int)(dx + got_dx), -127, 127);

      dy = (int8_t)constrain((int)(dy + got_dy), -127, 127);
    }
    

    // Map buttons:
    // report[0]: standard buttons
    // Bit 0 = left, 1 = right, 2 = middle
    // Bit 3 = forward (X1), Bit 4 = back (X2)
    mouseReport.buttons = report[0] & 0x1F; // take 5 lowest bits

    mouseReport.x = dx;
    mouseReport.y = dy;

    // Scroll wheel
    if(len > 3) mouseReport.wheel = (int8_t)report[3]; // some mice send wheel at byte3
    else mouseReport.wheel = 0;

    // Optional: horizontal pan (if your mouse sends it)
    if(len > 4) mouseReport.pan = (int8_t)report[4];
    else mouseReport.pan = 0;
  }

  tuh_hid_receive_report(dev_addr, instance);
}

} // extern "C"

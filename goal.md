# HX3-WiFi-MenuPanel

### Approach for a web- and GUI-based Menu Panel with ESP32 and color TFT display for an electronic organ

### TODO:

 * Split Web page "panel.html" into three similar pages for upper (upper.html), lower (lower.html) manual and (new) pedal (pedal.html), all using style.css and the three-column design from panel.html, leaving room for additions.
 * upper.html keeps sliders (="drawbars"), switches, "Save to Preset" button/number input and buttons for "Upper Voice Presets".
 * upper.html should show current preset number (from hx3EditArray[268]) as defaul value.
 * lower.html keeps sliders (="drawbars") and buttons for "lower Voice Presets".
 * pedal.html gets 12 "Pedal Voice Presets" buttons, but has only 5 sliders: "16", "16H", "8", "8H" (brown) and "Sustain" (grey).
 * Instead of "HX3 User Panel", add links to "Upper Manual", "Lower Manual" and "Pedal" on each page, including index.html.
 * On each "Upper Manual", "Lower Manual" and "Pedal", create a button "Save Voice".
 * Add/modify handlers in server.h accordingly, replacing "server.on("/setpanel"..) with handlers for upper, lower and pedal pages. I will add functionality later.
 * If in doubt, ask. 
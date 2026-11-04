# HX3-WiFi-MenuPanel

### Approach for a web- and GUI-based Menu Panel with ESP32 and color TFT display for an electronic organ

### TODO (all done, see "Status" below):

 * Split Web page "panel.html" into three similar pages for upper (upper.html), lower (lower.html) manual and (new) pedal (pedal.html), all using style.css and the three-column design from panel.html, leaving room for additions.
 * upper.html keeps sliders (="drawbars"), switches, "Save to Preset" button/number input and buttons for "Upper Voice Presets".
 * upper.html should show current preset number (from hx3EditArray[268]) as defaul value.
 * lower.html keeps sliders (="drawbars") and buttons for "lower Voice Presets".
 * pedal.html gets 12 "Pedal Voice Presets" buttons, but has only 5 sliders: "16", "16H", "8", "8H" (brown) and "Sustain" (grey).
 * Instead of "HX3 User Panel", add links to "Upper Manual", "Lower Manual" and "Pedal" on each page, including index.html.
 * On each "Upper Manual", "Lower Manual" and "Pedal", create a button "Save Voice".
 * Add/modify handlers in server.h accordingly, replacing "server.on("/setpanel"..) with handlers for upper, lower and pedal pages. I will add functionality later.
 * If in doubt, ask. 

### Status

**Web pages (data/)**
 * All pages use the single stylesheet `style.css` (`style.css.gz` must be regenerated after every change, the server prefers the `.gz` file). `panel.css` and `panel.html` are gone.
 * `index.html` (with logo), `params.html`, `upper.html`, `lower.html`, `pedal.html` share the "panel" look: `.panel`, `.panel-header`, `.panel-section`, `.btn`, link bar `li.panel-links` (current page bold/unlinked).
 * `upper.html`: Vibrato switches (left), upper drawbars + presets + "Save Voice" (middle), percussion, vibrato mode rotary, "Save to Preset" + number input (right). The number input shows `hx3EditArray[268]`; "Save to Preset" saves the overall preset (`/savepreset`), "Save Voice" is separate.
 * `lower.html` / `pedal.html`: same three-column grid with empty left/right sections (fixed widths 126/290 px in CSS) so drawbar spacing is identical on all three pages. Empty sections are hidden on narrow screens.
 * Pedal drawbar indices in hx3EditArray: 72 (16), 73 (16H), 74 (8), 75 (8H), 67 (Sustain).
 * `params.html`: loading banner (`.wait-banner`) instead of changing the heading; error banner if loading fails. Param description lines in `params_N.txt` must not contain `;` (it is the field separator, a stray one breaks the JSON).
 * Pages poll `/get*_json?cached=1` every 500 ms and refresh when `rev` changes (paused while tab hidden or a slider is being dragged).

**Firmware (include/)**
 * `server.h`: routes `/upper.html`, `/lower.html`, `/pedal.html`; JSON `/getupper_json`, `/getlower_json`, `/getpedal_json` (`?cached=1` = no SPI read, returns last values plus `rev`); set handlers `/setupper`, `/setlower`, `/setpedal` (shared helper `panelSetValues()`, with index range checks); `/setpresetupr`, `/setpresetlwr`, `/setpresetped`; `/savepreset`; stubs `/saveupper`, `/savelower`, `/savepedal`.
 * `editArrayRev` (global_vars.h) is incremented when a full edit array arrives (`XRESP_EDIT_ARR`), in `setParamValue()` (single parameter changes from HX3/MIDI) and in `changeValue()` of the menu system, so the web pages follow changes made on the device.

**Open points / considerations**
 * `PEDAL_PRESET_IDX = 271` in server.h is a guess (analogous to 269 upper / 270 lower), marked TODO. Verify. - DONE!
 * "Save Voice" handlers for upper, lower and pedal are stubs, functionality still to be added.
 * Other local writers of `hx3EditArray` in menu_items.h (preset/voice/organ/speaker handlers, ~lines 500-560) do not bump `editArrayRev`. Probably not needed, as the HX3 answers with a full edit array (which does), but check if displayed values ever show up late. - OK
 * Polling interval (500 ms) is fine for now; could be slowed down, or polled faster only after a click, if several browsers are open. - OK
 * `data_raw/style.css` is an old copy of the previous style.css and was not updated.
 * Pages were tested on real hardware, all controls work.
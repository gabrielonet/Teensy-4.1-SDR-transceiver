# Teensy-4.1-SDR-transceiver<br>
Credits to ZL2CTM - author for ino code<br>
To change a direct conversion SDR into a classic superthet transceiver, I have to add a H-mode mixer feed    by a DDS and folowed by a X-tall filter @ 9 Mhz.<br>
Since audio gain is too huge and need to add -30 + 90 dB I.F. stage before SDR, Teensy audio shield gain has to be reduced, also +-45 degrees opamps gain have to be reduced also.<br>


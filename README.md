# Somfy Receiver compatible with Arduino

This project lets you decode Somfy remote control pulses without depending on a specific receiver (in my case, I used a CC1101 connected to an ESP32).

You will need to set up your receiver to detect pulses and calculate their duration, in order to send them to your ``SomfyReceiver`` instance via the ``pulse`` function.

See the example in ``examples/ESP32/ELECHOUSE_CC1101/ELECHOUSE_CC1101.ino``

## Credits

- [Excellent PushTrack article](https://pushstack.wordpress.com/somfy-rts-protocol/)
- [ESPSomfy-RTS project](https://github.com/rstrouse/ESPSomfy-RTS)
- [Somfy RTS receiver project](https://github.com/walterbrebels/somfy-rts)
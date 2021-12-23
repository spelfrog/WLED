#pragma once

#include "wled.h"

/*
 * Usermods allow you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * 
 * This is an example for a v2 usermod.
 * v2 usermods are class inheritance based and can (but don't have to) implement more functions, each of them is shown in this example.
 * Multiple v2 usermods can be added to one compilation easily.
 * 
 * Creating a usermod:
 * This file serves as an example. If you want to create a usermod, it is recommended to use usermod_v2_empty.h from the usermods folder as a template.
 * Please remember to rename the class and file to a descriptive name.
 * You may also use multiple .h and .cpp files.
 * 
 * Using a usermod:
 * 1. Copy the usermod into the sketch folder (same folder as wled00.ino)
 * 2. Register the usermod by adding #include "usermod_filename.h" in the top and registerUsermod(new MyUsermodClass()) in the bottom of usermods_list.cpp
 */

//class name. Use something descriptive and leave the ": public Usermod" part :)
class PotisControls : public Usermod {
private:
    //Private class members. You can declare variables and functions only accessible to your usermod here
    const short dead_zone_radius = 4;
    const short min_analog = 37;
    const short max_analog = 932;
    const float range_analog = float(max_analog-min_analog);
    unsigned long lastTime = 0;

    int old_hue = 0;
    int old_saturation = 0;
    int old_brightness = 0;


public:
    float fract(float x) { return x - int(x); }

    float mix(float a, float b, float t) { return a + (b - a) * t; }

    void hsv(float h, float s) {
        float v = 1;
        col[0] = int((v * mix(1.0, constrain(std::abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s)) * 255);
        col[1] = int((v * mix(1.0, constrain(std::abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s)) * 255);
        col[2] = int((v * mix(1.0, constrain(std::abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s)) * 255);
    }

    //Functions called by WLED
    int readPoti(int pin) {
        digitalWrite(pin, HIGH);
        int value = analogRead(A0);
        digitalWrite(pin, LOW);
        return value;
    }

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
        pinMode(A0, INPUT);
        pinMode(D0, OUTPUT);
        pinMode(D5, OUTPUT);
        pinMode(D6, OUTPUT);

        digitalWrite(D0, LOW);
        digitalWrite(D5, LOW);
        digitalWrite(D6, LOW);
    }


    void loop() {
        if (millis() - lastTime > 25) {
            lastTime = millis();
            int hue = readPoti(D0);
            int brightness = readPoti(D5);
            int saturation = readPoti(D6);
            if (abs(hue - old_hue) < dead_zone_radius
                and abs(brightness - old_brightness) <dead_zone_radius
                and abs(saturation - old_saturation) <dead_zone_radius)
                return;
            old_hue = hue;
            old_brightness = brightness;
            old_saturation = saturation;

            hsv(float(hue-min_analog)/range_analog, float(saturation-min_analog)/range_analog);
            bri = int(float(brightness-min_analog)/range_analog*256);

            Serial.print(brightness);
            Serial.print("<raw na>");
            Serial.println(bri);
            if(bri <= 5)
                bri =0;

            colorUpdated(CALL_MODE_NO_NOTIFY);
        }
    }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    //uint16_t getId() {
    //    return USERMOD_ID_EXAMPLE;
    //}

    //More methods can be added in the future, this example will then be extended.
    //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};
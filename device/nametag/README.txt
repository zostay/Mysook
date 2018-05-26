If you plan on compiling this, you will need to create a src/secrets.h file that
looks something like this to setup the wifi APs you want to allow your nametag
to use.

    #ifndef __SECRETS_H
    #define __SECRETS_H

    const char *ap_configs[] = {
        "ssid1", "password1",
        "ssid2", "password2",
        "ssid3", NULL, // no password
        "ssid4, "password4",
    };

    const int ap_config_count = 4;

    #endif//__SECRETS_H

This file is not included for the obvious reason that I do not wish to share my
personal AP passwords.

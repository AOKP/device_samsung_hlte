/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.
   Copyright (c) 2017, The LineageOS Project. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <android-base/properties.h>
#include <android-base/logging.h>
#include <cutils/properties.h>
#include "vendor_init.h"
#include "log.h"

#include "init_msm8974.h"

using android::base::GetProperty;

void set_rild_libpath(char const *variant)
{
    std::string libpath("/system/vendor/lib/libsec-ril.");
    libpath += variant;
    libpath += ".so";

    property_override("rild.libpath", libpath.c_str());
}

void cdma_properties(char const *operator_alpha,
        char const *operator_numeric,
        char const *default_network,
        char const *cdma_sub,
        char const *rild_lib_variant)
{
    /* Dynamic CDMA Properties */
    property_set("ro.cdma.home.operator.alpha", operator_alpha);
    property_set("ro.cdma.home.operator.numeric", operator_numeric);
    property_set("ro.telephony.default_network", default_network);
    property_set("ro.telephony.default_cdma_sub", cdma_sub);
    set_rild_libpath(rild_lib_variant);

    /* Static CDMA Properties */
    property_set("ril.subscription.types", "NV,RUIM");
    property_set("telephony.lteOnCdmaDevice", "1");
}

void gsm_properties(char const *rild_lib_variant)
{
    set_rild_libpath(rild_lib_variant);

    property_set("ro.telephony.default_network", "9");
    property_set("telephony.lteOnGsmDevice", "1");
}

#define ISMATCH(a, b) (!strncmp((a), (b), PROP_VALUE_MAX))

void init_target_properties()
{
    std::string platform;
    std::string bootloader;
    std::string device;

    platform = GetProperty("ro.board.platform", "");
    if (platform != ANDROID_TARGET)
        return;

    bootloader = GetProperty("ro.bootloader", "");

    if (bootloader.find("N9005") == 0) {
        /* hltexx */
        property_override("ro.build.fingerprint", "samsung/hltexx/hlte:5.0/LRX21V/N9005XXSGBQD5:user/release-keys");
        property_override("ro.build.description", "hltexx-user 5.0 LRX21V N9005XXSGBQD5 release-keys");
        property_override("ro.product.model", "SM-N9005");
        property_override("ro.product.device", "hlte");
        gsm_properties("gsm");
    } else if (bootloader.find("N900P") == 0) {
        /* hltespr - Sprint */
        property_override("ro.build.fingerprint", "samsung/hltespr/hltespr:5.0/LRX21V/N900PVPSEPL1:user/release-keys");
        property_override("ro.build.description", "hltespr-user 5.0 LRX21V N900PVPSEPL1 release-keys");
        property_override("ro.product.model", "SM-N900P");
        property_override("ro.product.device", "hltespr");
        cdma_properties("Sprint", "310120", "8", "1", "spr");
    } else {
        gsm_properties("gsm");
    }

    device = GetProperty("ro.product.device", "");
    LOG(INFO) << "Found bootloader id " << bootloader << " setting build properties for device " << device << std::endl;
}

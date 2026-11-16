#ifndef CondFormats_HGCalObjects_interface_HGCalECONDZSParamSoA_h
#define CondFormats_HGCalObjects_interface_HGCalECONDZSParamSoA_h

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"

namespace hgcal {

GENERATE_SOA_LAYOUT(HGCalECONDZSParamSoALayout,

                    //SOA_COLUMN(uint32_t, chId),
                    SOA_COLUMN(float, adcThreshold),
                    SOA_COLUMN(float, adcm1Threshold)
                    //SOA_COLUMN(float, noise),
                    //SOA_COLUMN(unsigned char, valid)

)

using HGCalECONDZSParamSoA = HGCalECONDZSParamSoALayout<>;

}  // namespace hgcal

#endif
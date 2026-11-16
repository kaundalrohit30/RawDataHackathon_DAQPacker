#ifndef CondFormats_HGCalObjects_interface_HGCalECONDZSParameterHost_h
#define CondFormats_HGCalObjects_interface_HGCalECONDZSParameterHost_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParamSoA.h"

namespace hgcal {

  // SoA with channel-level ZS parameters in host memory:
  //   adcThreshold: ADC threshold for zero suppression
  using HGCalECONDZSParameterHost = PortableHostCollection<HGCalECONDZSParamSoA>;

}  // namespace hgcal

#endif  // CondFormats_HGCalObjects_interface_HGCalECONDZSParameterHost_h

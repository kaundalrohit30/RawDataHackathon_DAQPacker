#ifndef CondFormats_HGCalObjects_interface_alpaka_HGCalECONDZSParameterDevice_h
#define CondFormats_HGCalObjects_interface_alpaka_HGCalECONDZSParameterDevice_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"
#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParamSoA.h"
#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParameterHost.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  namespace hgcal {

    using namespace ::hgcal;
    using HGCalECONDZSParameterDevice = PortableCollection<HGCalECONDZSParamSoA>;

  }  // namespace hgcal

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // CondFormats_HGCalObjects_interface_alpaka_HGCalECONDZSParameterDevice_h

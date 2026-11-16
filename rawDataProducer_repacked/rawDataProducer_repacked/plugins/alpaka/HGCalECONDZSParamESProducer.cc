#include "Eigen/Core"

// includes for CMSSW
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"

// includes for Alpaka
// Alpaka configuration MUST come first
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/host.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

// Alpaka Core
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESGetToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ModuleFactory.h"

#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParameterHost.h"
#include "CondFormats/HGCalObjects/interface/alpaka/HGCalECONDZSParameterDevice.h"

#include "CondFormats/DataRecord/interface/HGCalModuleConfigurationRcd.h"

//#include "CondFormats/DataRecord/interface/HGCalElectronicsMappingRcd.h"
//#include "CondFormats/HGCalObjects/interface/HGCalMappingModuleIndexer.h"
#include "CondFormats/HGCalObjects/interface/HGCalCalibrationParameterHost.h"
#include "CondFormats/HGCalObjects/interface/HGCalConfiguration.h"

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>


namespace ALPAKA_ACCELERATOR_NAMESPACE {
    namespace hgcal{
        class HGCalECONDZSParameterESProducer : public ESProducer {
            public:
                
                HGCalECONDZSParameterESProducer(const edm::ParameterSet& iConfig) : ESProducer(iConfig){
                    
                    auto cc = setWhatProduced(this);
                    calibToken_ = cc.consumes(iConfig.getParameter<edm::ESInputTag>("calibSource"));
                }
                
                static void fillDescriptions(edm::ConfigurationDescriptions& descriptions){
                    edm::ParameterSetDescription desc;
                    desc.add<edm::ESInputTag>("calibSource", edm::ESInputTag(""));
                    descriptions.addWithDefaultLabel(desc);

                }

                std::optional<::hgcal::HGCalECONDZSParameterHost> produce(const HGCalModuleConfigurationRcd& iRecord){
                    auto const& calibParam = iRecord.get(calibToken_);
                    auto const& calibParam_view = calibParam.const_view();
                    int32_t ncalibParam = calibParam_view.metadata().size();
                    ::hgcal::HGCalECONDZSParameterHost ZSParam(cms::alpakatools::host(), ncalibParam);
                    auto ZSParam_view = ZSParam.view();
                    for (int32_t i = 0; i < ncalibParam; ++i) {
                        //ZSParam_view.chId()[i] = calibParam_view[i].Channel();
                        ZSParam_view.adcThreshold()[i] = calibParam_view[i].ADC_ped() + 3 * calibParam_view[i].Noise();
                        ZSParam_view.adcm1Threshold()[i] = calibParam_view[i].ADC_ped() + 3 * calibParam_view[i].Noise(); //ADC and ADCm1 thtrdholds are same for now!!
                        //ZSParam_view.noise()[i] = calibParam_view[i].Noise();
                        //ZSParam_view.valid()[i] = calibParam_view[i].valid();
                    }
                    return ZSParam;
                }

            private:
                edm::ESGetToken<hgcalrechit::HGCalCalibParamHost, HGCalModuleConfigurationRcd> calibToken_;
    };        
        
    }
}

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(hgcal::HGCalECONDZSParameterESProducer);

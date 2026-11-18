// -*- C++ -*-
//
// Package:    rawDataProducer_repacked_gen/rawDataProducer_repacked
// Class:      rawDataProducer_repacked
//
/**\class rawDataProducer_repacked rawDataProducer_repacked.cc rawDataProducer_repacked_gen/rawDataProducer_repacked/plugins/rawDataProducer_repacked.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Rohit Kaundal
//         Created:  Wed, 05 Aug 2026 06:31:40 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "FWCore/Utilities/interface/ESGetToken.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

//DetId
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
//Digi information
#include "DataFormats/HGCalDigi/interface/HGCalDigiHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
//mapping information
#include "CondFormats/DataRecord/interface/HGCalElectronicsMappingRcd.h"
#include "CondFormats/DataRecord/interface/HGCalDenseIndexInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingParameterHost.h"
#include "CondFormats/HGCalObjects/interface/HGCalMappingModuleIndexer.h"
#include "CondFormats/DataRecord/interface/HGCalModuleConfigurationRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCalibrationParameterHost.h"
#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParameterHost.h"

#include "CondFormats/HGCalObjects/interface/HGCalECONDZSParameterHost.h"
#include "CondFormats/HGCalObjects/interface/HGCalConfiguration.h"
#include "DataFormats/HGCalDigi/interface/HGCalECONDPacketInfoSoA.h"
#include "DataFormats/HGCalDigi/interface/HGCalECONDPacketInfoHost.h"
#include "DataFormats/HGCalDigi/interface/HGCalFEDPacketInfoSoA.h"
#include "DataFormats/HGCalDigi/interface/HGCalFEDPacketInfoHost.h"
#include "Geometry/HGCalMapping/interface/HGCalMappingTools.h"
#include "FWCore/Utilities/interface/Exception.h"
//Data packing
//#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"
#include "SimCalorimetry/HGCalSimAlgos/interface/HGCalRawDataPackingTools.h"
#include "DataFormats/HGCalDigi/interface/HGCalRawDataDefinitions.h"
#include "DataFormats/FEDRawData/interface/SLinkRocketHeaders.h"

#include "DataFormats/FEDRawData/interface/RawDataBuffer.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include <iostream>
#include <algorithm>
#include <boost/crc.hpp>
#include <stdexcept>
#include <map>

#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include <vector>
#include <memory>
//#include "TFile.h"
#include <unordered_map>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

uint16_t compressToT(uint16_t totraw) {
    if (totraw > 0x1ff)
      return (0x200 | (totraw >> 3));
    return (totraw & 0x1ff);
  }

class rawDataProducer_repacked : public edm::stream::EDProducer<> {
public:
  explicit rawDataProducer_repacked(const edm::ParameterSet&);
  ~rawDataProducer_repacked() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginStream(edm::StreamID) override;
  void produce(edm::Event&, const edm::EventSetup&) override;
  void endStream() override;

  //void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //void endRun(edm::Run const&, edm::EventSetup const&) override;
  //void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<hgcaldigi::HGCalDigiHost> digisToken_;
  //const edm::ESGetToken<hgcalrechit::HGCalCalibParamHost, HGCalModuleConfigurationRcd> calibToken_;
  edm::EDGetTokenT<hgcaldigi::HGCalECONDPacketInfoHost> econdInfoTkn_;
 // edm::EDGetTokenT<hgcaldigi::HGCalFEDPacketInfoHost> fedInfoTkn_;
  
//edm::EDGetTokenT<HGCalTestSystemTrigTimeCollection> trigtimeToken_;
  //edm::EDGetTokenT<HGCalTestSystemMCP> mcpToken_;
  //edm::EDGetTokenT<HGCalTestSystemTimingIn> timeinToken_;

  edm::ESGetToken<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd> denseIndexInfoTkn_;
  edm::ESGetToken<hgcal::HGCalMappingCellParamHost, HGCalElectronicsMappingRcd> cellTkn_;
  edm::ESGetToken<hgcal::HGCalMappingModuleParamHost, HGCalElectronicsMappingRcd> moduleTkn_;
  edm::ESGetToken<HGCalMappingModuleIndexer, HGCalElectronicsMappingRcd> moduleIndexTkn_;
  edm::ESGetToken<HGCalConfiguration, HGCalModuleConfigurationRcd> configToken_;
  edm::ESGetToken<hgcal::HGCalECONDZSParameterHost, HGCalModuleConfigurationRcd> zsParamToken_;
  edm::EDPutTokenT<RawDataBuffer> rawDataBufferPutToken_;

  int eventNum;
  std::vector<uint16_t> payloadLength, BX, L1A, fedBX, cm0, cm1;
  //std::vector<std::vector<uint16_t>> adc_Threshold, adcm1_Threshold, toa_Threshold;
 
  std::vector<uint8_t> Orbit;
  std::vector<uint32_t> cbBX, cbOrbit, cbL1a, fedObt;
  std::vector<uint64_t> fedL1A;

};

//
rawDataProducer_repacked::rawDataProducer_repacked(const edm::ParameterSet& iConfig)
: digisToken_(consumes<hgcaldigi::HGCalDigiHost>(iConfig.getUntrackedParameter<edm::InputTag>("hgcalDigis"))),
    //calibToken_{esConsumes(iConfig.getParameter<edm::ESInputTag>("calibSource"))},
    econdInfoTkn_(consumes<hgcaldigi::HGCalECONDPacketInfoHost>(iConfig.getUntrackedParameter<edm::InputTag>("econdInfo"))),
    //fedInfoTkn_(consumes<hgcaldigi::HGCalFEDPacketInfoHost>(iConfig.getUntrackedParameter<edm::InputTag>("hgcalDigis"))),
    
   //trigtimeToken_(consumes<HGCalTestSystemTrigTimeCollection>(iConfig.getParameter<edm::InputTag>("metaData"))),
    //mcpToken_(consumes<HGCalTestSystemMCP>(iConfig.getParameter<edm::InputTag>("metaData"))),
    //timeinToken_(consumes<HGCalTestSystemTimingIn>(iConfig.getParameter<edm::InputTag>("metaData"))),
    denseIndexInfoTkn_(esConsumes<hgcal::HGCalDenseIndexInfoHost, HGCalDenseIndexInfoRcd>()),
    cellTkn_(esConsumes()),
    moduleTkn_(esConsumes()),
    moduleIndexTkn_(esConsumes<HGCalMappingModuleIndexer, HGCalElectronicsMappingRcd>()),
    configToken_(esConsumes<HGCalConfiguration, HGCalModuleConfigurationRcd>()),
    zsParamToken_(esConsumes<hgcal::HGCalECONDZSParameterHost, HGCalModuleConfigurationRcd>()),
    rawDataBufferPutToken_(produces())

{

}

rawDataProducer_repacked::~rawDataProducer_repacked() {
  
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void rawDataProducer_repacked::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  
  eventNum = iEvent.id().event();
  payloadLength.clear();
  BX.clear();
  L1A.clear();
  Orbit.clear();
  cm0.clear();
  cm1.clear();
  cbBX.clear();
  cbOrbit.clear();
  fedBX.clear();
  fedL1A.clear();
  fedObt.clear();

  const auto& digis = iEvent.getHandle(digisToken_);
  if (!digis.isValid()) {
      throw cms::Exception("ProductNotFound")
          << "HGCal digis collection is not available!";
  }
  auto const& digis_view = digis->const_view();
  //int32_t ndigis = digis->const_view().metadata().size();

  //auto const& hostCalibParamProvider = iSetup.getData(calibToken_);
  auto const& zsParams = iSetup.getData(zsParamToken_);
  auto const& zsParams_view = zsParams.const_view();
  const auto nzs = zsParams_view.metadata().size();

  auto const& denseIndexInfo = iSetup.getData(denseIndexInfoTkn_);
  auto const& denseIndexInfo_view = denseIndexInfo.const_view();
  int32_t ndenseIndices = denseIndexInfo_view.metadata().size();

  const auto& econdInfo = iEvent.getHandle(econdInfoTkn_);
  if (!econdInfo.isValid()) {
    throw cms::Exception("ProductNotFound")
        << "ECOND information collection is not available!";
  }
  auto const& econdInfo_view = econdInfo->const_view();

  //const auto& fedInfo = iEvent.getHandle(fedInfoTkn_);
  //auto const& fedInfo_view = fedInfo->const_view();

  auto const& cellInfo = iSetup.getData(cellTkn_);
  auto const& cellInfo_view = cellInfo.const_view();
  auto const& moduleInfo = iSetup.getData(moduleTkn_);
  auto const& moduleIndex = iSetup.getData(moduleIndexTkn_);
  auto const& moduleInfo_view = moduleInfo.const_view();

  int32_t ndigis = 0;
  if(digis.isValid()){
  ndigis = digis->const_view().metadata().size();
  //assert(ndigis == ndenseIndices);
  
  }

  if (ndigis != ndenseIndices || nzs != ndenseIndices) {
    throw cms::Exception("SizeMismatch")
        << "Mismatch between digis, dense indices and ZS parameters!";
  }
  
  cout << "EventNo:  " << eventNum << endl;
  
  uint32_t nmodules = moduleIndex.maxModulesCount();
  std::vector<uint32_t> cbIdx(0), ECONDidx(0), nErx(0), SLinkIdx(0), fedID(0);
  std::vector<bool> econdPassThrough;
  const auto& config = iSetup.getData(configToken_);
  for(size_t i = 0; i < nmodules; i++){
    cbIdx.push_back(moduleInfo_view[i].captureblock());
    ECONDidx.push_back(moduleInfo_view[i].econdidx());
    nErx.push_back(moduleIndex.getNumERxs(moduleInfo_view[i].fedid(), moduleInfo_view[i].econdidx()));
    const auto& econdConfig = config.feds[moduleInfo_view[i].fedid()].econds[moduleInfo_view[i].econdidx()];
    econdPassThrough.push_back(econdConfig.passthrough);
    SLinkIdx.push_back(moduleInfo_view[i].slinkidx());
    fedID.push_back(moduleInfo_view[i].fedid());
    //std::cout << "nErx: " << moduleIndex.getNumERxs(moduleInfo_view[i].fedid(), moduleInfo_view[i].econdidx()) << "  ECOND_ID: " << moduleInfo_view[i].econdidx() << " CB ID: " << moduleInfo_view[i].captureblockidx() <<std::endl;
  }

    /*cout << std::left
     << std::setw(10) << "tctp"
     << std::setw(10) << "adc"
     << std::setw(10) << "adcm1"
     << std::setw(10) << "adcTH"
     << std::setw(10) << "tot"
     << std::setw(10) << "toa"
     << std::setw(10) << "cm"
     << std::setw(10) << "flags"
     << std::setw(10) << "channel"
     << std::setw(10) << "fedId"
     << std::setw(10) << "ECONDid"
     << std::setw(10) << "CBid"
     << std::setw(15) << "fedReadoutSeq"
     << std::setw(20) << "chI1"
     << std::setw(20) << "chI2"
     << std::setw(15) << "modI1"
     << std::setw(15) << "modI2"
     << std::setw(15) << "chType"
     << std::setw(15) << "isSiPM"
     << std::setw(15) << "iscalib"
     << '\n';*/

  int eRxNum = 0;
  int chIdx = 0;
  int TotalERx =  std::accumulate(nErx.begin(), nErx.end(), uint32_t{0});
  
  std::vector<hgcal::econd::ERxData> allERxData(TotalERx);
  std::vector<hgcal::econd::ERxChannelEnable> enableMaps(TotalERx, hgcal::econd::ERxChannelEnable(37,false));

  std::vector<uint8_t> econd_status(12, hgcal::backend::ECONDPacketStatus::InactiveECOND); //for CB 

  for (int32_t i = 0; i < ndenseIndices && digis.isValid(); i++) {
    //uint16_t TOT = digis_view.tot()[i];

    uint16_t adc_Thr = zsParams_view.adcThreshold()[i];      //hostCalibParamProvider.view()[i].ADC_ped() + k*hostCalibParamProvider.view()[i].Noise();
    uint16_t adcm1_Thr = zsParams_view.adcm1Threshold()[i];  //hostCalibParamProvider.view()[i].ADC_ped() + k*hostCalibParamProvider.view()[i].Noise();

    uint32_t cellInfoIdx(denseIndexInfo_view.cellInfoIdx()[i]);
    uint32_t modInfoIdx(denseIndexInfo_view.modInfoIdx()[i]);
    /*if(digis_view.flags()[i] != hgcal::DIGI_FLAG::NotAvailable){
      cout << std::left
       << std::setw(10) << static_cast<unsigned int>(digis_view.tctp()[i])
       << std::setw(10) << digis_view.adc()[i]
       << std::setw(10) << digis_view.adcm1()[i]
       << std::setw(10) << adc_Thr
       << std::setw(10) << compressToT(digis_view.tot()[i])
       << std::setw(10) << digis_view.toa()[i]
       << std::setw(10) << digis_view.cm()[i]
       << std::setw(10) << digis_view.flags()[i]
       << std::setw(10) << denseIndexInfo_view.chNumber()[i]
       << std::setw(10) << denseIndexInfo_view. ()[i]
       << std::setw(10) << moduleInfo_view.econdidx()[modInfoIdx]
       << std::setw(10) << moduleInfo_view.captureblockidx()[modInfoIdx]
       << std::setw(15) << denseIndexInfo_view.fedReadoutSeq()[i]
       << std::setw(20) << cellInfo_view.i1()[cellInfoIdx]
       << std::setw(20) << cellInfo_view.i2()[cellInfoIdx]
       << std::setw(15) << moduleInfo_view.i1()[modInfoIdx]
       << std::setw(15) << moduleInfo_view.i2()[modInfoIdx]
       << std::setw(15) << cellInfo_view.t()[cellInfoIdx]
       << std::setw(15) << moduleInfo_view.isSiPM()[modInfoIdx]
       << std::setw(15) << cellInfo_view.iscalib()[cellInfoIdx]
       << '\n';
    } */
    eRxNum = i / 37;
    chIdx  = denseIndexInfo_view.chNumber()[i] % 37;

    //cout << "eRxNum: " << eRxNum << "  chId: " << chIdx << "  ADC_Thr: " << zsParams_view.adcThreshold()[i] << endl;
  
    if(digis_view.flags()[i] == hgcal::DIGI_FLAG::NotAvailable){
      enableMaps.at(eRxNum).at(chIdx) = false;
      allERxData.at(eRxNum).tctp.push_back(uint8_t(0));
      allERxData.at(eRxNum).adc.push_back(uint16_t(0));
      allERxData.at(eRxNum).adcm.push_back(uint16_t(0));
      allERxData.at(eRxNum).toa.push_back(uint16_t(0));
      allERxData.at(eRxNum).tot.push_back(uint16_t(0));
      allERxData.at(eRxNum).passZS.push_back(false);
      allERxData.at(eRxNum).passZSm1.push_back(false);
      //allERxData.at(eRxNum).meta.push_back(moduleInfo_view.captureblockidx()[modInfoIdx]);
    }
    else{
      enableMaps.at(eRxNum).at(chIdx) = true;
      allERxData.at(eRxNum).tctp.push_back(digis_view.tctp()[i]);
      allERxData.at(eRxNum).adc.push_back(digis_view.adc()[i]);
      allERxData.at(eRxNum).adcm.push_back(digis_view.adcm1()[i]);
      allERxData.at(eRxNum).toa.push_back(digis_view.toa()[i]);
      allERxData.at(eRxNum).tot.push_back(compressToT(digis_view.tot()[i]));
      allERxData.at(eRxNum).passZS.push_back((digis_view.adc()[i] > adc_Thr ? true : false));
      allERxData.at(eRxNum).passZSm1.push_back((digis_view.adcm1()[i] > adcm1_Thr ? true : false));
      
    }

    
  }
  int32_t necons = 0;
  if(econdInfo.isValid()){
  necons = econdInfo->const_view().metadata().size();
  //assert(ndigis == ndenseIndices);

  for(int imod=0; imod<necons; imod++){
    const auto econd = econdInfo_view[imod];
    //payloads[imod] = econd.payloadLength();
    payloadLength.push_back(econd.payloadLength());
    BX.push_back(econd.BX());
    L1A.push_back(econd.L1A());
    Orbit.push_back(econd.Orbit());
    //if(imod == 0){
    cbBX.push_back(econd.CBBX());
    cbOrbit.push_back(econd.CBOrbit());
    cbL1a.push_back(econd.CBL1A());
    for(size_t ierx=0; ierx<nErx[imod]; ierx++){
      
      cm0.push_back(econd.cm().coeff(ierx,0));
      cm1.push_back(econd.cm().coeff(ierx,1));
      
    }

  }
}

  ///////////oooooooooooOOOOOOOOOOOOO FED Info OOOOOOOOOOOOOOOoooooooooooooooooo////////////////////
  //int32_t nfed = 0;
  //if(fedInfo.isValid()){
    //nfed = fedInfo->const_view().metadata().size();
    //assert(ndigis == ndenseIndices);
  //}

  //for(int i = 0; i < nfed && fedInfo.isValid(); i++){
    //const auto fed = fedInfo_view[i];
    //if(fed.FEDPayload() != 0){
      //fedBX.push_back(fed.FEDBX());
      //fedL1A.push_back(fed.FEDL1A());
      //fedObt.push_back(fed.FEDOrbit());
    //}
  //}


  //oooooooooooooOOOOOOOOOOOOOOO For getting the right sequence of DIGIS oooooooooooOOOOOOOOOOOOOOOOOOOO


    struct ECONDGroup {
      uint32_t econdID, cbID, slinkID, FedId, eRxNum, CBbx, CBL1A, CBOrbit;
      uint16_t pLoad, econdBX, econdL1A;
      uint8_t econdOrbit;
      bool pThrough;
      size_t startIndex;
      size_t chIndex;
    };

    std::vector<ECONDGroup> groups;

    size_t startIndex = 0;
    size_t chIndex = 0;

    for (size_t i = 0; i < nErx.size(); ++i) {

        ECONDGroup group;

        group.econdID   = ECONDidx[i];
        group.cbID      = cbIdx[i];
        group.slinkID   = SLinkIdx[i];
        group.FedId     = fedID[i];
        group.eRxNum    = nErx[i];
        group.pLoad     = payloadLength[i];
        group.econdBX    = BX[i];
        group.econdL1A   = L1A[i];
        group.econdOrbit  = Orbit[i];
        group.CBbx      = cbBX[i];
        group.CBL1A    = cbL1a[i];
        group.CBOrbit   = cbOrbit[i];
        group.pThrough  = econdPassThrough[i];
        group.startIndex = startIndex;
        group.chIndex = chIndex;

        groups.push_back(group);

        startIndex += nErx[i];

        chIndex += nErx[i]*37; //  37 channels per eRx
    }

    std::sort(
        groups.begin(),
        groups.end(),
        [](const ECONDGroup& a, const ECONDGroup& b) {
            // Primary sorting: FED ID
            if (a.FedId != b.FedId) {
                return a.FedId < b.FedId;
            }
          
            // Secondary sorting: Capture Block ID
            if (a.cbID != b.cbID) {
                return a.cbID < b.cbID;
            }
          
            // Tertiary sorting: ECOND ID
            return a.econdID < b.econdID;
        }
    );

    std::vector<hgcal::econd::ERxData> oldAllErxData = allERxData;
    std::vector<hgcal::econd::ERxChannelEnable> oldEnableChannel = enableMaps;
    std::vector<uint16_t> old_cMode0 = cm0;
    std::vector<uint16_t> old_cMode1 = cm1;
    //std::vector<uint16_t> old_ploadLength = payloadLength;

    std::vector<uint32_t> newERxNum;
    std::vector<uint32_t> newECONDid;
    std::vector<uint32_t> newCaptureBlockID;
    std::vector<uint32_t> newSLinkID;
    std::vector<uint32_t> newFedID;

    std::vector<hgcal::econd::ERxData> newAllErxData;
    std::vector<hgcal::econd::ERxChannelEnable> newEnableChMaps;
    std::vector<uint16_t> new_cMode0;
    std::vector<uint16_t> new_cMode1;
    std::vector<uint16_t> new_payloadLength;
    std::vector<uint16_t> new_econdBX;
    std::vector<uint16_t> new_econdL1A;
    std::vector<uint8_t> new_econdOrbit;
    std::vector<uint32_t> new_CBbx;
    std::vector<uint32_t> new_CBL1A;
    std::vector<uint32_t> new_CBOrbit;
    std::vector<bool> new_passThrough;

    newERxNum.reserve(nErx.size());
    newECONDid.reserve(ECONDidx.size());
    newCaptureBlockID.reserve(cbIdx.size());
    newSLinkID.reserve(SLinkIdx.size());
    newFedID.reserve(fedID.size());
    newAllErxData.reserve(allERxData.size());
    newEnableChMaps.reserve(enableMaps.size());
    new_cMode0.reserve(cm0.size());
    new_cMode1.reserve(cm1.size());
    new_payloadLength.reserve(payloadLength.size());
    new_econdBX.reserve(BX.size());
    new_econdL1A.reserve(L1A.size());
    new_econdOrbit.reserve(Orbit.size());
    new_CBbx.reserve(cbBX.size());
    new_CBL1A.reserve(cbL1a.size());
    new_CBOrbit.reserve(cbOrbit.size());
    new_passThrough.reserve(econdPassThrough.size());

    for (const auto& group : groups) {

        // Add group information
        newERxNum.push_back(group.eRxNum);
        newECONDid.push_back(group.econdID);
        newCaptureBlockID.push_back(group.cbID);
        newSLinkID.push_back(group.slinkID);
        newFedID.push_back(group.FedId);
        new_payloadLength.push_back(group.pLoad);
        new_econdBX.push_back(group.econdBX);
        new_econdL1A.push_back(group.econdL1A);
        new_econdOrbit.push_back(group.econdOrbit);
        new_CBbx.push_back(group.CBbx);
        new_CBL1A.push_back(group.CBL1A);
        new_CBOrbit.push_back(group.CBOrbit);
        new_passThrough.push_back(group.pThrough);

        // Copy this group's eRx entries
        for (size_t j = 0; j < group.eRxNum; ++j) {

            newAllErxData.push_back(
                oldAllErxData[group.startIndex + j]
            );
            newEnableChMaps.push_back(oldEnableChannel[group.startIndex + j]);

            new_cMode0.push_back(old_cMode0[group.startIndex + j]);
            new_cMode1.push_back(old_cMode1[group.startIndex + j]);
        }

    }

    nErx = std::move(newERxNum);
    ECONDidx = std::move(newECONDid);
    cbIdx = std::move(newCaptureBlockID);
    SLinkIdx = std::move(newSLinkID);
    fedID = std::move(newFedID);
    allERxData = std::move(newAllErxData);
    enableMaps = std::move(newEnableChMaps);
    cm0 = std::move(new_cMode0);
    cm1 = std::move(new_cMode1);
    payloadLength = std::move(new_payloadLength);
    econdPassThrough = std::move(new_passThrough);
    BX = std::move(new_econdBX);
    L1A = std::move(new_econdL1A);
    Orbit = std::move(new_econdOrbit);
    cbBX = std::move(new_CBbx);
    cbL1a = std::move(new_CBL1A);
    cbOrbit = std::move(new_CBOrbit);
  //oooooooooOOOOOOOOOOOOOOOOOOOo==================oooooooooooOOOOOOOOOOOOOOOOOOOOO

struct ECONDInfo {
    uint32_t nErx;
    uint16_t payloadLength, ECOND_BX, ECOND_L1A;
    uint8_t ECOND_Orbit;
    bool passThrough;

    size_t StartIndex;
    size_t rowIndex;
};


std::map<uint32_t,std::map<uint32_t,std::map<uint32_t, ECONDInfo>>> fedMap;

size_t StartIndex = 0;

for (size_t i = 0; i < nErx.size(); ++i) {

    ECONDInfo info;

    //info.slinkID      = SLinkIdx[i];
    info.nErx         = nErx[i];
    info.payloadLength = payloadLength[i];
    info.passThrough  = econdPassThrough[i];
    info.ECOND_BX     = BX[i];
    info.ECOND_L1A    = L1A[i];
    info.ECOND_Orbit  = Orbit[i];

    info.StartIndex = StartIndex;
    info.rowIndex   = i;

    auto result = fedMap[fedID[i]][cbIdx[i]].emplace(
        ECONDidx[i], info
    );

    if (!result.second) {
        throw std::runtime_error("Duplicate ECOND ID within FED/CB");
    }

    StartIndex += nErx[i];
}


std::vector<uint32_t> econdPacket{0};  
std::vector<uint32_t> CRC_calc;
std::vector<uint32_t> crcvec;
uint16_t header =  340;//170;
uint8_t ht = 0;
uint8_t ebo = 0;
uint8_t ehHam = 9;
uint8_t rr = 0;
uint8_t stat = 7;
uint8_t Ham = 0;
bool bitE = true;
bool charMode = false;
uint8_t econCrc = 0;
uint32_t padding = 0;
uint32_t cbIdxCounter = 0;
//std::vector<uint8_t> econd_status(12, hgcal::backend::ECONDPacketStatus::InactiveECOND);
struct SLinkPacketInfo {
    uint32_t fedId;
    std::vector<unsigned char> packet;
};
std::vector<SLinkPacketInfo> allSLinkPackets;
size_t totalBufferSize = 0;

for (const auto& fedPair : fedMap) {
    econdPacket.clear();
    uint32_t fed = fedPair.first;

    for (const auto& cbPair : fedPair.second) {
        //econd_status.clear();
        uint32_t cb = cbPair.first;
        for(size_t enableEcon = 0; enableEcon < cbPair.second.size(); enableEcon++){
          econd_status[enableEcon] = hgcal::backend::ECONDPacketStatus::Normal;
          //econd_status[1]     = hgcal::backend::ECONDPacketStatus::Normal;
        }

        auto cbHeader = hgcal::backend::buildCaptureBlockHeader(cbBX[cbIdxCounter],
                                                                cbOrbit[cbIdxCounter],//evt,
                                                                cbOrbit[cbIdxCounter],
                                                                econd_status);
        uint32_t CAPTUREBLOCK_RESERVED_POS = 25;
        uint32_t CAPTUREBLOCK_RESERVED_MASK = 0x7f;
        uint32_t value = 0x7f;
        cbHeader[0] &= ~(CAPTUREBLOCK_RESERVED_MASK << CAPTUREBLOCK_RESERVED_POS);
        cbHeader[0] |= ((value & CAPTUREBLOCK_RESERVED_MASK) << CAPTUREBLOCK_RESERVED_POS);
        econdPacket.push_back(cbHeader[0]);
        econdPacket.push_back(cbHeader[1]);
        cbIdxCounter += cbPair.second.size();
        
        for (const auto& econdPair : cbPair.second) {

            uint32_t econd = econdPair.first;

            const auto& info = econdPair.second;

            if(info.payloadLength > 469) {
                throw std::runtime_error("Payload length exceeds maximum allowed size for ECOND packet");
              continue;  //skip ECOND which exceeds maximum payload length

            }

            auto econdHeader = hgcal::econd::eventPacketHeader(header,
                                                               info.payloadLength, 
                                                               info.passThrough,
                                                               true,
                                                               ht,
                                                               ebo,
                                                               true,
                                                               false,
                                                               ehHam,
                                                               info.ECOND_BX,
                                                               info.ECOND_L1A,
                                                               info.ECOND_Orbit,
                                                               false,
                                                               rr);
            
            econdHeader[1] |= (econCrc & hgcal::ECOND_FRAME::EHCRC_MASK) << hgcal::ECOND_FRAME::EHCRC_POS;
            econdPacket.push_back(econdHeader[0]);  //filling EcondHeaders
            econdPacket.push_back(econdHeader[1]);                                                              

            for (size_t j = 0; j < info.nErx; ++j) {

                size_t eRxIndex = info.StartIndex + j;

                const auto& erxData = allERxData.at(eRxIndex);
                const auto& enabledChannels = enableMaps.at(eRxIndex);

                bool eRxPresent = std::any_of(enabledChannels.begin(),
                                              enabledChannels.end(),
                                              [](bool enabled) { return enabled; }
                                              );
                
                if (!eRxPresent) {
                  bitE = false;
                  const auto eRxheader = hgcal::econd::eRxSubPacketHeader(stat, Ham, bitE, cm0[eRxIndex], cm1[eRxIndex], enabledChannels);  //eRx Header for each eRX
                  econdPacket.push_back(eRxheader[0]);   //filling ERxHeaders 
                  CRC_calc.push_back(eRxheader[0]);                    
                } else{       
                    const auto eRxheader = hgcal::econd::eRxSubPacketHeader(stat, Ham, bitE, cm0[eRxIndex], cm1[eRxIndex], enabledChannels);  //eRx Header for each eRX
                    econdPacket.push_back(eRxheader[0]);     //filling ERxHeaders 
                    econdPacket.push_back(eRxheader[1]);
                          
                    CRC_calc.push_back(eRxheader[0]);      
                    CRC_calc.push_back(eRxheader[1]);
                          
                    const auto erx_chan_data = hgcal::econd::produceERxData(enabledChannels, erxData,  charMode, info.passThrough);  //eRx data for each eRX
                    for(size_t ch = 0; ch < erx_chan_data.size(); ++ch){
                      econdPacket.push_back(erx_chan_data[ch]);   //filling eRx payload(channel data)
                      CRC_calc.push_back(erx_chan_data[ch]);
                    }
                  }
                } //eRx loop ending

        crcvec.assign(CRC_calc.begin(), CRC_calc.end());
        std::transform(crcvec.begin(), crcvec.end(), crcvec.begin(), [](uint32_t w) {
        return ((w << 24) & 0xFF000000) | ((w << 8) & 0x00FF0000) | ((w >> 8) & 0x0000FF00) |
              ((w >> 24) & 0x000000FF);  //swapping endianness
        });

        auto array = &(crcvec[0]);
        auto bytes = reinterpret_cast<const unsigned char *>(array);
        auto crc32 = boost::crc<32,
                             hgcal::ECOND_FRAME::CRC_POL,
                             hgcal::ECOND_FRAME::CRC_INITREM,
                             hgcal::ECOND_FRAME::CRC_FINALXOR,
                             false,
                             //false>(bytes, 0 * 4);
                             false>(bytes, CRC_calc.size() * 4);  //need to be checked !!!!!
        
        if((CRC_calc.size() + 1) % 2 != 0){
          econdPacket.push_back(crc32);
          econdPacket.push_back(padding);  //padding word
        }else{
          econdPacket.push_back(crc32);
        }

        //cout << "Calculate Payload Length: " << CRC_calc.size() << "  ECOND Payload Length: " << info.payloadLength  << endl;

        CRC_calc.clear();

        } // ECOND loop ending
    } // CB loop ending

  /////////////oooooooooooOOOOOOOOOOOO SLink Packet OOOOOOOOOOOOOOOOoooooooooooooooo////////////////

  for (size_t i = 0; i + 1 < econdPacket.size(); i += 2) {
      std::swap(econdPacket[i], econdPacket[i + 1]);       // To get the correct sequence of the data words
  }

  size_t payloadBytes = econdPacket.size() * sizeof(uint32_t);

  size_t remainder = payloadBytes % 16;
  if (remainder != 0) {
      size_t paddingBytes = 16 - remainder;
      // Since each vector element is 4 bytes
      size_t paddingWords = paddingBytes / sizeof(uint32_t);

      for (size_t i = 0; i < paddingWords; ++i) {
          econdPacket.push_back(0);
      }
  }

  size_t payloadBytes_afterPadding = econdPacket.size() * sizeof(uint32_t);
  size_t totalSize = sizeof(SLinkRocketHeader_v3) + econdPacket.size() * sizeof(uint32_t) + sizeof(SLinkRocketTrailer_v3);
  //uint32_t sid = 1601;
  uint8_t emu_status = 0;
  uint16_t l1a_types = 132;
  uint8_t l1a_phys = 0;
  uint64_t global_event_id = 0;//fedL1A[0];
  uint16_t status = 4;
  uint16_t crc = 57005;
  uint16_t daqcrc = 57005;
  uint16_t fedBX = 0;//fedBX[0];
  uint32_t fedObt = 0;//fedObt[0];

  //unsigned char srcData[totalSize];

  constexpr size_t hdrsize = sizeof(SLinkRocketHeader_v3);
  constexpr size_t trsize  = sizeof(SLinkRocketTrailer_v3);

  //cout << "ECOND packetSize: " << econdPacket.size() << "  payloadBytes: "  << econdPacket.size() * sizeof(uint32_t) << "  totalSize: " << totalSize << endl;

  //unsigned char slinkPacket[totalSize];
  //std::memset(slinkPacket, 0, totalSize);
  std::vector<unsigned char> slinkPacket(totalSize, 0);
  
  //auto sh0 = new ((void*)slinkPacket) SLinkRocketHeader_v3(fed, l1a_types, l1a_phys, emu_status, global_event_id);
  auto sh0 = new (static_cast<void*>(slinkPacket.data())) SLinkRocketHeader_v3(fed, l1a_types, l1a_phys, emu_status, global_event_id);
  std::memcpy(
    slinkPacket.data() + hdrsize,
    econdPacket.data(),
    payloadBytes_afterPadding
  );
  //std::memcpy(
  //    slinkPacket + hdrsize,
  //    econdPacket.data(),
  //    payloadBytes);

  auto st0 = new (
    static_cast<void*>(slinkPacket.data() + hdrsize + payloadBytes_afterPadding)) SLinkRocketTrailer_v3(status, crc, fedObt, fedBX, totalSize >> SLR_WORD_NUM_BYTES_SHIFT, daqcrc);

  totalBufferSize += slinkPacket.size();

  if (totalBufferSize > std::numeric_limits<uint32_t>::max()) {
    throw cms::Exception("RawDataBuffer")
        << "Total SLink packet size exceeds uint32_t capacity";
  }

  allSLinkPackets.push_back(
      {fed, std::move(slinkPacket)}
  );
  
} // FED loop ending

auto rawDataBuffer = std::make_unique<RawDataBuffer>(
                     static_cast<uint32_t>(totalBufferSize));
    
for (const auto& packetInfo : allSLinkPackets) {

    uint32_t fed = packetInfo.fedId;

    const auto& packet = packetInfo.packet;

    uint32_t packetSize = static_cast<uint32_t>(packet.size());

    rawDataBuffer->addSource(
        fed,
        packet.data(),
        packetSize
    );
}

iEvent.put(rawDataBufferPutToken_, std::move(rawDataBuffer));

}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void rawDataProducer_repacked::beginStream(edm::StreamID) {
  // please remove this method if not needed
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void rawDataProducer_repacked::endStream() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void rawDataProducer_repacked::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.addUntracked<edm::InputTag>(
    "hgcalDigis",
    edm::InputTag("hgCalDigitizerProducer", "HGCROCDigis")
);

desc.addUntracked<edm::InputTag>(
    "econdInfo",
    edm::InputTag("hgCalDigitizerProducer", "HGCECONInfo")
);
  desc.add("calibSource", edm::ESInputTag{})->setComment("Label for calibration parameters");

  descriptions.add("rawDataBufferProducer", desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(rawDataProducer_repacked);
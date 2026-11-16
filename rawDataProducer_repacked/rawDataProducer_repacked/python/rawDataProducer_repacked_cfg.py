import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing

configdir = "/eos/user/r/rkaundal/Rohit/RawData_Hackthon/CMSSW_20_1_0_pre2/src/HGCalCommissioning/Calibrations"

options = VarParsing('python')
options.register('verbosity',0,mytype=VarParsing.varType.int,
                 info='Tester verbosity: 0 = Base+FED prints / 1 = +Module prints / 2 = +Cell prints')
options.register('modules','HGCalCommissioning/Calibrations/TB2026/maps/modulelocator_v4.txt',mytype=VarParsing.varType.string,
                 info="Path to module mapper. Absolute, or relative to CMSSW src directory")


#options.register('sicells','Geometry/HGCalMapping/data/CellMaps/WaferCellMapTraces.txt',mytype=VarParsing.varType.string,
options.register('sicells','HGCalCommissioning/Calibrations/P5/maps/WaferCellMapTraces.txt',mytype=VarParsing.varType.string,
                 info="Path to Si cell mapper. Absolute, or relative to CMSSW src directory")
#options.register('sicells','Geometry/HGCalMapping/data/CellMaps/WaferCellMapTraces.txt',mytype=VarParsing.varType.string,
#                 info="Path to Si cell mapper. Absolute, or relative to CMSSW src directory")

options.register('sipmcells','HGCalCommissioning/Calibrations/TB2026/maps/channels_sipmontile_tb.hgcal.txt',mytype=VarParsing.varType.string,
                 info="Path to SiPM-on-tile cell mapper. Absolute, or relative to CMSSW src directory")
#options.register('sipmcells','Geometry/HGCalMapping/data/CellMaps/channels_sipmontile.hgcal.txt',mytype=VarParsing.varType.string,
#                 info="Path to SiPM-on-tile cell mapper. Absolute, or relative to CMSSW src directory")

options.register('offsetfile','Geometry/HGCalMapping/data/CellMaps/calibration_to_surrounding_offsetMap.txt',mytype=VarParsing.varType.string,
                 info="Path to calibration-to-surrounding cell offset file. Absolute, or relative to CMSSW src directory")

options.register('params',
                 #f"{configdir}/level0_calib_Relay1727210224.json",
                 f"{configdir}/TB2026/calib/level0_calib_118361.json",
                 mytype=VarParsing.varType.string,
                 info="Path to calibration parameters (JSON format)")
options.register('energyloss',
                 f"{configdir}/EnergyLoss/hgcal_energyloss_v16.json",
                 mytype=VarParsing.varType.string,
                 info="Path to calibration parameters (JSON format)")

options.register('fedconfig',f"{configdir}/P5/config/config_feds_v1.json",mytype=VarParsing.varType.string,
                 info="Path to configuration (JSON format)")
options.register('modconfig',f"{configdir}/TB2026/config/config_econds_v1.json",mytype=VarParsing.varType.string,   
                 info="Path to configuration (JSON format)")

options.parseArguments()

process = cms.Process("HGC")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.Geometry.GeometryExtendedRun4D104Reco_cff")
process.load("Configuration.Geometry.GeometryExtendedRun4D104_cff")
from Geometry.HGCalMapping.hgcalmapping_cff import customise_hgcalmapper
process = customise_hgcalmapper(process)

kwargs = { k: getattr(options,k) for k in ['modules','sicells','sipmcells','offsetfile'] if getattr(options,k)!='' }
process = customise_hgcalmapper(process, **kwargs)

#from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9 as Era_Phase2
#process = cms.Process('hgcalrechit::HGCalCalibrationESProducer@alpaka',Era_Phase2)

process.hgcalCalibParamESProducer = cms.ESProducer( # ESProducer to load calibration parameters from JSON file, like pedestals
  'hgcalrechit::HGCalCalibrationESProducer@alpaka',
  filename=cms.FileInPath(
        'HGCalCommissioning/Calibrations/TB2026/calib/level0_calib_118361.json'
    ),
  filenameEnergyLoss=cms.FileInPath(
        'HGCalCommissioning/Calibrations/EnergyLoss/hgcal_energyloss_v16.json'
    )
)

process.hgcalConfigESProducer = cms.ESSource( # ESProducer to load configurations for packer
  # https://github.com/CMS-HGCAL/cmssw/blob/dev/hackathon_base_CMSSW_14_1_X/RecoLocalCalo/HGCalRecAlgos/plugins/HGCalConfigurationESProducer.cc
  'HGCalConfigurationESProducer',
  #fedjson=cms.FileInPath(options.fedconfig),  #cms.string(options.fedconfig), # JSON with FED configuration parameters
  #modjson=cms.FileInPath(options.modconfig),      #cms.string(options.modconfig), # JSON with ECON-D configuration parameters
  fedjson=cms.FileInPath('HGCalCommissioning/Calibrations/P5/config/config_feds_v1.json'),
  modjson=cms.FileInPath('HGCalCommissioning/Calibrations/TB2026/config/config_econds_v1.json')
)

process.hgcalECONDZSParameterESProducer = cms.ESProducer(
    "hgcal::HGCalECONDZSParameterESProducer@alpaka",
    
)

# Input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:RAW2DIGI_118368.root"
    )
)


process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('test_RawDataBuffer_118368.root')
)

#process.TFileService = cms.Service(
#    "TFileService",
#    fileName = cms.string("test_out.root")
#)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    

)

process.RawDataBuffer = cms.EDProducer("rawDataProducer_repacked",
   
    hgcalDigis = cms.untracked.InputTag("hgcalDigis","","RAW2DIGI"),
    #hgcalDigis = cms.untracked.InputTag("hgcalDigis","","RAW2DQM"),
    #econds = cms.untracked.InputTag("hgcalDigis","","RAW2DQM")
    #metaData = cms.InputTag("hgcalTrigTimeProducer","","RAW2DQM")
    #calibSource=cms.ESInputTag('hgcalECONDZSParameterESProducer', ''),
)

process.p = cms.Path(process.RawDataBuffer)
process.endPath = cms.EndPath(process.out)


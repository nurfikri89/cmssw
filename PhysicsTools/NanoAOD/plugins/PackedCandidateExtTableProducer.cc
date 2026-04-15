#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/transform.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

#include "DataFormats/TrackReco/interface/Track.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "Calibration/IsolatedParticles/interface/MatrixHCALDetIds.h"
#include "Calibration/IsolatedParticles/interface/CaloPropagateTrack.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/PulseShapeFitOOTPileupCorrection.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalDeterministicFit.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/MahiFit.h"
#include "CalibCalorimetry/HcalAlgos/interface/HcalTimeSlew.h"

#include "CondFormats/HcalObjects/interface/HcalPFCuts.h"
#include "CondFormats/HcalObjects/interface/HcalRespCorrs.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/CaloTopology/interface/HcalTopology.h"


class PackedCandidateExtTableProducer : public edm::stream::EDProducer<> {
public:
  explicit PackedCandidateExtTableProducer(const edm::ParameterSet &);
  ~PackedCandidateExtTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  edm::EDGetTokenT<reco::CandidateView>  pfcands_token_;

  const edm::EDGetTokenT<pat::JetCollection>  jetsToken_;
  const edm::EDGetTokenT<pat::MuonCollection> muonsToken_;

  const edm::EDGetTokenT<pat::PackedCandidateCollection> pc_;
  const edm::EDGetTokenT<edm::Association<reco::PFCandidateCollection>> pc2pf_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersHCALToken_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersHBHEToken_;
  const edm::EDGetTokenT<reco::PFRecHitCollection> pfRecHitsHBHEToken_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersECALToken_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersPSToken_;

  const edm::EDGetTokenT<HBHEChannelInfoCollection> hbheChannelInfoToken_;

  const bool savePFClustersHCAL_;
  const bool saveAllPFClustersHCAL_;
  const bool savePFClustersHBHE_;
  const bool savePFRecHitsHBHE_;
  const bool saveAllPFRecHitsHBHE_;
  const bool saveHBHEChannelInfo_;
  const bool saveMAHIInfo_;
  const bool savePFClustersECAL_;
  const bool savePFClustersPS_;

  const bool matchMuonsWithPFRecHitsHBHE_;



  const std::string name_;
  const bool saveFromPVvertexRef_;
  const int weightPrecision_;

  std::string name_PFClusterHCAL_;
  std::string name_PFClusterHBHE_;
  std::string name_PFRecHitHBHE_;
  std::string name_PFCandToPFClusterHCAL_;
  std::string name_PFCandToPFClusterECAL_;
  std::string name_PFClusterHCALToPFClusterHBHE_;
  std::string name_PFClusterHCALToPFRecHitHBHE_;
  std::string name_PFClusterHBHEToPFRecHitHBHE_;
  std::string name_MuonToPFRecHitHBHE_;

  std::string name_PFClusterECAL_;
  std::string name_PFRecHitEB_;
  std::string name_PFRecHitEE_;

  std::string name_PFClusterPS_;
  std::string name_PFRecHitPS_;

  std::vector<edm::EDGetTokenT<edm::ValueMap<float>>> v_pfcands_weights_tokens_;
  std::vector<std::string> v_weightNames_;
  std::vector<std::string> v_weightDocs_;

  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;
  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> geometryToken_;
  edm::ESGetToken<HcalTopology, HcalRecNumberingRecord> hcalTopologyToken_;
  edm::ESGetToken<HcalTimeSlew, HcalTimeSlewRecord> tokDelay_;
  edm::ESGetToken<HcalPFCuts, HcalPFCutsRcd> hcalPFCutsToken_;
  edm::ESGetToken<HcalRespCorrs, HcalRespCorrsRcd> hcalRespCorrsToken_;
  
  //
  // MAHIrelated
  // https://cmssdt.cern.ch/lxr/source/RecoLocalCalo/HcalRecAlgos/test/MahiDebugger.cc
  //
  bool  mahi_dynamicPed_;
  float mahi_ts4Thresh_;
  float mahi_chiSqSwitch_;
  bool  mahi_applyTimeSlew_;
  HcalTimeSlew::BiasSetting mahi_slewFlavor_;
  double mahi_tsDelay1GeV_ = 0;
  bool mahi_calculateArrivalTime_;
  int mahi_timeAlgo_;
  float mahi_thEnergeticPulses_;
  float mahi_meanTime_;
  float mahi_timeSigmaHPD_;
  float mahi_timeSigmaSiPM_;
  std::vector<int> mahi_activeBXs_;
  int mahi_nMaxItersMin_;
  int mahi_nMaxItersNNLS_;
  float mahi_deltaChiSqThresh_;
  float mahi_nnlsThresh_;

  std::unique_ptr<MahiFit> mahi_;

  const HcalTimeSlew* mahi_hcalTimeSlewDelay;
};

//
// constructors and destructor
//
PackedCandidateExtTableProducer::PackedCandidateExtTableProducer(const edm::ParameterSet &iConfig):
  pfcands_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("srcPFCandidates"))),
  jetsToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("srcJets"))),
  muonsToken_(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("srcMuons"))),
  pc_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
  pc2pf_(consumes<edm::Association<reco::PFCandidateCollection>>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
  pfClustersHCALToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersHCAL"))),
  pfClustersHBHEToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersHBHE"))),
  pfRecHitsHBHEToken_(consumes<std::vector<reco::PFRecHit>>(iConfig.getParameter<edm::InputTag>("PFRecHitsHBHE"))),
  pfClustersECALToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersECAL"))),
  pfClustersPSToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersPS"))),
  hbheChannelInfoToken_(consumes<HBHEChannelInfoCollection>(iConfig.getParameter<edm::InputTag>("hbheChannelInfo"))),
  savePFClustersHCAL_(iConfig.getParameter<bool>("savePFClustersHCAL")),
  saveAllPFClustersHCAL_(iConfig.getParameter<bool>("saveAllPFClustersHCAL")),
  savePFClustersHBHE_(iConfig.getParameter<bool>("savePFClustersHBHE")),
  savePFRecHitsHBHE_(iConfig.getParameter<bool>("savePFRecHitsHBHE")),
  saveAllPFRecHitsHBHE_(iConfig.getParameter<bool>("saveAllPFRecHitsHBHE")),
  saveHBHEChannelInfo_(iConfig.getParameter<bool>("saveHBHEChannelInfo")),
  saveMAHIInfo_(iConfig.getParameter<bool>("saveMAHIInfo")),
  savePFClustersECAL_(iConfig.getParameter<bool>("savePFClustersECAL")),
  savePFClustersPS_(iConfig.getParameter<bool>("savePFClustersPS")),
  matchMuonsWithPFRecHitsHBHE_(iConfig.getParameter<bool>("matchMuonsWithPFRecHitsHBHE")),
  name_(iConfig.getParameter<std::string>("name")),
  saveFromPVvertexRef_(iConfig.getParameter<bool>("saveFromPVvertexRef")),
  weightPrecision_(iConfig.getParameter<int>("weightPrecision")),
  //
  mahi_dynamicPed_(iConfig.getParameter<bool>("mahi_dynamicPed")),
  mahi_ts4Thresh_(iConfig.getParameter<double>("mahi_ts4Thresh")),
  mahi_chiSqSwitch_(iConfig.getParameter<double>("mahi_chiSqSwitch")),
  mahi_applyTimeSlew_(iConfig.getParameter<bool>("mahi_applyTimeSlew")),
  mahi_calculateArrivalTime_(iConfig.getParameter<bool>("mahi_calculateArrivalTime")),
  mahi_timeAlgo_(iConfig.getParameter<int>("mahi_timeAlgo")),
  mahi_thEnergeticPulses_(iConfig.getParameter<double>("mahi_thEnergeticPulses")),
  mahi_meanTime_(iConfig.getParameter<double>("mahi_meanTime")),
  mahi_timeSigmaHPD_(iConfig.getParameter<double>("mahi_timeSigmaHPD")),
  mahi_timeSigmaSiPM_(iConfig.getParameter<double>("mahi_timeSigmaSiPM")),
  mahi_activeBXs_(iConfig.getParameter<std::vector<int>>("mahi_activeBXs")),
  mahi_nMaxItersMin_(iConfig.getParameter<int>("mahi_nMaxItersMin")),
  mahi_nMaxItersNNLS_(iConfig.getParameter<int>("mahi_nMaxItersNNLS")),
  mahi_deltaChiSqThresh_(iConfig.getParameter<double>("mahi_deltaChiSqThresh")),
  mahi_nnlsThresh_(iConfig.getParameter<double>("mahi_nnlsThresh"))
{
  v_pfcands_weights_tokens_ = edm::vector_transform(
    iConfig.getParameter<std::vector<edm::InputTag>>("srcWeightsV"),
    [this](edm::InputTag const& tag) {return consumes<edm::ValueMap<float>>(tag);}
  );
  v_weightNames_ = iConfig.getParameter<std::vector<std::string>>("weightNamesV");
  v_weightDocs_ = iConfig.getParameter<std::vector<std::string>>("weightDocsV");

  produces<nanoaod::FlatTable>(name_);

  name_PFClusterHCAL_ = "PFClusterHCAL";
  name_PFClusterHBHE_ = "PFClusterHBHE";
  name_PFRecHitHBHE_ = "PFRecHitHBHE";
  name_PFCandToPFClusterHCAL_ = "PFCandToPFClusterHCAL";
  name_PFCandToPFClusterECAL_ = "PFCandToPFClusterECAL";
  name_PFClusterHCALToPFClusterHBHE_ = "PFClusterHCALToPFClusterHBHE";
  name_PFClusterHCALToPFRecHitHBHE_ = "PFClusterHCALToPFRecHitHBHE";
  name_PFClusterHBHEToPFRecHitHBHE_ = "PFClusterHBHEToPFRecHitHBHE";
  name_MuonToPFRecHitHBHE_ = "MuonToPFRecHitHBHBE";

  name_PFClusterECAL_ = "PFClusterECAL";
  name_PFRecHitEB_ = "PFRecHitEB";
  name_PFRecHitEE_ = "PFRecHitEE";

  name_PFClusterPS_ = "PFClusterPS";
  name_PFRecHitPS_ = "PFRecHitPS";

  if(savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFClusterHCAL_);
  if(savePFClustersHBHE_)
    produces<nanoaod::FlatTable>(name_PFClusterHBHE_);
  if (savePFRecHitsHBHE_){
    produces<nanoaod::FlatTable>(name_PFRecHitHBHE_);
    if (saveHBHEChannelInfo_){
      produces<nanoaod::FlatTable>("HBHEChannelInfo");
    }
  }
  if(savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFCandToPFClusterHCAL_);
  // if(savePFClustersHCAL_ && savePFClustersHBHE_)
  //   produces<nanoaod::FlatTable>(name_PFClusterHCALToPFClusterHBHE_);
  if (savePFRecHitsHBHE_ && savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFClusterHCALToPFRecHitHBHE_);
  if (savePFRecHitsHBHE_ && savePFClustersHBHE_)
    produces<nanoaod::FlatTable>(name_PFClusterHBHEToPFRecHitHBHE_);
  if (savePFRecHitsHBHE_ && matchMuonsWithPFRecHitsHBHE_){
    produces<nanoaod::FlatTable>(name_MuonToPFRecHitHBHE_);
    produces<nanoaod::FlatTable>("Muon");
  }
  if(savePFClustersECAL_)
    produces<nanoaod::FlatTable>(name_PFClusterECAL_);
  if(savePFClustersECAL_)
    produces<nanoaod::FlatTable>(name_PFCandToPFClusterECAL_);
  if (savePFClustersPS_)
    produces<nanoaod::FlatTable>(name_PFClusterPS_);

  // produces<nanoaod::FlatTable>(name_PFRecHitEB_);
  // produces<nanoaod::FlatTable>(name_PFRecHitEE_);

  // produces<nanoaod::FlatTable>(name_PFClusterPS_);
  // produces<nanoaod::FlatTable>(name_PFRecHitPS_);

  magneticFieldToken_ = esConsumes<MagneticField, IdealMagneticFieldRecord>();
  geometryToken_      = esConsumes<CaloGeometry, CaloGeometryRecord>();
  hcalTopologyToken_  = esConsumes<HcalTopology, HcalRecNumberingRecord>();
  tokDelay_           = esConsumes<HcalTimeSlew, HcalTimeSlewRecord>(edm::ESInputTag("", "HBHE"));

  hcalPFCutsToken_    = esConsumes<HcalPFCuts, HcalPFCutsRcd>(edm::ESInputTag("", "withTopo"));
  hcalRespCorrsToken_ = esConsumes<HcalRespCorrs, HcalRespCorrsRcd>(edm::ESInputTag("", "withTopo"));

  mahi_ = std::make_unique<MahiFit>();
  mahi_->setParameters(mahi_dynamicPed_,
                       mahi_ts4Thresh_,
                       mahi_chiSqSwitch_,
                       mahi_applyTimeSlew_,
                       HcalTimeSlew::Medium,
                       mahi_calculateArrivalTime_,
                       mahi_timeAlgo_,
                       mahi_thEnergeticPulses_,
                       mahi_meanTime_,
                       mahi_timeSigmaHPD_,
                       mahi_timeSigmaSiPM_,
                       mahi_activeBXs_,
                       mahi_nMaxItersMin_,
                       mahi_nMaxItersNNLS_,
                       mahi_deltaChiSqThresh_,
                       mahi_nnlsThresh_);

}

PackedCandidateExtTableProducer::~PackedCandidateExtTableProducer() {}

void PackedCandidateExtTableProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {

  // packedPFCandidate collection
  edm::Handle<pat::PackedCandidateCollection> pc_h;
  iEvent.getByToken(pc_, pc_h);

  // packedPFCandidates-->particleFlow(reco::PFCandidate) association
  edm::Handle<edm::Association<reco::PFCandidateCollection>> pc2pf;
  iEvent.getByToken(pc2pf_, pc2pf);

  // Selected pf candidates
  edm::Handle<reco::CandidateView> pfCands;
  iEvent.getByToken(pfcands_token_, pfCands);

  // Retrieve selected pf candidates to 
  std::vector<edm::Ptr<reco::Candidate>> candPtrs;
  candPtrs.reserve(pfCands->size());
  for (size_t iCand = 0; iCand < pfCands->size(); ++iCand) {
    candPtrs.push_back(pfCands->ptrAt(iCand));
  }

  std::vector<edm::ValueMap<float>> v_pfcands_weights;
  for (size_t i = 0; i < v_pfcands_weights_tokens_.size(); ++i) {
    v_pfcands_weights.push_back(iEvent.get(v_pfcands_weights_tokens_[i]));
  }

  //
  //
  //
  std::vector<std::vector<float>> v_v_weightsOut;
  v_v_weightsOut.reserve(v_pfcands_weights.size());
  for (size_t iW = 0; iW < v_pfcands_weights.size(); ++iW) {
    std::vector<float> weightsOut;
    weightsOut.reserve(pfCands->size());
    v_v_weightsOut.push_back(weightsOut);
  }

  //
  //
  //
  std::vector<int> fromPV_vertexRefOutVec;
  fromPV_vertexRefOutVec.reserve(pfCands->size());

  //
  //
  //
  std::vector<float> PFCand_ptRECO;   PFCand_ptRECO.reserve(pfCands->size());
  std::vector<float> PFCand_etaRECO;  PFCand_etaRECO.reserve(pfCands->size());
  std::vector<float> PFCand_phiRECO;  PFCand_phiRECO.reserve(pfCands->size());
  std::vector<float> PFCand_massRECO; PFCand_massRECO.reserve(pfCands->size());
  std::vector<float> PFCand_energyRECO; PFCand_energyRECO.reserve(pfCands->size());
  std::vector<unsigned int> PFCand_birthId;PFCand_birthId.reserve(pfCands->size());

  std::vector<float> PFCand_hcalDepthEnergyFraction1; PFCand_hcalDepthEnergyFraction1.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction2; PFCand_hcalDepthEnergyFraction2.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction3; PFCand_hcalDepthEnergyFraction3.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction4; PFCand_hcalDepthEnergyFraction4.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction5; PFCand_hcalDepthEnergyFraction5.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction6; PFCand_hcalDepthEnergyFraction6.reserve(pfCands->size());
  std::vector<float> PFCand_hcalDepthEnergyFraction7; PFCand_hcalDepthEnergyFraction7.reserve(pfCands->size());
  std::vector<float> PFCand_ecalEnergy;   PFCand_ecalEnergy.reserve(pfCands->size());
  std::vector<float> PFCand_rawEcalEnergy;PFCand_rawEcalEnergy.reserve(pfCands->size());
  std::vector<float> PFCand_hcalEnergy;   PFCand_hcalEnergy.reserve(pfCands->size());
  std::vector<float> PFCand_rawHcalEnergy;PFCand_rawHcalEnergy.reserve(pfCands->size());
  std::vector<float> PFCand_hoEnergy;     PFCand_hoEnergy.reserve(pfCands->size());
  std::vector<float> PFCand_rawHoEnergy;  PFCand_rawHoEnergy.reserve(pfCands->size());

  std::vector<int> PFCand_blockIdx;PFCand_blockIdx.reserve(pfCands->size());

  std::vector<int> PFCand_nPFTrack;PFCand_nPFTrack.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterHCAL;PFCand_nPFClusterHCAL.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterECAL;PFCand_nPFClusterECAL.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterPS;PFCand_nPFClusterPS.reserve(pfCands->size());

  std::vector<int> PFCand_nPFTrackInBlock;PFCand_nPFTrackInBlock.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterHCALInBlock;PFCand_nPFClusterHCALInBlock.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterECALInBlock;PFCand_nPFClusterECALInBlock.reserve(pfCands->size());
  std::vector<int> PFCand_nPFClusterPSInBlock;PFCand_nPFClusterPSInBlock.reserve(pfCands->size());

  // std::set<int> SelectedPFTrack_keys;

  //
  // We will keep track of which PFClusters to keep by storing the keys
  // to each PFCluster object in a set.
  //
  std::set<int> SelectedPFClusterHCAL_keys;
  std::set<int> SelectedPFClusterECAL_keys;
  std::set<int> SelectedPFClusterPS_keys;


  //first: PFCandIdx
  //second.first: PFCluster keys
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterHCAL;
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterECAL;
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterPS;
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterHCALInBlock;
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterECALInBlock;
  std::vector<std::pair<size_t, int >> MappingInfo_PFCand_To_PFClusterPSInBlock;

  //
  // Loop over ptr-candidate collection
  //
  for (size_t iPtr = 0; iPtr < candPtrs.size(); ++iPtr) {
    auto candPtr = candPtrs[iPtr];
    //
    // For each ptr-candidate collection, save all
    //
    for (size_t iW = 0; iW < v_pfcands_weights.size(); ++iW) {
      v_v_weightsOut[iW].push_back(v_pfcands_weights[iW][candPtr]);
    }
    //
    //
    //
    const reco::Candidate* cand = candPtr.get();
    const pat::PackedCandidate* packedCand = dynamic_cast<const pat::PackedCandidate*>(cand);

    if(saveFromPVvertexRef_){
      int fromPV_vertexRef = -1;
      if (packedCand != nullptr && packedCand->vertexRef().isNonnull())
        fromPV_vertexRef = packedCand->fromPV(packedCand->vertexRef().key());
      fromPV_vertexRefOutVec.push_back(fromPV_vertexRef);
    }

    //
    //
    //
    pat::PackedCandidateRef pcref = pat::PackedCandidateRef(pc_h, candPtr.key());
    reco::PFCandidateRef pfref = (*pc2pf)[pcref];

    PFCand_hcalDepthEnergyFraction1.push_back(pfref.get()->hcalDepthEnergyFraction(1));
    PFCand_hcalDepthEnergyFraction2.push_back(pfref.get()->hcalDepthEnergyFraction(2));
    PFCand_hcalDepthEnergyFraction3.push_back(pfref.get()->hcalDepthEnergyFraction(3));
    PFCand_hcalDepthEnergyFraction4.push_back(pfref.get()->hcalDepthEnergyFraction(4));
    PFCand_hcalDepthEnergyFraction5.push_back(pfref.get()->hcalDepthEnergyFraction(5));
    PFCand_hcalDepthEnergyFraction6.push_back(pfref.get()->hcalDepthEnergyFraction(6));
    PFCand_hcalDepthEnergyFraction7.push_back(pfref.get()->hcalDepthEnergyFraction(7));

    PFCand_ptRECO.push_back(pfref.get()->pt());
    PFCand_etaRECO.push_back(pfref.get()->eta());
    PFCand_phiRECO.push_back(pfref.get()->phi());
    PFCand_massRECO.push_back(pfref.get()->mass());
    PFCand_energyRECO.push_back(pfref.get()->energy());

    PFCand_birthId.push_back(pfref.get()->getRecoLocationIdx());

    PFCand_ecalEnergy.push_back(pfref.get()->ecalEnergy());
    PFCand_rawEcalEnergy.push_back(pfref.get()->rawEcalEnergy());
    PFCand_hcalEnergy.push_back(pfref.get()->hcalEnergy());
    PFCand_rawHcalEnergy.push_back(pfref.get()->rawHcalEnergy());
    PFCand_hoEnergy.push_back(pfref.get()->hoEnergy());
    PFCand_rawHoEnergy.push_back(pfref.get()->rawHoEnergy());

    //
    //
    //
    // Get the list of elements that this PF candidate consist of
    const reco::PFCandidate::ElementsInBlocks& pfElements = pfref.get()->elementsInBlocks();
    //
    // Get the original block where this pfCandidate comes from and the elements in the block
    //
    int pfCand_blockIdx = -1;

    unsigned int nTrack = 0;
    unsigned int nClusterHCAL = 0;
    unsigned int nClusterECAL = 0;
    unsigned int nClusterPS = 0;

    unsigned int nTrackInBlock = 0;
    unsigned int nClusterHCALInBlock = 0;
    unsigned int nClusterECALInBlock = 0;
    unsigned int nClusterPSInBlock = 0;

    if (pfElements.size() >= 1){
      pfCand_blockIdx = pfElements[0].first.index();
      const reco::PFBlockRef pfBlockRef = pfElements[0].first;
      const edm::OwnVector<reco::PFBlockElement>& elementsInOriBlock = pfBlockRef->elements();
      reco::PFBlock::LinkData linkData =  pfBlockRef->linkData();

      //
      // Loop over elements in the block where it comes from
      //
      for(unsigned int eBlock=0; eBlock < elementsInOriBlock.size(); eBlock++) {
        reco::PFBlockElement::Type typeInBlock = elementsInOriBlock[eBlock].type();
        switch( typeInBlock ) {
          case reco::PFBlockElement::TRACK:
            nTrackInBlock++;
            // SelectedPFTrack_keys.insert(elementsInOriBlock[eBlock].trackRef().key());
            break;
          case reco::PFBlockElement::HCAL:
            nClusterHCALInBlock++;
            MappingInfo_PFCand_To_PFClusterHCALInBlock.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
            SelectedPFClusterHCAL_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
            break;
          case reco::PFBlockElement::ECAL:
            nClusterECALInBlock++;
            MappingInfo_PFCand_To_PFClusterECALInBlock.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
            SelectedPFClusterECAL_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
            break;
          case reco::PFBlockElement::PS1:
          case reco::PFBlockElement::PS2:
            nClusterPSInBlock++;
            MappingInfo_PFCand_To_PFClusterPSInBlock.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
            SelectedPFClusterPS_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
            break;
          default:
            break;
        }
        //
        // For a given element in the block, check if this corresponds to the element
        // that make up this PFcandidate
        //
        for(unsigned int e=0; e < pfElements.size(); e++) {
          if (elementsInOriBlock[eBlock].index() ==  pfElements[e].second){
            switch( typeInBlock ) {
              case reco::PFBlockElement::TRACK:
                nTrack++;
                // SelectedPFTrack_keys.insert(elementsInOriBlock[eBlock].trackRef().key());
                break;
              case reco::PFBlockElement::HCAL:
                nClusterHCAL++;
                MappingInfo_PFCand_To_PFClusterHCAL.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
                SelectedPFClusterHCAL_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
                break;
              case reco::PFBlockElement::ECAL:
                nClusterECAL++;
                MappingInfo_PFCand_To_PFClusterECAL.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
                SelectedPFClusterECAL_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
                break;
              case reco::PFBlockElement::PS1:
              case reco::PFBlockElement::PS2:
                nClusterPS++;
                MappingInfo_PFCand_To_PFClusterPS.push_back(std::make_pair(iPtr,elementsInOriBlock[eBlock].clusterRef().key()));
                SelectedPFClusterPS_keys.insert(elementsInOriBlock[eBlock].clusterRef().key());
                break;
              default:
                break;
            }
          }
        }
      }
    }
    PFCand_blockIdx.push_back(pfCand_blockIdx);
    PFCand_nPFTrack.push_back(nTrack);
    PFCand_nPFClusterHCAL.push_back(nClusterHCAL);
    PFCand_nPFClusterECAL.push_back(nClusterECAL);
    PFCand_nPFClusterPS.push_back(nClusterPS);

    PFCand_nPFTrackInBlock.push_back(nTrackInBlock);
    PFCand_nPFClusterHCALInBlock.push_back(nClusterHCALInBlock);
    PFCand_nPFClusterECALInBlock.push_back(nClusterECALInBlock);
    PFCand_nPFClusterPSInBlock.push_back(nClusterPSInBlock);
  }

  //==========================================
  //
  // PFClusterHCAL
  //
  //==========================================
  unsigned int nPFClusterHCAL=0;
  std::vector<float>          PFClusterHCAL_pt;
  std::vector<float>          PFClusterHCAL_energy;
  std::vector<float>          PFClusterHCAL_eta;
  std::vector<float>          PFClusterHCAL_phi;
  std::vector<int>            PFClusterHCAL_layer;
  std::vector<int>            PFClusterHCAL_nhits;
  std::vector<unsigned int>   PFClusterHCAL_seedhit_detId;
  std::vector<int>            PFClusterHCAL_key;

  edm::Handle<reco::PFClusterCollection> pfClustersHCALHandle;
  iEvent.getByToken(pfClustersHCALToken_, pfClustersHCALHandle);

  //
  //
  //
  if (saveAllPFClustersHCAL_){
    for (int key = 0; key < (int)pfClustersHCALHandle->size(); key++) {
      SelectedPFClusterHCAL_keys.insert(key);
    }
  }

  //
  // We will keep track of which PFRecHit to keep by storing the detId
  // of each PFRecHit object in a set.
  //
  std::set<uint32_t> SelectedPFRecHitHBHE_rawDetId;

  //first: SelectedPFClusterIdx
  //second.first: DetId
  //second.second: RecHit energy fraction to the PFCluster
  std::vector<std::pair<int,std::pair<unsigned int,float>>> MappingInfo_PFClusterHCAL_To_PFRecHit;

  //first: SelectedPFClusterIdx
  //second.first: DetId
  //second.second: RecHit energy fraction to the PFCluster
  std::vector<std::pair<int,unsigned int>> MappingInfo_PFClusterHCAL_To_PFClusterHBHE;

  if (savePFClustersHCAL_){
    for (const int key : SelectedPFClusterHCAL_keys) {
      reco::PFClusterRef clusterRef( pfClustersHCALHandle, key );

      PFClusterHCAL_pt.push_back(clusterRef->pt());
      PFClusterHCAL_energy.push_back(clusterRef->energy());
      PFClusterHCAL_eta.push_back(clusterRef->eta());
      PFClusterHCAL_phi.push_back(clusterRef->phi());
      PFClusterHCAL_layer.push_back(clusterRef->layer());
      PFClusterHCAL_seedhit_detId.push_back(clusterRef->seed());
      PFClusterHCAL_key.push_back(key);
      const std::vector<std::pair<DetId, float> >& clusterHits = clusterRef->hitsAndFractions();
      PFClusterHCAL_nhits.push_back(clusterHits.size());

      // Loop over PFRecHits
      for (size_t ihit=0; ihit < clusterHits.size(); ++ihit){
        SelectedPFRecHitHBHE_rawDetId.insert(clusterHits[ihit].first);
        MappingInfo_PFClusterHCAL_To_PFRecHit.push_back(
          std::make_pair(PFClusterHCAL_nhits.size()-1, std::make_pair(clusterHits[ihit].first,clusterHits[ihit].second))
        );
      }

      //Loop over parent PFClusters in PFClustersHBHE (This can be completely bogus)
      // const std::set<int>& parentClusterKeys = clusterRef->parentClusterKeys();
      // for (auto ikey = parentClusterKeys.begin(); ikey != parentClusterKeys.end(); ++ikey) {
      //   MappingInfo_PFClusterHCAL_To_PFClusterHBHE.push_back(
      //     std::make_pair(PFClusterHCAL_nhits.size()-1, *ikey)
      //   );
      // }
      nPFClusterHCAL++;
    }
  }

  //==================================================
  // Make branches for PFCand <-> PFCluster mapping
  //==================================================
  unsigned int      nPFCandToPFClusterHCAL=0;
  std::vector<int>  PFCandToPFClusterHCAL_PFCandIdx;
  std::vector<int>  PFCandToPFClusterHCAL_PFClusterIdx;

  for (size_t i = 0; i < MappingInfo_PFCand_To_PFClusterHCAL.size(); ++i) {
    int PFCandIdx = MappingInfo_PFCand_To_PFClusterHCAL[i].first;
    int PFClusterKey = MappingInfo_PFCand_To_PFClusterHCAL[i].second;
    //
    int PFClusterIdx = -1;
    auto it = std::find(PFClusterHCAL_key.begin(), PFClusterHCAL_key.end(), PFClusterKey);
    if (it != PFClusterHCAL_key.end()) {
      PFClusterIdx = std::distance(PFClusterHCAL_key.begin(), it);
    }
    PFCandToPFClusterHCAL_PFCandIdx.push_back(PFCandIdx);
    PFCandToPFClusterHCAL_PFClusterIdx.push_back(PFClusterIdx);
    nPFCandToPFClusterHCAL++;
  }

  //==========================================
  //
  // PFClusterHBHE
  // NOTE: This part here can be completely bogus.
  // I hacked my way with PFCluster with the intention
  // that I am able to store key of the PFClustersHBHE that
  // are used for PFClustersHCAL (Multidepth clusterint)
  //
  //==========================================
  unsigned int nPFClusterHBHE=0;
  std::vector<float>          PFClusterHBHE_pt;
  std::vector<float>          PFClusterHBHE_energy;
  std::vector<float>          PFClusterHBHE_eta;
  std::vector<float>          PFClusterHBHE_phi;
  std::vector<int>            PFClusterHBHE_layer;
  std::vector<int>            PFClusterHBHE_nhits;
  std::vector<unsigned int>   PFClusterHBHE_seedhit_detId;
  std::vector<int>            PFClusterHBHE_key;

  edm::Handle<reco::PFClusterCollection> pfClustersHBHEHandle;
  iEvent.getByToken(pfClustersHBHEToken_, pfClustersHBHEHandle);

  //first: SelectedPFClusterIdx
  //second.first: DetId
  //second.second: RecHit energy fraction to the PFCluster
  std::vector<std::pair<int,std::pair<unsigned int,float>>> MappingInfo_PFClusterHBHE_To_PFRecHit;

  unsigned int nPFClusterHCALToPFClusterHBHBE=0;
  std::vector<int>   PFClusterHCALToPFClusterHBHE_PFClusterHCALIdx;
  std::vector<int>   PFClusterHCALToPFClusterHBHE_PFClusterHBHEIdx;

  if (savePFClustersHBHE_){
    for (int key = 0; key < (int)pfClustersHBHEHandle->size(); key++) {
      reco::PFClusterRef clusterRef( pfClustersHBHEHandle, key );

      PFClusterHBHE_pt.push_back(clusterRef->pt());
      PFClusterHBHE_energy.push_back(clusterRef->energy());
      PFClusterHBHE_eta.push_back(clusterRef->eta());
      PFClusterHBHE_phi.push_back(clusterRef->phi());
      PFClusterHBHE_layer.push_back(clusterRef->layer());
      PFClusterHBHE_seedhit_detId.push_back(clusterRef->seed());
      PFClusterHBHE_key.push_back(key);
      const std::vector<std::pair<DetId, float> >& clusterHits = clusterRef->hitsAndFractions();
      PFClusterHBHE_nhits.push_back(clusterHits.size());

      // Loop over PFRecHits
      for (size_t ihit=0; ihit < clusterHits.size(); ++ihit){
        SelectedPFRecHitHBHE_rawDetId.insert(clusterHits[ihit].first);
        MappingInfo_PFClusterHBHE_To_PFRecHit.push_back(
          std::make_pair(PFClusterHBHE_nhits.size()-1, std::make_pair(clusterHits[ihit].first,clusterHits[ihit].second))
        );
      }
      nPFClusterHBHE++;
    }

    //==================================================
    // Make branches for PFClusterHCAL <-> PFClusterHBHE mapping
    //======================================================
    // for (size_t i = 0; i < MappingInfo_PFClusterHCAL_To_PFClusterHBHE.size(); ++i) {
    //   int PFClusterHCALIdx = MappingInfo_PFClusterHCAL_To_PFClusterHBHE[i].first;
    //   int PFClusterHBHEKey = MappingInfo_PFClusterHCAL_To_PFClusterHBHE[i].second;
    //   //
    //   int PFClusterHBHEIdx = -1;
    //   auto it = std::find(PFClusterHBHE_key.begin(), PFClusterHBHE_key.end(), PFClusterHBHEKey);
    //   if (it != PFClusterHBHE_key.end()) {
    //     PFClusterHBHEIdx = std::distance(PFClusterHBHE_key.begin(), it);
    //   }
    //   PFClusterHCALToPFClusterHBHE_PFClusterHCALIdx.push_back(PFClusterHCALIdx);
    //   PFClusterHCALToPFClusterHBHE_PFClusterHBHEIdx.push_back(PFClusterHBHEIdx);
    //   nPFClusterHCALToPFClusterHBHBE++;
    // }
  }

  //==========================================
  //
  // HCAL Rechits in jet cone
  //
  //==========================================
  // edm::Handle<reco::CandidateView> jets;
  // iEvent.getByToken(jetsToken_, jets);
  // unsigned int nJet = jets->size();
  // for (unsigned int ijet = 0; ijet < nJet; ijet++) {
  //   const pat::Jet &itJet = (*jets)[ijet];
  //   std::vector<DetId> matrixHCALIds(const DetId& det,
  //                                   const CaloGeometry* geo,
  //                                   const HcalTopology* topology,
  //                                   double dR,
  //                                   const GlobalVector& trackMom,
  //                                   bool includeHO,
  //                                   bool debug)
  // }

  //==========================================
  //
  // HCAL Rechits associated to muons
  //
  //==========================================
  //first: Muon index
  //second.first:  DetId of associated rechits
  //second.second: is closest rechit
  std::vector<std::pair<int,std::pair<DetId,bool>>> MappingInfo_Muon_To_PFRecHit;

  unsigned int nMuons = 0;

  if (savePFRecHitsHBHE_ && matchMuonsWithPFRecHitsHBHE_){
    const CaloGeometry* geometry = &iSetup.getData(geometryToken_);
    const MagneticField* bField = &iSetup.getData(magneticFieldToken_);
    const HcalTopology* theHBHETopology = &iSetup.getData(hcalTopologyToken_);

    edm::Handle<pat::MuonCollection> muons;
    iEvent.getByToken(muonsToken_, muons);
    nMuons = muons->size();
    for (unsigned int imuon = 0; imuon < nMuons; imuon++) {
      const pat::Muon& itMuon = (*muons)[imuon];

      reco::TrackRef muonTrack = itMuon.innerTrack();
      const reco::Track* pTrack = muonTrack.get();
      if (pTrack){
        spr::propagatedTrackID trackID = spr::propagateCALO(pTrack, geometry, bField, /*debug=*/ false );
        if (trackID.okHCAL) {
          const DetId closestCell(trackID.detIdHCAL);
          std::vector<DetId> dets(1, closestCell);
          std::vector<DetId> vdets = spr::matrixHCALIds(dets, theHBHETopology, /*ieta=*/ 1,  /*iphi=*/ 1, false, /*debug=*/ false);
          for (unsigned int detsIdx = 0; detsIdx < vdets.size(); detsIdx++) {
            bool isClosest = vdets[detsIdx] == closestCell;
            MappingInfo_Muon_To_PFRecHit.push_back(
              std::make_pair(imuon,std::make_pair(vdets[detsIdx],isClosest))
            );
            SelectedPFRecHitHBHE_rawDetId.insert(vdets[detsIdx]);
          }
        }
      }
    }
  }

  //==========================================
  //
  // PFRecHits HCAL: HBHE
  //
  //==========================================
  unsigned int nPFRecHitHBHE=0;
  std::vector<float>          PFRecHitHBHE_energy;
  std::vector<float>          PFRecHitHBHE_time;
  std::vector<int>            PFRecHitHBHE_ieta;
  std::vector<int>            PFRecHitHBHE_iphi;
  std::vector<int>            PFRecHitHBHE_depth;
  std::vector<unsigned int>   PFRecHitHBHE_detId;
  std::vector<float>          PFRecHitHBHE_pfCutThresh;
  std::vector<float>          PFRecHitHBHE_respCorr;

  unsigned int nPFClusterHCALToPFRecHitHBHBE=0;
  std::vector<int>   PFClusterHCALToPFRecHitHBHBE_PFClusterHCALIdx;
  std::vector<int>   PFClusterHCALToPFRecHitHBHBE_PFRecHitHBHEIdx;
  std::vector<float> PFClusterHCALToPFRecHitHBHBE_fraction;

  unsigned int nPFClusterHBHEToPFRecHitHBHBE=0;
  std::vector<int>   PFClusterHBHEToPFRecHitHBHBE_PFClusterHBHEIdx;
  std::vector<int>   PFClusterHBHEToPFRecHitHBHBE_PFRecHitHBHEIdx;
  std::vector<float> PFClusterHBHEToPFRecHitHBHBE_fraction;

  unsigned int nMuonToPFRecHitHBHBE=0;
  std::vector<int>   MuonToPFRecHitHBHBE_MuonIdx;
  std::vector<int>   MuonToPFRecHitHBHBE_PFRecHitHBHEIdx;

  std::vector<int>   Muon_ClosestPFRecHitHBHEIdx;
  std::vector<unsigned int>   Muon_ClosestPFRecHitHBHEDetId;

  if (savePFRecHitsHBHE_){
    const HcalPFCuts* hcalPFCuts = &iSetup.getData(hcalPFCutsToken_);
    const HcalRespCorrs* hcalRespCorrs = &iSetup.getData(hcalRespCorrsToken_);

    edm::Handle<std::vector<reco::PFRecHit>> pfRecHitsHBHEHandle;
    iEvent.getByToken(pfRecHitsHBHEToken_, pfRecHitsHBHEHandle);
    auto pfRecHitsHBHE = pfRecHitsHBHEHandle.product();

    // std::cout << " pfRecHitsHBHE->size() = " << pfRecHitsHBHE->size() << std::endl;
    for (size_t idx = 0; idx < pfRecHitsHBHE->size(); idx++){
      reco::PFRecHitRef pfrechitRef( pfRecHitsHBHEHandle, idx);
      if (!saveAllPFRecHitsHBHE_){
        if (SelectedPFRecHitHBHE_rawDetId.find(pfrechitRef.get()->detId()) == SelectedPFRecHitHBHE_rawDetId.end())
          continue;
      }
      HcalDetId theHcalDetId(pfrechitRef.get()->detId());
      PFRecHitHBHE_energy.push_back(pfrechitRef.get()->energy());
      PFRecHitHBHE_time.push_back(pfrechitRef.get()->time());
      PFRecHitHBHE_ieta.push_back(theHcalDetId.ieta());
      PFRecHitHBHE_iphi.push_back(theHcalDetId.iphi());
      PFRecHitHBHE_depth.push_back(theHcalDetId.depth());
      PFRecHitHBHE_detId.push_back(pfrechitRef.get()->detId());
      const HcalPFCut* cutValue = hcalPFCuts->getValues(pfrechitRef.get()->detId());
      float thresholdE = cutValue->noiseThreshold();
      PFRecHitHBHE_pfCutThresh.push_back(thresholdE);
      float respCorr = 1.0f;
      if (hcalRespCorrs->exists(pfrechitRef.get()->detId()))
        respCorr = hcalRespCorrs->getValues(pfrechitRef.get()->detId())->getValue();
      PFRecHitHBHE_respCorr.push_back(respCorr);
      nPFRecHitHBHE++;
    }
    //==================================================
    // Make branches for PFRecHit <-> PFClusterHCAL mapping
    //==================================================
    for (size_t i = 0; i < MappingInfo_PFClusterHCAL_To_PFRecHit.size(); ++i) {
      int PFClusterIdx = MappingInfo_PFClusterHCAL_To_PFRecHit[i].first;
      int PFRecHitDetId = MappingInfo_PFClusterHCAL_To_PFRecHit[i].second.first;
      float fraction = MappingInfo_PFClusterHCAL_To_PFRecHit[i].second.second;
      //
      int PFRecHitIdx = -1;
      auto it = std::find(PFRecHitHBHE_detId.begin(), PFRecHitHBHE_detId.end(), PFRecHitDetId);
      if (it != PFRecHitHBHE_detId.end()) {
        PFRecHitIdx = std::distance(PFRecHitHBHE_detId.begin(), it);
      }
      PFClusterHCALToPFRecHitHBHBE_PFClusterHCALIdx.push_back(PFClusterIdx);
      PFClusterHCALToPFRecHitHBHBE_PFRecHitHBHEIdx.push_back(PFRecHitIdx);
      PFClusterHCALToPFRecHitHBHBE_fraction.push_back(fraction);
      nPFClusterHCALToPFRecHitHBHBE++;
    }

    //==================================================
    // Make branches for PFRecHit <-> PFClusterHBHE mapping
    //==================================================
    for (size_t i = 0; i < MappingInfo_PFClusterHBHE_To_PFRecHit.size(); ++i) {
      int PFClusterIdx = MappingInfo_PFClusterHBHE_To_PFRecHit[i].first;
      int PFRecHitDetId = MappingInfo_PFClusterHBHE_To_PFRecHit[i].second.first;
      float fraction = MappingInfo_PFClusterHBHE_To_PFRecHit[i].second.second;
      //
      int PFRecHitIdx = -1;
      auto it = std::find(PFRecHitHBHE_detId.begin(), PFRecHitHBHE_detId.end(), PFRecHitDetId);
      if (it != PFRecHitHBHE_detId.end()) {
        PFRecHitIdx = std::distance(PFRecHitHBHE_detId.begin(), it);
      }
      PFClusterHBHEToPFRecHitHBHBE_PFClusterHBHEIdx.push_back(PFClusterIdx);
      PFClusterHBHEToPFRecHitHBHBE_PFRecHitHBHEIdx.push_back(PFRecHitIdx);
      PFClusterHBHEToPFRecHitHBHBE_fraction.push_back(fraction);
      nPFClusterHBHEToPFRecHitHBHBE++;
    }

    //==================================================
    // Make branches for PFRecHit <-> Muon mapping
    //==================================================
    if(matchMuonsWithPFRecHitsHBHE_){
      Muon_ClosestPFRecHitHBHEIdx.assign(nMuons,-1);
      Muon_ClosestPFRecHitHBHEDetId.assign(nMuons,0);
      for (size_t i = 0; i < MappingInfo_Muon_To_PFRecHit.size(); ++i) {
        int MuonIdx = MappingInfo_Muon_To_PFRecHit[i].first;
        int PFRecHitDetId = MappingInfo_Muon_To_PFRecHit[i].second.first;
        int isClosest = MappingInfo_Muon_To_PFRecHit[i].second.second;
        //
        int PFRecHitIdx = -1;
        auto it = std::find(PFRecHitHBHE_detId.begin(), PFRecHitHBHE_detId.end(), PFRecHitDetId);
        if (it != PFRecHitHBHE_detId.end()) {
          PFRecHitIdx = std::distance(PFRecHitHBHE_detId.begin(), it);
        }
        MuonToPFRecHitHBHBE_MuonIdx.push_back(MuonIdx);
        MuonToPFRecHitHBHBE_PFRecHitHBHEIdx.push_back(PFRecHitIdx);
        nMuonToPFRecHitHBHBE++;
        if (isClosest){
          Muon_ClosestPFRecHitHBHEIdx[MuonIdx] = PFRecHitIdx;
          Muon_ClosestPFRecHitHBHEDetId[MuonIdx] = PFRecHitDetId;
        }
      }
    }
  }

  //==========================================
  //
  // Digi information and MAHI output
  //
  //==========================================
  //
  // https://cmssdt.cern.ch/lxr/source/RecoLocalCalo/HcalRecAlgos/test/MahiDebugger.cc
  //
  edm::EventBase const& eventbase = iEvent;
  // int nBxTrain = int(eventbase.bunchCrossing());

  if (saveMAHIInfo_) {
    mahi_hcalTimeSlewDelay = &iSetup.getData(tokDelay_);
  }

  edm::Handle<HBHEChannelInfoCollection> hbheChannelInfo;
  iEvent.getByToken(hbheChannelInfoToken_, hbheChannelInfo);

  std::vector<int>   hbhechan_ieta;
  std::vector<int>   hbhechan_iphi;
  std::vector<int>   hbhechan_depth;

  std::vector<float> hbhechan_fcByPE;
  std::vector<float> hbhechan_lambda;
  std::vector<float> hbhechan_noisecorr;

  std::vector<std::vector<float>>  hbhechan_tsRawCharge(8,std::vector<float>());
  std::vector<std::vector<float>>  hbhechan_tsPedestal(8,std::vector<float>());
  std::vector<std::vector<float>>  hbhechan_tsDFcPerADC(8,std::vector<float>());

  std::vector<int>   mahi_nSamples;
  std::vector<int>   mahi_soi;
  std::vector<float> mahi_inTimeConst;
  std::vector<float> mahi_inDarkCurrent;
  std::vector<float> mahi_inPedAvg;
  std::vector<float> mahi_inGain;
  std::vector<bool>  mahi_use8;
  std::vector<float> mahi_chiSq;
  std::vector<float> mahi_arrivalTime;
  std::vector<float> mahi_mahiEnergy;
  std::vector<float> mahi_ootEnergy0;
  std::vector<float> mahi_ootEnergy1;
  std::vector<float> mahi_ootEnergy2;
  std::vector<float> mahi_ootEnergy3;
  std::vector<float> mahi_ootEnergy4;
  std::vector<float> mahi_ootEnergy5;
  std::vector<float> mahi_ootEnergy6;
  std::vector<float> mahi_pedEnergy;

  std::vector<std::vector<float>> mahi_count(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_inputTS(8,std::vector<float>());
  std::vector<std::vector<int  >> mahi_inputTDC(8,std::vector<int>());
  std::vector<std::vector<float>> mahi_itPulse(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_inNoiseADC(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_inNoiseDC(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_inNoisePhoto(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_inPedestal(8,std::vector<float>());
  std::vector<std::vector<float>> mahi_totalUCNoise(8,std::vector<float>());

  std::vector<std::vector<std::vector<float>>>
    mahi_ootPulse(8,std::vector<std::vector<float>>(7,std::vector<float>()));

  std::vector<int>  PFRecHitHBHE_HBHEChannelInfoIdx(PFRecHitHBHE_detId.size(),-1);

  int nHBHEChannelInfo=0;

  if (savePFRecHitsHBHE_ && saveHBHEChannelInfo_){
    for (HBHEChannelInfoCollection::const_iterator iter = hbheChannelInfo->begin(); iter != hbheChannelInfo->end(); iter++) {
      const HBHEChannelInfo& hci(*iter);
      const HcalDetId detid = hci.id();

      //
      // Skip channels where we dont save the HBHE rechits. If we do save the rechit, get the index
      //
      std::size_t index = -1;
      auto it = std::find(PFRecHitHBHE_detId.begin(), PFRecHitHBHE_detId.end(), detid);
      if (it != PFRecHitHBHE_detId.end()) {
        index = std::distance(PFRecHitHBHE_detId.begin(), it);
      } else {
        continue;
      }

      int ieta = detid.ieta();
      int iphi = detid.iphi();
      int depth = detid.depth();

      hbhechan_ieta.push_back(ieta);
      hbhechan_iphi.push_back(iphi);
      hbhechan_depth.push_back(depth);

      hbhechan_fcByPE.push_back(hci.fcByPE());
      hbhechan_lambda.push_back(hci.lambda());
      hbhechan_noisecorr.push_back(hci.noisecorr());

      for (int iTS=0; iTS < 8; iTS++){
        hbhechan_tsRawCharge[iTS].push_back(hci.tsRawCharge(iTS));
        hbhechan_tsPedestal[iTS].push_back(hci.tsPedestal(iTS));
        hbhechan_tsDFcPerADC[iTS].push_back(hci.tsDFcPerADC(iTS));
      }

      if (saveMAHIInfo_){
        HcalPulseShapes theHcalPulseShapes_;
        //for pulse shapes
        std::unique_ptr<FitterFuncs::PulseShapeFunctor> psfPtr_;
        std::unique_ptr<ROOT::Math::Functor> pfunctor_;


        const MahiFit* mahi = mahi_.get();
        mahi_->setPulseShapeTemplate(hci.recoShape(), theHcalPulseShapes_, hci.hasTimeInfo(), mahi_hcalTimeSlewDelay, hci.nSamples(), hci.tsGain(0));

        MahiDebugInfo mdi;
        // initialize energies so that the values in the previous iteration are not stored
        mdi.mahiEnergy = 0;
        for (unsigned int ioot = 0; ioot < 7; ioot++){
          mdi.ootEnergy[ioot] = 0;
        }
        mahi->phase1Debug(hci, mdi);

        mahi_nSamples.push_back(mdi.nSamples);
        mahi_soi.push_back(mdi.soi);

        mahi_inTimeConst.push_back(mdi.inTimeConst);
        mahi_inDarkCurrent.push_back(mdi.inDarkCurrent);
        mahi_inPedAvg.push_back(mdi.inPedAvg);
        mahi_inGain.push_back(mdi.inGain);

        mahi_use8.push_back(mdi.use3);
        mahi_chiSq.push_back(mdi.chiSq);
        mahi_arrivalTime.push_back(mdi.arrivalTime);
        mahi_mahiEnergy.push_back(mdi.mahiEnergy);

        mahi_ootEnergy0.push_back(mdi.ootEnergy[0]);
        mahi_ootEnergy1.push_back(mdi.ootEnergy[1]);
        mahi_ootEnergy2.push_back(mdi.ootEnergy[2]);
        mahi_ootEnergy3.push_back(mdi.ootEnergy[3]);
        mahi_ootEnergy4.push_back(mdi.ootEnergy[4]);
        mahi_ootEnergy5.push_back(mdi.ootEnergy[5]);
        mahi_ootEnergy6.push_back(mdi.ootEnergy[6]);

        mahi_pedEnergy.push_back(mdi.pedEnergy);

        for (int iTS=0; iTS < 8; iTS++){
          mahi_count[iTS].push_back(mdi.count[iTS]);
          mahi_inputTS[iTS].push_back(mdi.inputTS[iTS]);
          mahi_inputTDC[iTS].push_back(mdi.inputTDC[iTS]);
          mahi_itPulse[iTS].push_back(mdi.itPulse[iTS]);
          mahi_inNoiseADC[iTS].push_back(mdi.inNoiseADC[iTS]);
          mahi_inNoiseDC[iTS].push_back(mdi.inNoiseDC[iTS]);
          mahi_inNoisePhoto[iTS].push_back(mdi.inNoisePhoto[iTS]);
          mahi_inPedestal[iTS].push_back(mdi.inPedestal[iTS]);
          mahi_totalUCNoise[iTS].push_back(mdi.totalUCNoise[iTS]);
          for (int iOOT=0; iOOT < 7; iOOT++){
            mahi_ootPulse[iTS][iOOT].push_back(mdi.ootPulse[iOOT][iTS]);
          }
        }
      }
      PFRecHitHBHE_HBHEChannelInfoIdx[index] = index;
      nHBHEChannelInfo++;
    }
  }

  //==========================================
  //
  // PFClusterECAL
  //
  //==========================================
  unsigned int nPFClusterECAL=0;
  std::vector<float>          PFClusterECAL_pt;
  std::vector<float>          PFClusterECAL_energy;
  std::vector<float>          PFClusterECAL_correctedEnergy;
  std::vector<float>          PFClusterECAL_eta;
  std::vector<float>          PFClusterECAL_phi;
  std::vector<int>            PFClusterECAL_layer;
  std::vector<int>            PFClusterECAL_nhits;
  std::vector<unsigned int>   PFClusterECAL_seedhit_detId;
  std::vector<int>            PFClusterECAL_key;

  edm::Handle<reco::PFClusterCollection> pfClustersECALHandle;
  iEvent.getByToken(pfClustersECALToken_, pfClustersECALHandle);

  for (const int key : SelectedPFClusterECAL_keys) {
    reco::PFClusterRef clusterRef( pfClustersECALHandle, key );

    PFClusterECAL_pt.push_back(clusterRef->pt());
    PFClusterECAL_energy.push_back(clusterRef->energy());
    PFClusterECAL_correctedEnergy.push_back(clusterRef->correctedEnergy());
    PFClusterECAL_eta.push_back(clusterRef->eta());
    PFClusterECAL_phi.push_back(clusterRef->phi());
    PFClusterECAL_layer.push_back(clusterRef->layer());
    PFClusterECAL_seedhit_detId.push_back(clusterRef->seed());
    PFClusterECAL_key.push_back(key);
    const std::vector<std::pair<DetId, float> >& clusterHits = clusterRef->hitsAndFractions();
    PFClusterECAL_nhits.push_back(clusterHits.size());
    nPFClusterECAL++;
  }

  //==================================================
  // Make branches for PFCand <-> PFCluster mapping
  //==================================================
  unsigned int      nPFCandToPFClusterECAL=0;
  std::vector<int>  PFCandToPFClusterECAL_PFCandIdx;
  std::vector<int>  PFCandToPFClusterECAL_PFClusterIdx;

  for (size_t i = 0; i < MappingInfo_PFCand_To_PFClusterECAL.size(); ++i) {
    int PFCandIdx = MappingInfo_PFCand_To_PFClusterECAL[i].first;
    int PFClusterKey = MappingInfo_PFCand_To_PFClusterECAL[i].second;
    //
    int PFClusterIdx = -1;
    auto it = std::find(PFClusterECAL_key.begin(), PFClusterECAL_key.end(), PFClusterKey);
    if (it != PFClusterECAL_key.end()) {
      PFClusterIdx = std::distance(PFClusterECAL_key.begin(), it);
    }
    PFCandToPFClusterECAL_PFCandIdx.push_back(PFCandIdx);
    PFCandToPFClusterECAL_PFClusterIdx.push_back(PFClusterIdx);
    nPFCandToPFClusterECAL++;
  }

  //==========================================
  //
  // PFClusterPS
  //
  //==========================================
  unsigned int nPFClusterPS=0;
  std::vector<float>          PFClusterPS_pt;
  std::vector<float>          PFClusterPS_energy;
  std::vector<float>          PFClusterPS_correctedEnergy;
  std::vector<float>          PFClusterPS_eta;
  std::vector<float>          PFClusterPS_phi;
  std::vector<int>            PFClusterPS_layer;
  std::vector<int>            PFClusterPS_nhits;
  std::vector<unsigned int>   PFClusterPS_seedhit_detId;
  std::vector<int>            PFClusterPS_key;

  edm::Handle<reco::PFClusterCollection> pfClustersPSHandle;
  iEvent.getByToken(pfClustersPSToken_, pfClustersPSHandle);

  for (const int key : SelectedPFClusterPS_keys) {
    reco::PFClusterRef clusterRef( pfClustersPSHandle, key );

    PFClusterPS_pt.push_back(clusterRef->pt());
    PFClusterPS_energy.push_back(clusterRef->energy());
    PFClusterPS_correctedEnergy.push_back(clusterRef->correctedEnergy());
    PFClusterPS_eta.push_back(clusterRef->eta());
    PFClusterPS_phi.push_back(clusterRef->phi());
    PFClusterPS_layer.push_back(clusterRef->layer());
    PFClusterPS_seedhit_detId.push_back(clusterRef->seed());
    PFClusterPS_key.push_back(key);
    const std::vector<std::pair<DetId, float> >& clusterHits = clusterRef->hitsAndFractions();
    PFClusterPS_nhits.push_back(clusterHits.size());
    nPFClusterPS++;
  }

  //==========================================
  //
  // Extend PF table
  //
  //==========================================
  auto candTable = std::make_unique<nanoaod::FlatTable>(pfCands->size(), name_, false, true);
  for (size_t iW = 0; iW < v_pfcands_weights.size(); ++iW) {
    candTable->addColumn<float>(v_weightNames_[iW], v_v_weightsOut[iW], v_weightDocs_[iW], weightPrecision_);
  }
  if (saveFromPVvertexRef_){
    candTable->addColumn<int>("fromPVvertexRef", fromPV_vertexRefOutVec, "PV(vertexRef) (NoPV = 0, PVLoose = 1, PVTight = 2, PVUsedInFit = 3)");
  }

  candTable->addColumn<float>("hcalDepthEnergyFraction1", PFCand_hcalDepthEnergyFraction1, "hcalDepthEnergyFraction1", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction2", PFCand_hcalDepthEnergyFraction2, "hcalDepthEnergyFraction2", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction3", PFCand_hcalDepthEnergyFraction3, "hcalDepthEnergyFraction3", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction4", PFCand_hcalDepthEnergyFraction4, "hcalDepthEnergyFraction4", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction5", PFCand_hcalDepthEnergyFraction5, "hcalDepthEnergyFraction5", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction6", PFCand_hcalDepthEnergyFraction6, "hcalDepthEnergyFraction6", -1);
  candTable->addColumn<float>("hcalDepthEnergyFraction7", PFCand_hcalDepthEnergyFraction7, "hcalDepthEnergyFraction7", -1);

  candTable->addColumn<float>("ptRECO",   PFCand_ptRECO,   "ptRECO",   -1);
  candTable->addColumn<float>("etaRECO",  PFCand_etaRECO,  "etaRECO",  -1);
  candTable->addColumn<float>("phiRECO",  PFCand_phiRECO,  "phiRECO",  -1);
  candTable->addColumn<float>("massRECO", PFCand_massRECO, "massRECO", -1);
  candTable->addColumn<float>("energyRECO", PFCand_energyRECO, "energyRECO", -1);

  candTable->addColumn<unsigned int>("birthId", PFCand_birthId, "birthId");

  candTable->addColumn<float>("ecalEnergy",    PFCand_ecalEnergy,    "ecalEnergy",    -1);
  candTable->addColumn<float>("rawEcalEnergy", PFCand_rawEcalEnergy, "rawEcalEnergy", -1);
  candTable->addColumn<float>("hcalEnergy",    PFCand_hcalEnergy,    "hcalEnergy",    -1);
  candTable->addColumn<float>("rawHcalEnergy", PFCand_rawHcalEnergy, "rawHcalEnergy", -1);
  candTable->addColumn<float>("hoEnergy",      PFCand_hoEnergy,      "hoEnergy",      -1);
  candTable->addColumn<float>("rawHoEnergy",   PFCand_rawHoEnergy,   "rawHoEnergy",   -1);

  candTable->addColumn<unsigned int>("blockIdx",         PFCand_blockIdx,         "blockIdx");
  candTable->addColumn<unsigned int>("nPFTrack",         PFCand_nPFTrack,         "nPFTrack");
  candTable->addColumn<unsigned int>("nPFClusterHCAL",   PFCand_nPFClusterHCAL,   "nPFClusterHCAL");
  candTable->addColumn<unsigned int>("nPFClusterECAL",   PFCand_nPFClusterECAL,   "nPFClusterECAL");
  candTable->addColumn<unsigned int>("nPFClusterPS",     PFCand_nPFClusterPS,     "nPFClusterPS");

  candTable->addColumn<unsigned int>("nPFTrackInBlock",         PFCand_nPFTrackInBlock,         "nPFTrackInBlock");
  candTable->addColumn<unsigned int>("nPFClusterHCALInBlock",   PFCand_nPFClusterHCALInBlock,   "nPFClusterHCALInBlock");
  candTable->addColumn<unsigned int>("nPFClusterECALInBlock",   PFCand_nPFClusterECALInBlock,   "nPFClusterECALInBlock");
  candTable->addColumn<unsigned int>("nPFClusterPSInBlock",     PFCand_nPFClusterPSInBlock,     "nPFClusterPSInBlock");

  iEvent.put(std::move(candTable), name_);

  //==========================================
  //
  // Make PFCluster HCAL table
  //
  //==========================================
  if (savePFClustersHCAL_){
    auto pfClusterHCALTable = std::make_unique<nanoaod::FlatTable>(nPFClusterHCAL, name_PFClusterHCAL_, false, false);
    pfClusterHCALTable->addColumn<float>("pt", PFClusterHCAL_pt,"pt",-1);
    pfClusterHCALTable->addColumn<float>("energy", PFClusterHCAL_energy,"energy",-1);
    pfClusterHCALTable->addColumn<float>("eta", PFClusterHCAL_eta,"eta",-1);
    pfClusterHCALTable->addColumn<float>("phi", PFClusterHCAL_phi,"phi",-1);
    pfClusterHCALTable->addColumn<int>("layer", PFClusterHCAL_layer,"layer");
    pfClusterHCALTable->addColumn<int>("nhits", PFClusterHCAL_nhits,"nhits");
    pfClusterHCALTable->addColumn<unsigned int>("seedhit_detId", PFClusterHCAL_seedhit_detId,"seedhit_detId");
    iEvent.put(std::move(pfClusterHCALTable), name_PFClusterHCAL_);
  }

  //==========================================
  //
  // Make PFCluster HBHE table
  //
  //==========================================
  if (savePFClustersHBHE_){
    auto pfClusterHBHETable = std::make_unique<nanoaod::FlatTable>(nPFClusterHBHE, name_PFClusterHBHE_, false, false);
    pfClusterHBHETable->addColumn<float>("pt", PFClusterHBHE_pt,"pt",-1);
    pfClusterHBHETable->addColumn<float>("energy", PFClusterHBHE_energy,"energy",-1);
    pfClusterHBHETable->addColumn<float>("eta", PFClusterHBHE_eta,"eta",-1);
    pfClusterHBHETable->addColumn<float>("phi", PFClusterHBHE_phi,"phi",-1);
    pfClusterHBHETable->addColumn<int>("layer", PFClusterHBHE_layer,"layer");
    pfClusterHBHETable->addColumn<int>("nhits", PFClusterHBHE_nhits,"nhits");
    pfClusterHBHETable->addColumn<unsigned int>("seedhit_detId", PFClusterHBHE_seedhit_detId,"seedhit_detId");
    iEvent.put(std::move(pfClusterHBHETable), name_PFClusterHBHE_);
  }


  //==========================================
  //
  // Make PFRecHit HBHE table
  //
  //==========================================
  if (savePFRecHitsHBHE_){
    auto pfRecHitHBHETable = std::make_unique<nanoaod::FlatTable>(nPFRecHitHBHE, name_PFRecHitHBHE_,  false, false);
    pfRecHitHBHETable->addColumn<float>("energy", PFRecHitHBHE_energy,"energy",-1);
    pfRecHitHBHETable->addColumn<float>("time", PFRecHitHBHE_time,"time",-1);
    pfRecHitHBHETable->addColumn<int>("ieta", PFRecHitHBHE_ieta,"ieta");
    pfRecHitHBHETable->addColumn<int>("iphi", PFRecHitHBHE_iphi,"iphi");
    pfRecHitHBHETable->addColumn<int>("depth", PFRecHitHBHE_depth,"depth");
    pfRecHitHBHETable->addColumn<float>("pfCutThresh", PFRecHitHBHE_pfCutThresh,"pfCutThresh");
    pfRecHitHBHETable->addColumn<float>("respCorr", PFRecHitHBHE_respCorr,"respCorr");
    pfRecHitHBHETable->addColumn<unsigned int>("detId", PFRecHitHBHE_detId,"detId");
    pfRecHitHBHETable->addColumn<int>("HBHEChannelInfoIdx", PFRecHitHBHE_HBHEChannelInfoIdx,"HBHEChannelInfoIdx");
    iEvent.put(std::move(pfRecHitHBHETable),  name_PFRecHitHBHE_);

    if (saveHBHEChannelInfo_){
      auto HBHEChannelInfoTable = std::make_unique<nanoaod::FlatTable>(nHBHEChannelInfo, "HBHEChannelInfo",  false, false);
      HBHEChannelInfoTable->addColumn<int>("ieta", hbhechan_ieta,"ieta");
      HBHEChannelInfoTable->addColumn<int>("iphi", hbhechan_iphi,"iphi");
      HBHEChannelInfoTable->addColumn<int>("depth", hbhechan_depth,"depth");
      HBHEChannelInfoTable->addColumn<float>("fcByPE", hbhechan_fcByPE,"fcByPE");
      HBHEChannelInfoTable->addColumn<float>("lambda", hbhechan_lambda,"lambda");
      HBHEChannelInfoTable->addColumn<float>("noisecorr", hbhechan_noisecorr,"noisecorr");
      for (int iTS=0; iTS < 8; iTS++){
        HBHEChannelInfoTable->addColumn<float>(std::string("ts")+std::to_string(iTS)+std::string("RawCharge"),hbhechan_tsRawCharge[iTS],"tsRawCharge",-1);
        HBHEChannelInfoTable->addColumn<float>(std::string("ts")+std::to_string(iTS)+std::string("Pedestal"), hbhechan_tsPedestal[iTS], "tsPedestal",-1);
        HBHEChannelInfoTable->addColumn<float>(std::string("ts")+std::to_string(iTS)+std::string("DFcPerADC"),hbhechan_tsDFcPerADC[iTS],"tsDFcPerADC",-1);
      }
      if(saveMAHIInfo_){
        HBHEChannelInfoTable->addColumn<int>  ("mahi_nSamples",mahi_nSamples,"mahi_nSamples");
        HBHEChannelInfoTable->addColumn<int>  ("mahi_soi",mahi_soi,"mahi_soi");
        HBHEChannelInfoTable->addColumn<float>("mahi_inTimeConst",mahi_inTimeConst,"mahi_inTimeConst",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_inDarkCurrent",mahi_inDarkCurrent,"mahi_inDarkCurrent",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_inPedAvg",mahi_inPedAvg,"mahi_inPedAvg",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_inGain",mahi_inGain,"mahi_inGain",-1);
        HBHEChannelInfoTable->addColumn<bool> ("mahi_use8",mahi_use8,"mahi_use8");
        HBHEChannelInfoTable->addColumn<float>("mahi_chiSq",mahi_chiSq,"mahi_chiSq",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_arrivalTime",mahi_arrivalTime,"mahi_arrivalTime",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_mahiEnergy",mahi_mahiEnergy,"mahi_mahiEnergy",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy0",mahi_ootEnergy0,"mahi_ootEnergy0",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy1",mahi_ootEnergy1,"mahi_ootEnergy1",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy2",mahi_ootEnergy2,"mahi_ootEnergy2",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy3",mahi_ootEnergy3,"mahi_ootEnergy3",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy4",mahi_ootEnergy4,"mahi_ootEnergy4",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy5",mahi_ootEnergy5,"mahi_ootEnergy5",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_ootEnergy6",mahi_ootEnergy6,"mahi_ootEnergy6",-1);
        HBHEChannelInfoTable->addColumn<float>("mahi_pedEnergy",mahi_pedEnergy,"mahi_pedEnergy",-1);
        for (int iTS=0; iTS < 8; iTS++){
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_count")+std::to_string(iTS),       mahi_count[iTS],"mahi_count",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_inputTS")+std::to_string(iTS),     mahi_inputTS[iTS],"mahi_inputTS",-1);
          HBHEChannelInfoTable->addColumn<int>  (std::string("mahi_inputTDC")+std::to_string(iTS),    mahi_inputTDC[iTS],"mahi_inputTDC");
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_itPulse")+std::to_string(iTS),     mahi_itPulse[iTS],"mahi_itPulse",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_inNoiseADC")+std::to_string(iTS),  mahi_inNoiseADC[iTS],"mahi_inNoiseADC",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_inNoiseDC")+std::to_string(iTS),   mahi_inNoiseDC[iTS],"mahi_inNoiseDC",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_inNoisePhoto")+std::to_string(iTS),mahi_inNoisePhoto[iTS],"mahi_inNoisePhoto",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_inPedestal")+std::to_string(iTS),  mahi_inPedestal[iTS],"mahi_inPedestal",-1);
          HBHEChannelInfoTable->addColumn<float>(std::string("mahi_totalUCNoise")+std::to_string(iTS),mahi_totalUCNoise[iTS],"mahi_totalUCNoise",-1);
          for (int iOOT=0; iOOT < 7; iOOT++){
            HBHEChannelInfoTable->addColumn<float>(std::string("mahi_ootPulse")+std::to_string(iTS)+std::to_string(iOOT),mahi_ootPulse[iTS][iOOT],"mahi_ootPulse",-1);
          }
        }
      }
      iEvent.put(std::move(HBHEChannelInfoTable),"HBHEChannelInfo");
    }
  }

  //========================================================
  //
  // Make PFCand <-> PFCluster HCAL mapping table
  //
  //========================================================
  if (savePFClustersHCAL_){
    auto pfCandToClusterHCALTable = std::make_unique<nanoaod::FlatTable>(nPFCandToPFClusterHCAL, name_PFCandToPFClusterHCAL_,  false, false);
    pfCandToClusterHCALTable->addColumn<int>("PFCandIdx",        PFCandToPFClusterHCAL_PFCandIdx,   "PFCandIdx");
    pfCandToClusterHCALTable->addColumn<int>("PFClusterHCALIdx", PFCandToPFClusterHCAL_PFClusterIdx,"PFClusterHCALIdx");
    iEvent.put(std::move(pfCandToClusterHCALTable),  name_PFCandToPFClusterHCAL_);
  }

  //========================================================
  //
  // Make PFCluster HCAL <-> PFCluster HBHE mapping table
  //
  //========================================================
  // if (savePFClustersHCAL_ && savePFClustersHBHE_){
  //   auto pfClusterHCALToClusterHBHETable = std::make_unique<nanoaod::FlatTable>(nPFClusterHCALToPFClusterHBHBE, name_PFClusterHCALToPFClusterHBHE_,  false, false);
  //   pfClusterHCALToClusterHBHETable->addColumn<int>("PFCandIdx",        PFClusterHCALToPFClusterHBHE_PFClusterHCALIdx,"PFClusterHCALIdx");
  //   pfClusterHCALToClusterHBHETable->addColumn<int>("PFClusterHBHEIdx", PFClusterHCALToPFClusterHBHE_PFClusterHBHEIdx,"PFClusterHBHEIdx");
  //   iEvent.put(std::move(pfClusterHCALToClusterHBHETable),  name_PFClusterHCALToPFClusterHBHE_);
  // }

  //========================================================
  //
  // Make PFClusterHCAL <-> PFRecHitHBHE mapping table
  //
  //========================================================
  if (savePFClustersHCAL_ && savePFRecHitsHBHE_){
    auto pfClusterHCALToRecHitHBHETable = std::make_unique<nanoaod::FlatTable>(nPFClusterHCALToPFRecHitHBHBE, name_PFClusterHCALToPFRecHitHBHE_,  false, false);
    pfClusterHCALToRecHitHBHETable->addColumn<int>("PFClusterHCALIdx", PFClusterHCALToPFRecHitHBHBE_PFClusterHCALIdx,"PFClusterHCALIdx");
    pfClusterHCALToRecHitHBHETable->addColumn<int>("PFRecHitHBHEIdx",  PFClusterHCALToPFRecHitHBHBE_PFRecHitHBHEIdx,"PFRecHitHBHEIdx");
    pfClusterHCALToRecHitHBHETable->addColumn<float>("fraction",       PFClusterHCALToPFRecHitHBHBE_fraction,"fraction",-1);
    iEvent.put(std::move(pfClusterHCALToRecHitHBHETable),  name_PFClusterHCALToPFRecHitHBHE_);
  }

  //========================================================
  //
  // Make PFClusterHBHE <-> PFRecHitHBHE mapping table
  //
  //========================================================
  if (savePFClustersHBHE_ && savePFRecHitsHBHE_){
    auto pfClusterHBHEToRecHitHBHETable = std::make_unique<nanoaod::FlatTable>(nPFClusterHBHEToPFRecHitHBHBE, name_PFClusterHBHEToPFRecHitHBHE_,  false, false);
    pfClusterHBHEToRecHitHBHETable->addColumn<int>("PFClusterHBHEIdx", PFClusterHBHEToPFRecHitHBHBE_PFClusterHBHEIdx,"PFClusterHBHEIdx");
    pfClusterHBHEToRecHitHBHETable->addColumn<int>("PFRecHitHBHEIdx",  PFClusterHBHEToPFRecHitHBHBE_PFRecHitHBHEIdx,"PFRecHitHBHEIdx");
    pfClusterHBHEToRecHitHBHETable->addColumn<float>("fraction",       PFClusterHBHEToPFRecHitHBHBE_fraction,"fraction",-1);
    iEvent.put(std::move(pfClusterHBHEToRecHitHBHETable),  name_PFClusterHBHEToPFRecHitHBHE_);
  }

  //========================================================
  //
  // Make Muon <-> PFRecHitHBHE mapping table
  //
  //========================================================
  if (matchMuonsWithPFRecHitsHBHE_ && savePFRecHitsHBHE_){
    auto muonToRecHitHBHETable = std::make_unique<nanoaod::FlatTable>(nMuonToPFRecHitHBHBE, name_MuonToPFRecHitHBHE_,  false, false);
    muonToRecHitHBHETable->addColumn<int>("MuonIdx", MuonToPFRecHitHBHBE_MuonIdx,"MuonIdx");
    muonToRecHitHBHETable->addColumn<int>("PFRecHitHBHEIdx",  MuonToPFRecHitHBHBE_PFRecHitHBHEIdx,"PFRecHitHBHEIdx");
    iEvent.put(std::move(muonToRecHitHBHETable),  name_MuonToPFRecHitHBHE_);

    auto muonTable = std::make_unique<nanoaod::FlatTable>(nMuons, "Muon", false, true);
    muonTable->addColumn<int>("ClosestPFRecHitHBHEIdx", Muon_ClosestPFRecHitHBHEIdx,"Index of closest PFRecHitHBHE");
    muonTable->addColumn<int>("ClosestPFRecHitHBHEDetId", Muon_ClosestPFRecHitHBHEDetId,"DetId of closest PFRecHitHBHE");

    iEvent.put(std::move(muonTable),  "Muon");
  }

  //==========================================
  //
  // Make PFCluster ECAL table
  //
  //==========================================
  if (savePFClustersECAL_){
    auto pfClusterECALTable = std::make_unique<nanoaod::FlatTable>(nPFClusterECAL, name_PFClusterECAL_, false, false);
    pfClusterECALTable->addColumn<float>("pt", PFClusterECAL_pt,"pt",-1);
    pfClusterECALTable->addColumn<float>("energy", PFClusterECAL_energy,"energy",-1);
    pfClusterECALTable->addColumn<float>("correctedEnergy", PFClusterECAL_correctedEnergy,"correctedEnergy",-1);
    pfClusterECALTable->addColumn<float>("eta", PFClusterECAL_eta,"eta",-1);
    pfClusterECALTable->addColumn<float>("phi", PFClusterECAL_phi,"phi",-1);
    pfClusterECALTable->addColumn<int>("layer", PFClusterECAL_layer,"layer");
    pfClusterECALTable->addColumn<int>("nhits", PFClusterECAL_nhits,"nhits");
    pfClusterECALTable->addColumn<unsigned int>("seedhit_detId", PFClusterECAL_seedhit_detId,"seedhit_detId");
    iEvent.put(std::move(pfClusterECALTable), name_PFClusterECAL_);
  }

  //========================================================
  //
  // Make PFCand <-> PFCluster ECAL mapping table
  //
  //========================================================
  if (savePFClustersECAL_){
    auto pfCandToClusterECALTable = std::make_unique<nanoaod::FlatTable>(nPFCandToPFClusterECAL, name_PFCandToPFClusterECAL_,  false, false);
    pfCandToClusterECALTable->addColumn<int>("PFCandIdx",        PFCandToPFClusterECAL_PFCandIdx,   "PFCandIdx");
    pfCandToClusterECALTable->addColumn<int>("PFClusterECALIdx", PFCandToPFClusterECAL_PFClusterIdx,"PFClusterECALIdx");
    iEvent.put(std::move(pfCandToClusterECALTable),  name_PFCandToPFClusterECAL_);
  }

  //==========================================
  //
  // Make PFCluster PS table
  //
  //==========================================
  if (savePFClustersPS_){
    auto pfClusterPSTable = std::make_unique<nanoaod::FlatTable>(nPFClusterPS, name_PFClusterPS_, false, false);
    pfClusterPSTable->addColumn<float>("pt", PFClusterPS_pt,"pt",-1);
    pfClusterPSTable->addColumn<float>("energy", PFClusterPS_energy,"energy",-1);
    pfClusterPSTable->addColumn<float>("correctedEnergy", PFClusterPS_correctedEnergy,"correctedEnergy",-1);
    pfClusterPSTable->addColumn<float>("eta", PFClusterPS_eta,"eta",-1);
    pfClusterPSTable->addColumn<float>("phi", PFClusterPS_phi,"phi",-1);
    pfClusterPSTable->addColumn<int>("layer", PFClusterPS_layer,"layer");
    pfClusterPSTable->addColumn<int>("nhits", PFClusterPS_nhits,"nhits");
    pfClusterPSTable->addColumn<unsigned int>("seedhit_detId", PFClusterPS_seedhit_detId,"seedhit_detId");
    iEvent.put(std::move(pfClusterPSTable), name_PFClusterPS_);
  }

  // auto pfRecHitEBTable    = std::make_unique<nanoaod::FlatTable>(SelectedPFRecHitEB,    name_PFRecHitEB_,    false, false);
  // auto pfRecHitEETable    = std::make_unique<nanoaod::FlatTable>(SelectedPFRecHitEE,    name_PFRecHitEE_,    false, false);
  // auto pfRecHitPSTable    = std::make_unique<nanoaod::FlatTable>(SelectedPFRecHitPS,    name_PFRecHitPS_,    false, false);
  // iEvent.put(std::move(pfRecHitEBTable),    name_PFRecHitEB_);
  // iEvent.put(std::move(pfRecHitEETable),    name_PFRecHitEE_);
  // iEvent.put(std::move(pfRecHitPSTable),    name_PFRecHitPS_);
}

void PackedCandidateExtTableProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("srcPFCandidates", edm::InputTag("finalJetsConstituents"));
  desc.add<edm::InputTag>("srcJets", edm::InputTag("finalJetsPuppi"));
  desc.add<edm::InputTag>("srcMuons", edm::InputTag("finalMuons"));
  desc.add<edm::InputTag>("packedPFCandidates", edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("PFClustersHCAL", edm::InputTag("particleFlowClusterHCAL"));
  desc.add<edm::InputTag>("PFClustersHBHE", edm::InputTag("particleFlowClusterHBHE"));
  desc.add<edm::InputTag>("PFRecHitsHBHE", edm::InputTag("particleFlowRecHitHBHE"));
  desc.add<edm::InputTag>("PFClustersECAL", edm::InputTag("particleFlowClusterECAL"));
  desc.add<edm::InputTag>("PFClustersPS", edm::InputTag("particleFlowClusterPS"));
  desc.add<edm::InputTag>("hbheChannelInfo", edm::InputTag("hbheprereco"));
  desc.add<bool>("savePFClustersHCAL", true);
  desc.add<bool>("saveAllPFClustersHCAL", false);
  desc.add<bool>("savePFClustersHBHE", true);
  desc.add<bool>("savePFRecHitsHBHE", true);
  desc.add<bool>("saveAllPFRecHitsHBHE", false);
  desc.add<bool>("saveHBHEChannelInfo", true);
  desc.add<bool>("saveMAHIInfo", false);
  desc.add<bool>("savePFClustersECAL", false);
  desc.add<bool>("savePFClustersPS", true);
  desc.add<bool>("matchMuonsWithPFRecHitsHBHE", false);
  desc.add<std::string>("name", "PFCand");
  desc.add<bool>("saveFromPVvertexRef", false);
  desc.add<int>("weightPrecision", -1);

  desc.add<bool>("mahi_dynamicPed",false);
  desc.add<double>("mahi_ts4Thresh",0.);
  desc.add<double>("mahi_chiSqSwitch",0.);
  desc.add<bool>("mahi_applyTimeSlew",false);
  desc.add<bool>("mahi_calculateArrivalTime",false);
  desc.add<int>("mahi_timeAlgo",0);
  desc.add<double>("mahi_thEnergeticPulses",0.);
  desc.add<double>("mahi_meanTime",0.);
  desc.add<double>("mahi_timeSigmaHPD",0.);
  desc.add<double>("mahi_timeSigmaSiPM",0.);
  desc.add<std::vector<int>>("mahi_activeBXs",std::vector<int>());
  desc.add<int>("mahi_nMaxItersMin",0);
  desc.add<int>("mahi_nMaxItersNNLS",0);
  desc.add<double>("mahi_deltaChiSqThresh",0.);
  desc.add<double>("mahi_nnlsThresh",0.);

  std::vector<edm::InputTag> emptyVInputTags;
  desc.add<std::vector<edm::InputTag>>("srcWeightsV",emptyVInputTags);

  std::vector<std::string> emptyVStrings;
  desc.add<std::vector<std::string>>("weightNamesV",emptyVStrings);
  desc.add<std::vector<std::string>>("weightDocsV",emptyVStrings);

  descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(PackedCandidateExtTableProducer);
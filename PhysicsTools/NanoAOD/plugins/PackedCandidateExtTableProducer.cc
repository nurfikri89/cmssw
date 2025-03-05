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

class PackedCandidateExtTableProducer : public edm::stream::EDProducer<> {
public:
  explicit PackedCandidateExtTableProducer(const edm::ParameterSet &);
  ~PackedCandidateExtTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  edm::EDGetTokenT<reco::CandidateView>  pfcands_token_;
  const edm::EDGetTokenT<pat::PackedCandidateCollection> pc_;
  const edm::EDGetTokenT<edm::Association<reco::PFCandidateCollection>> pc2pf_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersHCALToken_;
  const edm::EDGetTokenT<reco::PFRecHitCollection> pfRecHitsHBHEToken_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersECALToken_;
  const edm::EDGetTokenT<reco::PFClusterCollection> pfClustersPSToken_;

  const bool savePFClustersHCAL_;
  const bool savePFRecHitsHBHE_;
  const bool savePFClustersECAL_;
  const bool savePFClustersPS_;

  const std::string name_;
  const bool saveFromPVvertexRef_;
  const int weightPrecision_;

  std::string name_PFClusterHCAL_;
  std::string name_PFRecHitHBHE_;
  std::string name_PFCandToPFClusterHCAL_;
  std::string name_PFClusterHCALToPFRecHitHBHE_;

  std::string name_PFClusterECAL_;
  std::string name_PFRecHitEB_;
  std::string name_PFRecHitEE_;

  std::string name_PFClusterPS_;
  std::string name_PFRecHitPS_;

  std::vector<edm::EDGetTokenT<edm::ValueMap<float>>> v_pfcands_weights_tokens_;
  std::vector<std::string> v_weightNames_;
  std::vector<std::string> v_weightDocs_;
};

//
// constructors and destructor
//
PackedCandidateExtTableProducer::PackedCandidateExtTableProducer(const edm::ParameterSet &iConfig):
  pfcands_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("srcPFCandidates"))),
  pc_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
  pc2pf_(consumes<edm::Association<reco::PFCandidateCollection>>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
  pfClustersHCALToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersHCAL"))),
  pfRecHitsHBHEToken_(consumes<std::vector<reco::PFRecHit>>(iConfig.getParameter<edm::InputTag>("PFRecHitsHBHE"))),
  pfClustersECALToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersECAL"))),
  pfClustersPSToken_(consumes<reco::PFClusterCollection>(iConfig.getParameter<edm::InputTag>("PFClustersPS"))),
  savePFClustersHCAL_(iConfig.getParameter<bool>("savePFClustersHCAL")),
  savePFRecHitsHBHE_(iConfig.getParameter<bool>("savePFRecHitsHBHE")),
  savePFClustersECAL_(iConfig.getParameter<bool>("savePFClustersECAL")),
  savePFClustersPS_(iConfig.getParameter<bool>("savePFClustersPS")),
  name_(iConfig.getParameter<std::string>("name")),
  saveFromPVvertexRef_(iConfig.getParameter<bool>("saveFromPVvertexRef")),
  weightPrecision_(iConfig.getParameter<int>("weightPrecision"))
{
  v_pfcands_weights_tokens_ = edm::vector_transform(
    iConfig.getParameter<std::vector<edm::InputTag>>("srcWeightsV"),
    [this](edm::InputTag const& tag) {return consumes<edm::ValueMap<float>>(tag);}
  );
  v_weightNames_ = iConfig.getParameter<std::vector<std::string>>("weightNamesV");
  v_weightDocs_ = iConfig.getParameter<std::vector<std::string>>("weightDocsV");

  produces<nanoaod::FlatTable>(name_);

  name_PFClusterHCAL_ = "PFClusterHCAL";
  name_PFRecHitHBHE_ = "PFRecHitHBHE";
  name_PFCandToPFClusterHCAL_ = "PFCandToPFClusterHCAL";
  name_PFClusterHCALToPFRecHitHBHE_ = "PFClusterHCALToPFRecHitHBHE";

  name_PFClusterECAL_ = "PFClusterECAL";
  name_PFRecHitEB_ = "PFRecHitEB";
  name_PFRecHitEE_ = "PFRecHitEE";

  name_PFClusterPS_ = "PFClusterPS";
  name_PFRecHitPS_ = "PFRecHitPS";

  if(savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFClusterHCAL_);
  if (savePFRecHitsHBHE_)
    produces<nanoaod::FlatTable>(name_PFRecHitHBHE_);
  if(savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFCandToPFClusterHCAL_);
  if (savePFRecHitsHBHE_ && savePFClustersHCAL_)
    produces<nanoaod::FlatTable>(name_PFClusterHCALToPFRecHitHBHE_);
  if(savePFClustersECAL_)
    produces<nanoaod::FlatTable>(name_PFClusterECAL_);
  if (savePFClustersPS_)
    produces<nanoaod::FlatTable>(name_PFClusterPS_);

  // produces<nanoaod::FlatTable>(name_PFRecHitEB_);
  // produces<nanoaod::FlatTable>(name_PFRecHitEE_);

  // produces<nanoaod::FlatTable>(name_PFClusterPS_);
  // produces<nanoaod::FlatTable>(name_PFRecHitPS_);
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
  // We will keep track of which PFRecHit to keep by storing the detId
  // to of each PFRecHit object in a set.
  //
  std::set<uint32_t> SelectedPFRecHitHBHE_rawDetId;

  //first: SelectedPFClusterIdx
  //second.first: DetId
  //second.second: RecHit energy fraction to the PFCluster
  std::vector<std::pair<int,std::pair<unsigned int,float>>> MappingInfo_PFClusterHCAL_To_PFRecHit;

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
  // PFRecHits HCAL: HBHE
  //
  //==========================================
  unsigned int nPFRecHitHBHE=0;
  std::vector<float>          PFRecHitHBHE_energy;
  std::vector<int>            PFRecHitHBHE_ieta;
  std::vector<int>            PFRecHitHBHE_iphi;
  std::vector<int>            PFRecHitHBHE_depth;
  std::vector<unsigned int>   PFRecHitHBHE_detId;

  unsigned int nPFClusterHCALToPFRecHitHBHBE=0;
  std::vector<int>   PFClusterHCALToPFRecHitHBHBE_PFClusterHCALIdx;
  std::vector<int>   PFClusterHCALToPFRecHitHBHBE_PFRecHitHBHEIdx;
  std::vector<float> PFClusterHCALToPFRecHitHBHBE_fraction;

  if (savePFRecHitsHBHE_){
    edm::Handle<std::vector<reco::PFRecHit>> pfRecHitsHBHEHandle;
    iEvent.getByToken(pfRecHitsHBHEToken_, pfRecHitsHBHEHandle);
    auto pfRecHitsHBHE = pfRecHitsHBHEHandle.product();

    for (size_t idx = 0; idx < pfRecHitsHBHE->size(); idx++){
      reco::PFRecHitRef pfrechitRef( pfRecHitsHBHEHandle, idx);
      if (SelectedPFRecHitHBHE_rawDetId.find(pfrechitRef.get()->detId()) == SelectedPFRecHitHBHE_rawDetId.end())
        continue;
      HcalDetId theHcalDetId(pfrechitRef.get()->detId());
      PFRecHitHBHE_energy.push_back(pfrechitRef.get()->energy());
      PFRecHitHBHE_ieta.push_back(theHcalDetId.ieta());
      PFRecHitHBHE_iphi.push_back(theHcalDetId.iphi());
      PFRecHitHBHE_depth.push_back(theHcalDetId.depth());
      PFRecHitHBHE_detId.push_back(pfrechitRef.get()->detId());
      nPFRecHitHBHE++;
    }
    //==================================================
    // Make branches for PFRecHit <-> PFCluster mapping
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
  // Make PFRecHit HBHE table
  //
  //==========================================
  if (savePFRecHitsHBHE_){
    auto pfRecHitHBHETable = std::make_unique<nanoaod::FlatTable>(nPFRecHitHBHE, name_PFRecHitHBHE_,  false, false);
    pfRecHitHBHETable->addColumn<float>("energy", PFRecHitHBHE_energy,"energy",-1);
    pfRecHitHBHETable->addColumn<int>("ieta", PFRecHitHBHE_ieta,"ieta");
    pfRecHitHBHETable->addColumn<int>("iphi", PFRecHitHBHE_iphi,"iphi");
    pfRecHitHBHETable->addColumn<int>("depth", PFRecHitHBHE_depth,"depth");
    pfRecHitHBHETable->addColumn<unsigned int>("detId", PFRecHitHBHE_detId,"detId");
    iEvent.put(std::move(pfRecHitHBHETable),  name_PFRecHitHBHE_);
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
  desc.add<edm::InputTag>("packedPFCandidates", edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("PFClustersHCAL", edm::InputTag("particleFlowClusterHCAL"));
  desc.add<edm::InputTag>("PFRecHitsHBHE", edm::InputTag("particleFlowRecHitHBHE"));
  desc.add<edm::InputTag>("PFClustersECAL", edm::InputTag("particleFlowClusterECAL"));
  desc.add<edm::InputTag>("PFClustersPS", edm::InputTag("particleFlowClusterPS"));
  desc.add<bool>("savePFClustersHCAL", true);
  desc.add<bool>("savePFRecHitsHBHE", true);
  desc.add<bool>("savePFClustersECAL", true);
  desc.add<bool>("savePFClustersPS", true);
  desc.add<std::string>("name", "PFCand");
  desc.add<bool>("saveFromPVvertexRef", false);
  desc.add<int>("weightPrecision", -1);

  std::vector<edm::InputTag> emptyVInputTags;
  desc.add<std::vector<edm::InputTag>>("srcWeightsV",emptyVInputTags);

  std::vector<std::string> emptyVStrings;
  desc.add<std::vector<std::string>>("weightNamesV",emptyVStrings);
  desc.add<std::vector<std::string>>("weightDocsV",emptyVStrings);

  descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(PackedCandidateExtTableProducer);
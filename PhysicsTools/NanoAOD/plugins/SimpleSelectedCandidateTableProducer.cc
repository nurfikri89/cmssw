#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

class SimpleSelectedCandidateTableProducer : public edm::stream::EDProducer<> {
public:
  explicit SimpleSelectedCandidateTableProducer(const edm::ParameterSet &);
  ~SimpleSelectedCandidateTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  const std::string name_;
  const std::string candIdxName_;
  const std::string candIdxDoc_;

  edm::EDGetTokenT<reco::CandidateView> candMain_token_;
  edm::EDGetTokenT<reco::CandidateView> candSelected_token_;
  edm::EDGetTokenT<edm::Association<reco::GenParticleCollection>> pc2gp_token_;

  bool getGenInfo_;

  edm::Handle<reco::CandidateView> candMain_;
  edm::Handle<reco::CandidateView> candSelected_;
  edm::Handle<edm::Association<reco::GenParticleCollection>> pc2gp_;
};

//
// constructors and destructor
//
SimpleSelectedCandidateTableProducer::SimpleSelectedCandidateTableProducer(const edm::ParameterSet &iConfig): 
  name_(iConfig.getParameter<std::string>("name")),
  candIdxName_(iConfig.getParameter<std::string>("candIdxName")),
  candIdxDoc_(iConfig.getParameter<std::string>("candIdxDoc")),
  candMain_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidatesMain"))),
  candSelected_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidatesSelected")))
{
  // getGenInfo_=false;
  // edm::InputTag pc2gpInputTag = iConfig.getParameter<edm::InputTag>("pc2gp");
  // if (!pc2gpInputTag.label().empty()) {
  //   pc2gp_token_ = consumes<edm::Association<reco::GenParticleCollection>>(iConfig.getParameter<edm::InputTag>("pc2gp"));
  //   getGenInfo_ = true;
  // }

  produces<nanoaod::FlatTable>(name_);
  produces<std::vector<reco::CandidatePtr>>();
}

SimpleSelectedCandidateTableProducer::~SimpleSelectedCandidateTableProducer() {}

void SimpleSelectedCandidateTableProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  // elements in all these collections must have the same order!
  auto outCands = std::make_unique<std::vector<reco::CandidatePtr>>();

  iEvent.getByToken(candMain_token_, candMain_);
  auto candMainPtrs = candMain_->ptrs();

  iEvent.getByToken(candSelected_token_, candSelected_);
  auto candSelectedPtrs = candSelected_->ptrs();

  // if (getGenInfo_){
  //   iEvent.getByToken(pc2gp_token_, pc2gp_);
  //   const auto& packedCandToGenParticles = *pc2gp_;
  // }

  //
  // Loop over selected candidates
  //
  std::vector<int> candIdx;

  // std::vector<float> genPartAssoc_pt;
  // std::vector<float> genPartAssoc_eta;
  // std::vector<float> genPartAssoc_phi;
  // std::vector<float> genPartAssoc_mass;
  // std::vector<int>   genPartAssoc_pdgId;

  for (unsigned candSelectedIdx = 0; candSelectedIdx < candSelectedPtrs.size(); ++candSelectedIdx) {
    const auto& candSelected = candSelectedPtrs.at(candSelectedIdx);
    auto candInMainList = std::find(candMainPtrs.begin(), candMainPtrs.end(), candSelected);
    if (candInMainList == candMainPtrs.end()) {
      continue;
    }
    outCands->push_back(candSelected);
    candIdx.push_back(candInMainList - candMainPtrs.begin());

    // float genPart_pt = -1.;
    // float genPart_eta = -9.;
    // float genPart_phi = -9.;
    // float genPart_mass = -9.;
    // float genPart_pdgId = 0;

    // if (getGenInfo_){
    //   const pat::PackedCandidate* packedCand = dynamic_cast<const pat::PackedCandidate*>(candSelected.get());
    //   if (packedCand && packedCand->hasTrackDetails()){
    //     auto genPart = packedCandToGenParticles[candSelected];
    //     if(genPart.isNonnull()){
    //       genPart_pt = genPart->pt();
    //       genPart_eta = genPart->eta();
    //       genPart_phi = genPart->phi();
    //       genPart_mass = genPart->mass();
    //       genPart_pdgId = genPart->pdgId();
    //     }
    //   }
    // }
    // genPartAssoc_pt.push_back(genPart_pt);
    // genPartAssoc_eta.push_back(genPart_eta);
    // genPartAssoc_phi.push_back(genPart_phi);
    // genPartAssoc_mass.push_back(genPart_mass);
    // genPartAssoc_pdgId.push_back(genPart_pdgId);
  }

  auto candTable = std::make_unique<nanoaod::FlatTable>(outCands->size(), name_, false, true);
  // We fill from here only stuff that cannot be created with the SimpleFlatTableProducer
  candTable->addColumn<int>(candIdxName_, candIdx, candIdxDoc_);
  // if (getGenInfo_){
  //   candTable->addColumn<float>("GenPt",genPartAssoc_pt,"pt of gen particle matched to track",15);
  //   candTable->addColumn<float>("GenEta",genPartAssoc_eta, "eta of gen particle matched to track",15);
  //   candTable->addColumn<float>("GenPhi",genPartAssoc_phi, "phi of gen particle matched to track",15);
  //   candTable->addColumn<float>("GenMass",genPartAssoc_mass, "mass of gen particle matched to track",15);
  //   candTable->addColumn<int>("GenPdgId",genPartAssoc_pdgId, "pdgId of gen particle matched to track");
  // }

  iEvent.put(std::move(candTable), name_);
  iEvent.put(std::move(outCands));
}

void SimpleSelectedCandidateTableProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name", "SelectedPFCand");
  desc.add<std::string>("candIdxName", "PFCandIdx");
  desc.add<std::string>("candIdxDoc",  "Index in PFCand table");
  desc.add<edm::InputTag>("candidatesMain", edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("candidatesSelected", edm::InputTag("packedPFCandidates"));
  // desc.add<edm::InputTag>("pc2gp", edm::InputTag(""));
  descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(SimpleSelectedCandidateTableProducer);


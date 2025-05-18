#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

class SimplePatTauConstituentTableProducer : public edm::stream::EDProducer<> {
public:
  explicit SimplePatTauConstituentTableProducer(const edm::ParameterSet &);
  ~SimplePatTauConstituentTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  const std::string name_;
  const std::string candIdxName_;
  const std::string candIdxDoc_;

  edm::EDGetTokenT<edm::View<pat::Tau>> tau_token_;
  edm::EDGetTokenT<reco::CandidateView> cand_token_;

  const StringCutObjectSelector<pat::Tau> tauCut_;

  edm::Handle<reco::CandidateView> cands_;
};

//
// constructors and destructor
//
SimplePatTauConstituentTableProducer::SimplePatTauConstituentTableProducer(const edm::ParameterSet &iConfig):
  name_(iConfig.getParameter<std::string>("name")),
  candIdxName_(iConfig.getParameter<std::string>("candIdxName")),
  candIdxDoc_(iConfig.getParameter<std::string>("candIdxDoc")),
  tau_token_(consumes<edm::View<pat::Tau>>(iConfig.getParameter<edm::InputTag>("taus"))),
  cand_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"))),
  tauCut_(iConfig.getParameter<std::string>("tauCut"))
{
  produces<nanoaod::FlatTable>(name_);
  produces<std::vector<reco::CandidatePtr>>();
}

SimplePatTauConstituentTableProducer::~SimplePatTauConstituentTableProducer() {}

void SimplePatTauConstituentTableProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  // elements in all these collections must have the same order!
  auto outCands = std::make_unique<std::vector<reco::CandidatePtr>>();

  auto taus = iEvent.getHandle(tau_token_);

  iEvent.getByToken(cand_token_, cands_);
  auto candPtrs = cands_->ptrs();

  //
  // First, select taus
  //
  std::vector<pat::Tau> tausPassCut;
  for (unsigned tauIdx = 0; tauIdx < taus->size(); ++tauIdx) {
    const auto &tau = taus->at(tauIdx);
    if (!tauCut_(tau)) continue;
    tausPassCut.push_back(taus->at(tauIdx));
  }

  //
  // Then loop over selected taus
  //
  std::vector<int> parentTauIdx;
  std::vector<int> candIdx;
  std::vector<bool> candIsSignal;
  for (unsigned tauIdx = 0; tauIdx < tausPassCut.size(); ++tauIdx) {
    const auto &tau = tausPassCut.at(tauIdx);

    //
    // Loop over tau signal candidates
    //
    reco::CandidatePtrVector const & signalCands = tau.signalCands();
    for (const auto &cand : signalCands) {
      auto candInNewList = std::find( candPtrs.begin(), candPtrs.end(), cand );
      if ( candInNewList == candPtrs.end() ) {
        continue;
      }
      outCands->push_back(cand);
      parentTauIdx.push_back(tauIdx);
      candIsSignal.push_back(true);
      candIdx.push_back(candInNewList - candPtrs.begin());
    }

    //
    // Loop over tau isolation candidates
    //
    reco::CandidatePtrVector const & isolationCands = tau.isolationCands();
    for (const auto &cand : isolationCands) {
      auto candInNewList = std::find( candPtrs.begin(), candPtrs.end(), cand );
      if ( candInNewList == candPtrs.end() ) {
        continue;
      }
      outCands->push_back(cand);
      parentTauIdx.push_back(tauIdx);
      candIsSignal.push_back(false);
      candIdx.push_back(candInNewList - candPtrs.begin());
    }
  }// end tau loop

  auto candTable = std::make_unique<nanoaod::FlatTable>(outCands->size(), name_, false);
  // We fill from here only stuff that cannot be created with the SimpleFlatTableProducer
  candTable->addColumn<int>(candIdxName_, candIdx,      candIdxDoc_);
  candTable->addColumn<bool>("IsSignalCand", candIsSignal, "Is signalCand if true, is isolationCand if false");
  std::string parentTauIdxName("tauIdx");
  std::string parentTauIdxDoc("Index of the parent tau");
  candTable->addColumn<int>(parentTauIdxName, parentTauIdx, parentTauIdxDoc);

  iEvent.put(std::move(candTable), name_);
  iEvent.put(std::move(outCands));
}

void SimplePatTauConstituentTableProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name", "TauPFCand");
  desc.add<std::string>("candIdxName", "PFCandIdx");
  desc.add<std::string>("candIdxDoc", "Index in PFCand table");
  desc.add<edm::InputTag>("taus", edm::InputTag("slimmedTaus"));
  desc.add<edm::InputTag>("candidates", edm::InputTag("packedPFCandidates"));
  desc.add<std::string>("tauCut", "");
  descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(SimplePatTauConstituentTableProducer);

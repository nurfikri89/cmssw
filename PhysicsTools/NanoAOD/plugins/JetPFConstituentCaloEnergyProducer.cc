#include <string>
#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

template <typename T>
class JetPFConstituentCaloEnergyProducer : public edm::stream::EDProducer<> {
public:
  explicit JetPFConstituentCaloEnergyProducer(const edm::ParameterSet&);
  ~JetPFConstituentCaloEnergyProducer() override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  void PutValueMapInEvent(edm::Event&, const edm::Handle<edm::View<T>>&, const std::vector<float>&, std::string);

  const edm::EDGetTokenT<edm::View<T>> jet_token_;
  const edm::EDGetTokenT<pat::PackedCandidateCollection> pc_;
  const edm::EDGetTokenT<edm::Association<reco::PFCandidateCollection>> pc2pf_;
};

//
// constructors and destructor
//
template <typename T>
JetPFConstituentCaloEnergyProducer<T>::JetPFConstituentCaloEnergyProducer(const edm::ParameterSet& iConfig)
    : jet_token_(consumes<edm::View<T>>(iConfig.getParameter<edm::InputTag>("jets"))),
      pc_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
      pc2pf_(consumes<edm::Association<reco::PFCandidateCollection>>(iConfig.getParameter<edm::InputTag>("packedPFCandidates")))
  {
    produces<edm::ValueMap<float>>("ecalEnergy");
    produces<edm::ValueMap<float>>("hcalEnergy");
    produces<edm::ValueMap<float>>("hcalDepth1Energy");
    produces<edm::ValueMap<float>>("hcalDepth2Energy");
    produces<edm::ValueMap<float>>("hcalDepth3Energy");
    produces<edm::ValueMap<float>>("hcalDepth4Energy");
    produces<edm::ValueMap<float>>("hcalDepth5Energy");
    produces<edm::ValueMap<float>>("hcalDepth6Energy");
    produces<edm::ValueMap<float>>("hcalDepth7Energy");
    produces<edm::ValueMap<float>>("hoEnergy");
    produces<edm::ValueMap<float>>("ps1Energy");
    produces<edm::ValueMap<float>>("ps2Energy");

    produces<edm::ValueMap<float>>("ecalEnergyRaw");
    produces<edm::ValueMap<float>>("hcalEnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth1EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth2EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth3EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth4EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth5EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth6EnergyRaw");
    produces<edm::ValueMap<float>>("hcalDepth7EnergyRaw");

    produces<edm::ValueMap<float>>("hfHadEnergy");
    produces<edm::ValueMap<float>>("hfEmEnergy");
    produces<edm::ValueMap<float>>("hfShortEnergy");
    produces<edm::ValueMap<float>>("hfLongEnergy");

    // produces<edm::ValueMap<float>>("hfHadEnergyRaw");
    // produces<edm::ValueMap<float>>("hfEmEnergyRaw");
    // produces<edm::ValueMap<float>>("hfShortEnergyRaw");
    // produces<edm::ValueMap<float>>("hfLongEnergyRaw");

    produces<edm::ValueMap<float>>("ecalEnergyFromChgHad");
    produces<edm::ValueMap<float>>("hcalEnergyFromChgHad");
    produces<edm::ValueMap<float>>("hoEnergyFromChgHad");

    produces<edm::ValueMap<float>>("ecalEnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalEnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth1EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth2EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth3EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth4EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth5EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth6EnergyRawFromChgHad");
    produces<edm::ValueMap<float>>("hcalDepth7EnergyRawFromChgHad");

    produces<edm::ValueMap<float>>("hcalEnergyFromNeuHad");
    produces<edm::ValueMap<float>>("hoEnergyFromNeuHad");

    produces<edm::ValueMap<float>>("hcalEnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth1EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth2EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth3EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth4EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth5EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth6EnergyRawFromNeuHad");
    produces<edm::ValueMap<float>>("hcalDepth7EnergyRawFromNeuHad");

    produces<edm::ValueMap<float>>("ecalEnergyFromPhoton");
    produces<edm::ValueMap<float>>("ecalEnergyRawFromPhoton");
    produces<edm::ValueMap<float>>("ecalEnergyFromElectron");
    produces<edm::ValueMap<float>>("ecalEnergyRawFromElectron");

    produces<edm::ValueMap<float>>("ecalEnergyFromMuon");
    produces<edm::ValueMap<float>>("hcalEnergyFromMuon");
    produces<edm::ValueMap<float>>("hoEnergyFromMuon");

    produces<edm::ValueMap<float>>("ecalEnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalEnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth1EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth2EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth3EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth4EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth5EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth6EnergyRawFromMuon");
    produces<edm::ValueMap<float>>("hcalDepth7EnergyRawFromMuon");
}

template <typename T>
JetPFConstituentCaloEnergyProducer<T>::~JetPFConstituentCaloEnergyProducer() {}

template <typename T>
void JetPFConstituentCaloEnergyProducer<T>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // packedPFCandidate collection
  edm::Handle<pat::PackedCandidateCollection> pc_h;
  iEvent.getByToken(pc_, pc_h);

  // packedPFCandidates-->particleFlow(reco::PFCandidate) association
  edm::Handle<edm::Association<reco::PFCandidateCollection>> pc2pf;
  iEvent.getByToken(pc2pf_, pc2pf);

  // Input jets
  auto jets = iEvent.getHandle(jet_token_);

  std::vector<float> jet_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth1_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth2_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth3_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth4_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth5_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth6_energy(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth7_energy(jets->size(), 0.f);
  std::vector<float> jet_ho_energy(jets->size(), 0.f);
  std::vector<float> jet_pS1_energy(jets->size(), 0.f);
  std::vector<float> jet_pS2_energy(jets->size(), 0.f);

  std::vector<float> jet_ecal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth1_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth2_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth3_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth4_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth5_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth6_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_hcal_depth7_energy_raw(jets->size(), 0.f);

  std::vector<float> jet_hfem_energy(jets->size(), 0.f);
  std::vector<float> jet_hfhad_energy(jets->size(), 0.f);
  std::vector<float> jet_hfshort_energy(jets->size(), 0.f);
  std::vector<float> jet_hflong_energy(jets->size(), 0.f);

  // std::vector<float> jet_hfem_energy_raw(jets->size(), 0.f);
  // std::vector<float> jet_hfhad_energy_raw(jets->size(), 0.f);
  // std::vector<float> jet_hfshort_energy_raw(jets->size(), 0.f);
  // std::vector<float> jet_hflong_energy_raw(jets->size(), 0.f);

  /////////////////////////////////////////////////////////////////
  std::vector<float> jet_chgHad_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_energy(jets->size(), 0.f);
  std::vector<float> jet_chgHad_ho_energy(jets->size(), 0.f);

  std::vector<float> jet_chgHad_ecal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth1_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth2_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth3_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth4_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth5_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth6_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_chgHad_hcal_depth7_energy_raw(jets->size(), 0.f);

  /////////////////////////////////////////////////////////////////
  std::vector<float> jet_neuHad_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_energy(jets->size(), 0.f);
  std::vector<float> jet_neuHad_ho_energy(jets->size(), 0.f);

  std::vector<float> jet_neuHad_ecal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth1_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth2_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth3_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth4_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth5_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth6_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_neuHad_hcal_depth7_energy_raw(jets->size(), 0.f);

  /////////////////////////////////////////////////////////////////
  std::vector<float> jet_photon_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_photon_ecal_energy_raw(jets->size(), 0.f);

  std::vector<float> jet_electron_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_electron_ecal_energy_raw(jets->size(), 0.f);

  /////////////////////////////////////////////////////////////////
  std::vector<float> jet_muon_ecal_energy(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_energy(jets->size(), 0.f);
  std::vector<float> jet_muon_ho_energy(jets->size(), 0.f);

  std::vector<float> jet_muon_ecal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth1_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth2_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth3_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth4_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth5_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth6_energy_raw(jets->size(), 0.f);
  std::vector<float> jet_muon_hcal_depth7_energy_raw(jets->size(), 0.f);

  /////////////////////////////////////////////////////////////////

  // Loop over jet
  for (std::size_t jet_idx = 0; jet_idx < jets->size(); jet_idx++) {
    const auto& jet = (*jets)[jet_idx];

    float jet_energy_raw = jet.energy();
    if constexpr (std::is_same<T, pat::Jet>::value) {
      jet_energy_raw = jet.correctedJet(0).energy();
    }

    float ecal_energy = 0.f;
    float hcal_energy = 0.f;
    float hcal_depth1_energy = 0.f;
    float hcal_depth2_energy = 0.f;
    float hcal_depth3_energy = 0.f;
    float hcal_depth4_energy = 0.f;
    float hcal_depth5_energy = 0.f;
    float hcal_depth6_energy = 0.f;
    float hcal_depth7_energy = 0.f;
    float ho_energy = 0.f;
    float pS1_energy = 0.f;
    float pS2_energy = 0.f;

    float ecal_energy_raw = 0.f;
    float hcal_energy_raw = 0.f;
    float hcal_depth1_energy_raw = 0.f;
    float hcal_depth2_energy_raw = 0.f;
    float hcal_depth3_energy_raw = 0.f;
    float hcal_depth4_energy_raw = 0.f;
    float hcal_depth5_energy_raw = 0.f;
    float hcal_depth6_energy_raw = 0.f;
    float hcal_depth7_energy_raw = 0.f;

    float hfem_energy = 0.f;
    float hfhad_energy = 0.f;
    float hfshort_energy = 0.f;
    float hflong_energy = 0.f;

    // float hfem_energy_raw = 0.f;
    // float hfhad_energy_raw = 0.f;
    // float hfshort_energy_raw = 0.f;
    // float hflong_energy_raw = 0.f;

    //////////////////////////////////////////////////////
    float chgHad_ecal_energy = 0.f;
    float chgHad_hcal_energy = 0.f;
    float chgHad_ho_energy = 0.f;

    float chgHad_ecal_energy_raw = 0.f;
    float chgHad_hcal_energy_raw = 0.f;
    float chgHad_hcal_depth1_energy_raw = 0.f;
    float chgHad_hcal_depth2_energy_raw = 0.f;
    float chgHad_hcal_depth3_energy_raw = 0.f;
    float chgHad_hcal_depth4_energy_raw = 0.f;
    float chgHad_hcal_depth5_energy_raw = 0.f;
    float chgHad_hcal_depth6_energy_raw = 0.f;
    float chgHad_hcal_depth7_energy_raw = 0.f;

    //////////////////////////////////////////////////////
    float neuHad_hcal_energy = 0.f;
    float neuHad_ho_energy = 0.f;

    float neuHad_hcal_energy_raw = 0.f;
    float neuHad_hcal_depth1_energy_raw = 0.f;
    float neuHad_hcal_depth2_energy_raw = 0.f;
    float neuHad_hcal_depth3_energy_raw = 0.f;
    float neuHad_hcal_depth4_energy_raw = 0.f;
    float neuHad_hcal_depth5_energy_raw = 0.f;
    float neuHad_hcal_depth6_energy_raw = 0.f;
    float neuHad_hcal_depth7_energy_raw = 0.f;

    //////////////////////////////////////////////////////
    float photon_ecal_energy = 0.f;
    float photon_ecal_energy_raw = 0.f;

    float electron_ecal_energy = 0.f;
    float electron_ecal_energy_raw = 0.f;

    //////////////////////////////////////////////////////
    float muon_ecal_energy = 0.f;
    float muon_hcal_energy = 0.f;
    float muon_ho_energy = 0.f;

    float muon_ecal_energy_raw = 0.f;
    float muon_hcal_energy_raw = 0.f;
    float muon_hcal_depth1_energy_raw = 0.f;
    float muon_hcal_depth2_energy_raw = 0.f;
    float muon_hcal_depth3_energy_raw = 0.f;
    float muon_hcal_depth4_energy_raw = 0.f;
    float muon_hcal_depth5_energy_raw = 0.f;
    float muon_hcal_depth6_energy_raw = 0.f;
    float muon_hcal_depth7_energy_raw = 0.f;
    //////////////////////////////////////////////////////

    //
    // Loop over jet constituents
    //
    for (const reco::CandidatePtr& dau : jet.daughterPtrVector()) {
      pat::PackedCandidateRef pcref = pat::PackedCandidateRef(pc_h, dau.key());
      reco::PFCandidateRef pfref = (*pc2pf)[pcref];

      if((pfref.get()->pdgId() != 1) && (pfref.get()->pdgId() != 2)){
        ecal_energy += pfref.get()->ecalEnergy();
        hcal_energy += pfref.get()->hcalEnergy();
        hcal_depth1_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(1));
        hcal_depth2_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(2));
        hcal_depth3_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(3));
        hcal_depth4_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(4));
        hcal_depth5_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(5));
        hcal_depth6_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(6));
        hcal_depth7_energy += (pfref.get()->hcalEnergy() * pfref.get()->hcalDepthEnergyFraction(7));
        ho_energy += pfref.get()->hoEnergy();
        pS1_energy += pfref.get()->pS2Energy();
        pS2_energy += pfref.get()->pS2Energy();

        ecal_energy_raw += pfref.get()->rawEcalEnergy();
        hcal_energy_raw += pfref.get()->rawHcalEnergy();
        hcal_depth1_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(1));
        hcal_depth2_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(2));
        hcal_depth3_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(3));
        hcal_depth4_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(4));
        hcal_depth5_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(5));
        hcal_depth6_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(6));
        hcal_depth7_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(7));
      }else if (pfref.get()->pdgId() == 1 || pfref.get()->pdgId() == 2){
        hfem_energy  += pfref.get()->ecalEnergy();
        hfhad_energy += pfref.get()->hcalEnergy();
        float shortFibre_energy = pfref.get()->hcalEnergy() / 2.;
        float longFibre_energy  = pfref.get()->ecalEnergy() + shortFibre_energy;
        hfshort_energy += shortFibre_energy;
        hflong_energy  += longFibre_energy;

        // hfem_energy_raw  += pfref.get()->rawEcalEnergy();
        // hfhad_energy_raw += pfref.get()->rawHcalEnergy();
        // float shortFibre_energy_raw = pfref.get()->rawHcalEnergy() / 2.;
        // float longFibre_energy_raw  = pfref.get()->rawEcalEnergy() + shortFibre_energy_raw;
        // hfshort_energy_raw += shortFibre_energy_raw;
        // hflong_energy_raw  += longFibre_energy_raw;
      }
      if(abs(pfref.get()->pdgId()) == 211){
        chgHad_ecal_energy += pfref.get()->ecalEnergy();
        chgHad_hcal_energy += pfref.get()->hcalEnergy();
        chgHad_ho_energy += pfref.get()->hoEnergy();
        chgHad_ecal_energy_raw += pfref.get()->rawEcalEnergy();
        chgHad_hcal_energy_raw += pfref.get()->rawHcalEnergy();
        chgHad_hcal_depth1_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(1));
        chgHad_hcal_depth2_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(2));
        chgHad_hcal_depth3_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(3));
        chgHad_hcal_depth4_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(4));
        chgHad_hcal_depth5_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(5));
        chgHad_hcal_depth6_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(6));
        chgHad_hcal_depth7_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(7));
      }
      else if(abs(pfref.get()->pdgId()) == 130){
        neuHad_hcal_energy += pfref.get()->hcalEnergy();
        neuHad_ho_energy += pfref.get()->hoEnergy();
        neuHad_hcal_energy_raw += pfref.get()->rawHcalEnergy();
        neuHad_hcal_depth1_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(1));
        neuHad_hcal_depth2_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(2));
        neuHad_hcal_depth3_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(3));
        neuHad_hcal_depth4_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(4));
        neuHad_hcal_depth5_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(5));
        neuHad_hcal_depth6_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(6));
        neuHad_hcal_depth7_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(7));
      }
      else if(abs(pfref.get()->pdgId()) == 22){
        photon_ecal_energy += pfref.get()->ecalEnergy();
        photon_ecal_energy_raw += pfref.get()->rawEcalEnergy();
      }
      else if(abs(pfref.get()->pdgId()) == 11){
        electron_ecal_energy += pfref.get()->ecalEnergy();
        electron_ecal_energy_raw += pfref.get()->rawEcalEnergy();
      }
      else if(abs(pfref.get()->pdgId()) == 13){
        muon_ecal_energy += pfref.get()->ecalEnergy();
        muon_hcal_energy += pfref.get()->hcalEnergy();
        muon_ho_energy += pfref.get()->hoEnergy();
        muon_ecal_energy_raw += pfref.get()->rawEcalEnergy();
        muon_hcal_energy_raw += pfref.get()->rawHcalEnergy();
        muon_hcal_depth1_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(1));
        muon_hcal_depth2_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(2));
        muon_hcal_depth3_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(3));
        muon_hcal_depth4_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(4));
        muon_hcal_depth5_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(5));
        muon_hcal_depth6_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(6));
        muon_hcal_depth7_energy_raw += (pfref.get()->rawHcalEnergy() * pfref.get()->hcalDepthEnergyFraction(7));
      }

      jet_ecal_energy[jet_idx] = ecal_energy;
      jet_hcal_energy[jet_idx] = hcal_energy;
      jet_hcal_depth1_energy[jet_idx] = hcal_depth1_energy;
      jet_hcal_depth2_energy[jet_idx] = hcal_depth2_energy;
      jet_hcal_depth3_energy[jet_idx] = hcal_depth3_energy;
      jet_hcal_depth4_energy[jet_idx] = hcal_depth4_energy;
      jet_hcal_depth5_energy[jet_idx] = hcal_depth5_energy;
      jet_hcal_depth6_energy[jet_idx] = hcal_depth6_energy;
      jet_hcal_depth7_energy[jet_idx] = hcal_depth7_energy;
      jet_ho_energy[jet_idx] = ho_energy;
      jet_pS1_energy[jet_idx] = pS1_energy;
      jet_pS2_energy[jet_idx] = pS2_energy;

      jet_ecal_energy_raw[jet_idx] = ecal_energy_raw;
      jet_hcal_energy_raw[jet_idx] = hcal_energy_raw;
      jet_hcal_depth1_energy_raw[jet_idx] = hcal_depth1_energy_raw;
      jet_hcal_depth2_energy_raw[jet_idx] = hcal_depth2_energy_raw;
      jet_hcal_depth3_energy_raw[jet_idx] = hcal_depth3_energy_raw;
      jet_hcal_depth4_energy_raw[jet_idx] = hcal_depth4_energy_raw;
      jet_hcal_depth5_energy_raw[jet_idx] = hcal_depth5_energy_raw;
      jet_hcal_depth6_energy_raw[jet_idx] = hcal_depth6_energy_raw;
      jet_hcal_depth7_energy_raw[jet_idx] = hcal_depth7_energy_raw;

      jet_hfem_energy[jet_idx] = hfem_energy;
      jet_hfhad_energy[jet_idx] = hfhad_energy;
      jet_hfshort_energy[jet_idx] = hfshort_energy;
      jet_hflong_energy[jet_idx] = hflong_energy;

      // jet_hfem_energy_raw[jet_idx] = hfem_energy_raw;
      // jet_hfhad_energy_raw[jet_idx] = hfhad_energy_raw;
      // jet_hfshort_energy_raw[jet_idx] = hfshort_energy_raw;
      // jet_hflong_energy_raw[jet_idx] = hflong_energy_raw;

      jet_chgHad_ecal_energy[jet_idx] = chgHad_ecal_energy;
      jet_chgHad_hcal_energy[jet_idx] = chgHad_hcal_energy;
      jet_chgHad_ho_energy[jet_idx] = chgHad_ho_energy;
      jet_chgHad_ecal_energy_raw[jet_idx] = chgHad_ecal_energy_raw;
      jet_chgHad_hcal_energy_raw[jet_idx] = chgHad_hcal_energy_raw;
      jet_chgHad_hcal_depth1_energy_raw[jet_idx] = chgHad_hcal_depth1_energy_raw;
      jet_chgHad_hcal_depth2_energy_raw[jet_idx] = chgHad_hcal_depth2_energy_raw;
      jet_chgHad_hcal_depth3_energy_raw[jet_idx] = chgHad_hcal_depth3_energy_raw;
      jet_chgHad_hcal_depth4_energy_raw[jet_idx] = chgHad_hcal_depth4_energy_raw;
      jet_chgHad_hcal_depth5_energy_raw[jet_idx] = chgHad_hcal_depth5_energy_raw;
      jet_chgHad_hcal_depth6_energy_raw[jet_idx] = chgHad_hcal_depth6_energy_raw;
      jet_chgHad_hcal_depth7_energy_raw[jet_idx] = chgHad_hcal_depth7_energy_raw;
      //////////////////////////////////////////////
      jet_neuHad_hcal_energy[jet_idx] = neuHad_hcal_energy;
      jet_neuHad_ho_energy[jet_idx] = neuHad_ho_energy;
      jet_neuHad_hcal_energy_raw[jet_idx] = neuHad_hcal_energy_raw;
      jet_neuHad_hcal_depth1_energy_raw[jet_idx] = neuHad_hcal_depth1_energy_raw;
      jet_neuHad_hcal_depth2_energy_raw[jet_idx] = neuHad_hcal_depth2_energy_raw;
      jet_neuHad_hcal_depth3_energy_raw[jet_idx] = neuHad_hcal_depth3_energy_raw;
      jet_neuHad_hcal_depth4_energy_raw[jet_idx] = neuHad_hcal_depth4_energy_raw;
      jet_neuHad_hcal_depth5_energy_raw[jet_idx] = neuHad_hcal_depth5_energy_raw;
      jet_neuHad_hcal_depth6_energy_raw[jet_idx] = neuHad_hcal_depth6_energy_raw;
      jet_neuHad_hcal_depth7_energy_raw[jet_idx] = neuHad_hcal_depth7_energy_raw;
      //////////////////////////////////////////////
      jet_photon_ecal_energy[jet_idx] = photon_ecal_energy;
      jet_photon_ecal_energy_raw[jet_idx] = photon_ecal_energy_raw;
      jet_electron_ecal_energy[jet_idx] = electron_ecal_energy;
      jet_electron_ecal_energy_raw[jet_idx] = electron_ecal_energy_raw;
      //////////////////////////////////////////////
      jet_muon_ecal_energy[jet_idx] = muon_ecal_energy;
      jet_muon_hcal_energy[jet_idx] = muon_hcal_energy;
      jet_muon_ho_energy[jet_idx] = muon_ho_energy;
      jet_muon_ecal_energy_raw[jet_idx] = muon_ecal_energy_raw;
      jet_muon_hcal_energy_raw[jet_idx] = muon_hcal_energy_raw;
      jet_muon_hcal_depth1_energy_raw[jet_idx] = muon_hcal_depth1_energy_raw;
      jet_muon_hcal_depth2_energy_raw[jet_idx] = muon_hcal_depth2_energy_raw;
      jet_muon_hcal_depth3_energy_raw[jet_idx] = muon_hcal_depth3_energy_raw;
      jet_muon_hcal_depth4_energy_raw[jet_idx] = muon_hcal_depth4_energy_raw;
      jet_muon_hcal_depth5_energy_raw[jet_idx] = muon_hcal_depth5_energy_raw;
      jet_muon_hcal_depth6_energy_raw[jet_idx] = muon_hcal_depth6_energy_raw;
      jet_muon_hcal_depth7_energy_raw[jet_idx] = muon_hcal_depth7_energy_raw;

    }
  }

  PutValueMapInEvent(iEvent, jets, jet_ecal_energy, "ecalEnergy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_energy, "hcalEnergy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth1_energy, "hcalDepth1Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth2_energy, "hcalDepth2Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth3_energy, "hcalDepth3Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth4_energy, "hcalDepth4Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth5_energy, "hcalDepth5Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth6_energy, "hcalDepth6Energy");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth7_energy, "hcalDepth7Energy");
  PutValueMapInEvent(iEvent, jets, jet_ho_energy, "hoEnergy");
  PutValueMapInEvent(iEvent, jets, jet_pS1_energy, "ps1Energy");
  PutValueMapInEvent(iEvent, jets, jet_pS2_energy, "ps2Energy");

  PutValueMapInEvent(iEvent, jets, jet_ecal_energy_raw, "ecalEnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_energy_raw, "hcalEnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth1_energy_raw, "hcalDepth1EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth2_energy_raw, "hcalDepth2EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth3_energy_raw, "hcalDepth3EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth4_energy_raw, "hcalDepth4EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth5_energy_raw, "hcalDepth5EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth6_energy_raw, "hcalDepth6EnergyRaw");
  PutValueMapInEvent(iEvent, jets, jet_hcal_depth7_energy_raw, "hcalDepth7EnergyRaw");

  PutValueMapInEvent(iEvent, jets, jet_hfem_energy, "hfEmEnergy");
  PutValueMapInEvent(iEvent, jets, jet_hfhad_energy, "hfHadEnergy");
  PutValueMapInEvent(iEvent, jets, jet_hfshort_energy, "hfShortEnergy");
  PutValueMapInEvent(iEvent, jets, jet_hflong_energy, "hfLongEnergy");

  // PutValueMapInEvent(iEvent, jets, jet_hfem_energy_raw, "hfEmEnergyRaw");
  // PutValueMapInEvent(iEvent, jets, jet_hfhad_energy_raw, "hfHadEnergyRaw");
  // PutValueMapInEvent(iEvent, jets, jet_hfshort_energy_raw, "hfShortEnergyRaw");
  // PutValueMapInEvent(iEvent, jets, jet_hflong_energy_raw, "hfLongEnergyRaw");

  PutValueMapInEvent(iEvent, jets, jet_chgHad_ecal_energy,"ecalEnergyFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_energy,"hcalEnergyFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_ho_energy,"hoEnergyFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_ecal_energy_raw,"ecalEnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_energy_raw,"hcalEnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth1_energy_raw,"hcalDepth1EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth2_energy_raw,"hcalDepth2EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth3_energy_raw,"hcalDepth3EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth4_energy_raw,"hcalDepth4EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth5_energy_raw,"hcalDepth5EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth6_energy_raw,"hcalDepth6EnergyRawFromChgHad");
  PutValueMapInEvent(iEvent, jets, jet_chgHad_hcal_depth7_energy_raw,"hcalDepth7EnergyRawFromChgHad");

  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_energy,"hcalEnergyFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_ho_energy,"hoEnergyFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_energy_raw,"hcalEnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth1_energy_raw,"hcalDepth1EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth2_energy_raw,"hcalDepth2EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth3_energy_raw,"hcalDepth3EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth4_energy_raw,"hcalDepth4EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth5_energy_raw,"hcalDepth5EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth6_energy_raw,"hcalDepth6EnergyRawFromNeuHad");
  PutValueMapInEvent(iEvent, jets, jet_neuHad_hcal_depth7_energy_raw,"hcalDepth7EnergyRawFromNeuHad");

  PutValueMapInEvent(iEvent, jets, jet_photon_ecal_energy,"ecalEnergyFromPhoton");
  PutValueMapInEvent(iEvent, jets, jet_photon_ecal_energy_raw,"ecalEnergyRawFromPhoton");
  PutValueMapInEvent(iEvent, jets, jet_electron_ecal_energy,"ecalEnergyFromElectron");
  PutValueMapInEvent(iEvent, jets, jet_electron_ecal_energy_raw,"ecalEnergyRawFromElectron");

  PutValueMapInEvent(iEvent, jets, jet_muon_ecal_energy,"ecalEnergyFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_energy,"hcalEnergyFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_ho_energy,"hoEnergyFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_ecal_energy_raw,"ecalEnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_energy_raw,"hcalEnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth1_energy_raw,"hcalDepth1EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth2_energy_raw,"hcalDepth2EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth3_energy_raw,"hcalDepth3EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth4_energy_raw,"hcalDepth4EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth5_energy_raw,"hcalDepth5EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth6_energy_raw,"hcalDepth6EnergyRawFromMuon");
  PutValueMapInEvent(iEvent, jets, jet_muon_hcal_depth7_energy_raw,"hcalDepth7EnergyRawFromMuon");
}
template <typename T>
void JetPFConstituentCaloEnergyProducer<T>::PutValueMapInEvent(edm::Event& iEvent,
                                                        const edm::Handle<edm::View<T>>& coll,
                                                        const std::vector<float>& vec_var,
                                                        std::string VMName) {
  std::unique_ptr<edm::ValueMap<float>> VM(new edm::ValueMap<float>());
  edm::ValueMap<float>::Filler fillerVM(*VM);
  fillerVM.insert(coll, vec_var.begin(), vec_var.end());
  fillerVM.fill();
  iEvent.put(std::move(VM), VMName);
}

template <typename T>
void JetPFConstituentCaloEnergyProducer<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jets", edm::InputTag("finalJetsPuppi"));
  desc.add<edm::InputTag>("packedPFCandidates", edm::InputTag("packedPFCandidates"));
  descriptions.addWithDefaultLabel(desc);
}

typedef JetPFConstituentCaloEnergyProducer<pat::Jet> PatJetPFConstituentCaloEnergyProducer;
DEFINE_FWK_MODULE(PatJetPFConstituentCaloEnergyProducer);
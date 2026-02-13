// -*- C++ -*-
//
// Package:    JetFlavourClustering
// Class:      JetFlavourClustering
//
/**\class JetFlavourClustering JetFlavourClustering.cc PhysicsTools/JetMCAlgos/plugins/JetFlavourClustering.cc
 * \brief Clusters hadrons, partons, and jet contituents to determine the jet flavour
 *
 * This producer clusters hadrons, partons and jet contituents to determine the jet flavour. The jet flavour information
 * is stored in the event as an AssociationVector which associates an object of type JetFlavourInfo to each of the jets.
 *
 * The producer takes as input jets and hadron and partons selected by the HadronAndPartonSelector producer. The hadron
 * and parton four-momenta are rescaled by a very small number (default rescale factor is 10e-18) which turns them into
 * the so-called "ghosts". The "ghost" hadrons and partons are clustered together with all of the jet constituents. It is
 * important to use the same clustering algorithm and jet size as for the original input jet collection. Since the
 * "ghost" hadrons and partons are extremely soft, the resulting jet collection will be practically identical to the
 * original one but now with "ghost" hadrons and partons clustered inside jets. The jet flavour is determined based on
 * the "ghost" hadrons clustered inside a jet:
 *
 * - jet is considered a b jet if there is at least one b "ghost" hadron clustered inside it (hadronFlavour = 5)
 * 
 * - jet is considered a c jet if there is at least one c and no b "ghost" hadrons clustered inside it (hadronFlavour = 4)
 * 
 * - jet is considered a light-flavour jet if there are no b or c "ghost" hadrons clustered inside it (hadronFlavour = 0)
 *
 * To further assign a more specific flavour to light-flavour jets, "ghost" partons are used:
 *
 * - jet is considered a b jet if there is at least one b "ghost" parton clustered inside it (partonFlavour = 5)
 * 
 * - jet is considered a c jet if there is at least one c and no b "ghost" partons clustered inside it (partonFlavour = 4)
 * 
 * - jet is considered a light-flavour jet if there are light-flavour and no b or c "ghost" partons clustered inside it.
 *   The jet is assigned the flavour of the hardest light-flavour "ghost" parton clustered inside it (partonFlavour = 1, 2, 3, or 21)
 * 
 * - jet has an undefined flavour if there are no "ghost" partons clustered inside it (partonFlavour = 0)
 *
 * In rare instances a conflict between the hadron- and parton-based flavours can occur. In such cases it is possible to
 * keep both flavours or to give priority to the hadron-based flavour. This is controlled by the 'hadronFlavourHasPriority'
 * switch. The priority is given to the hadron-based flavour as follows:
 * 
 * - if hadronFlavour==0 && (partonFlavour==4 || partonFlavour==5): partonFlavour is set to the flavour of the hardest
 *   light-flavour parton clustered inside the jet if such parton exists. Otherwise, the parton flavour is left undefined
 * 
 * - if hadronFlavour!=0 && hadronFlavour!=partonFlavour: partonFlavour is set equal to hadronFlavour
 *
 * The producer is also capable of assigning the flavour to subjets of fat jets, in which case it produces an additional
 * AssociationVector providing the flavour information for subjets. In order to assign the flavour to subjets, three input
 * jet collections are required:
 *
 * - jets, in this case represented by fat jets
 * 
 * - groomed jets, which is a collection of fat jets from which the subjets are derived (e.g. pruned, filtered, soft drop, top-tagged, etc. jets)
 * 
 * - subjets, derived from the groomed fat jets
 *
 * The "ghost" hadrons and partons clustered inside a fat jet are assigned to the closest subjet in the rapidity-phi
 * space. Once hadrons and partons have been assigned to subjets, the subjet flavour is determined in the same way as for
 * jets. The reason for requiring three jet collections as input in order to determine the subjet flavour is to avoid
 * possible inconsistencies between the fat jet and subjet flavours (such as a non-b fat jet having a b subjet and vice
 * versa) as well as the fact that re-clustering the constituents of groomed fat jets will generally result in a jet
 * collection different from the input groomed fat jets. Also note that "ghost" particles generally cannot be clustered
 * inside subjets in the same way this is done for fat jets. This is because some of the jet grooming techniques could
 * reject such very soft particle. So instead, the "ghost" particles are assigned to the closest subjet.
 * 
 * Finally, "ghost" leptons can also be clustered inside jets but they are not used in any way to determine the jet
 * flavour. This functionality is optional and is potentially useful to identify jets from hadronic taus.
 * 
 * For more details, please refer to
 * https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools
 * 
 */
//
// Original Author:  Dinko Ferencek
//         Created:  Wed Nov  6 00:49:55 CET 2013
//
//

// system include files
#include <memory>
#include <iomanip>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "DataFormats/JetMatching/interface/JetFlavourInfoMatching.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#include "fastjet/PseudoJet.hh"

#include "PhysicsTools/JetMCAlgos/interface/GHSAlgo.h"
#include "fastjet/contrib/FlavInfo.hh"

#include "fastjet/NNH.hh"

//
// constants, enums and typedefs
//
typedef std::shared_ptr<fastjet::ClusterSequence> ClusterSequencePtr;
typedef std::shared_ptr<fastjet::JetDefinition> JetDefPtr;

//
// class declaration
//
class GhostInfo : public fastjet::PseudoJet::UserInfoBase {
public:
  GhostInfo(const bool& isHadron,
            const bool& isbHadron,
            const bool& isParton,
            const bool& isLepton,
            const reco::GenParticleRef& particleRef)
      : m_particleRef(particleRef) {
    m_type = 0;
    if (isHadron)
      m_type |= (1 << 0);
    if (isbHadron)
      m_type |= (1 << 1);
    if (isParton)
      m_type |= (1 << 2);
    if (isLepton)
      m_type |= (1 << 3);
  }

  const bool isHadron() const { return (m_type & (1 << 0)); }
  const bool isbHadron() const { return (m_type & (1 << 1)); }
  const bool isParton() const { return (m_type & (1 << 2)); }
  const bool isLepton() const { return (m_type & (1 << 3)); }
  const reco::GenParticleRef& particleRef() const { return m_particleRef; }

protected:
  const reco::GenParticleRef m_particleRef;
  int m_type;
};

class GhostFinalPartonInfo : public GhostInfo {
public:
  GhostFinalPartonInfo(const reco::GenParticleRef& particleRef, const bool& isFinalParton)
      : GhostInfo(false, false, true, false, particleRef) {
    m_type |= (1 << 4);  // Set the final parton flag
  }
  const bool isFinalParton() const { return (m_type & (1 << 4)); }  // Check if this is a final parton
};

class JetFlavourClustering : public edm::stream::EDProducer<> {
public:
  explicit JetFlavourClustering(const edm::ParameterSet&);
  ~JetFlavourClustering() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  void insertGhosts(const edm::Handle<reco::GenParticleRefVector>& particles,
                    const double ghostRescaling,
                    const bool isHadron,
                    const bool isbHadron,
                    const bool isParton,
                    const bool isLepton,
                    std::vector<fastjet::PseudoJet>& constituents);

  void insertGhostFinalPartons(const edm::Handle<reco::GenParticleRefVector>& particles,
                               const double ghostRescaling,
                               std::vector<fastjet::PseudoJet>& constituents,
                               const bool& ghostInfoHasPriority = false);

  void matchReclusteredJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                            const std::vector<fastjet::PseudoJet>& matchedJets,
                            std::vector<int>& matchedIndices);
  void matchGroomedJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                        const edm::Handle<edm::View<reco::Jet>>& matchedJets,
                        std::vector<int>& matchedIndices);
  void matchSubjets(const std::vector<int>& groomedIndices,
                    const edm::Handle<edm::View<reco::Jet>>& groomedJets,
                    const edm::Handle<edm::View<reco::Jet>>& subjets,
                    std::vector<std::vector<int>>& matchedIndices);

  void setFlavours(const reco::GenParticleRefVector& clusteredbHadrons,
                   const reco::GenParticleRefVector& clusteredcHadrons,
                   const reco::GenParticleRefVector& clusteredPartons,
                   int& hadronFlavour,
                   int& partonFlavour);

  // A "full-chain" function to make GHS algorithm jets
  void makeGHSAlgoJets(const edm::Handle<edm::View<reco::Jet>>& inputGenJets,
                       const edm::Handle<reco::GenParticleRefVector>& inputGenPartons,
                       std::vector<int>& matchedIndices,
                       std::vector<fastjet::PseudoJet>& outputGHSAlgoJets);
  void makeGHSFullAlgoJets(const edm::Handle<edm::View<reco::Jet>>& inputGenJets,
                           const edm::Handle<reco::GenParticleRefVector>& inputGenPartons,
                           const edm::Handle<edm::ValueMap<float>>& inputWeights,
                           std::vector<int>& matchedIndices,
                           std::vector<fastjet::PseudoJet>& outputGHSFullAlgoJets);
  void GHSCore(const std::vector<fastjet::PseudoJet>& inputJets,
               const std::vector<fastjet::PseudoJet>& inputFlavParticles,
                     std::vector<fastjet::PseudoJet>& outputGHSAlgoJets);

  bool isFinalParton(const reco::GenParticleRef& particle) const;

  void makeFinalPartonSet(const reco::GenParticleRefVector& particles, reco::GenParticleRefVector& finalPartons);

  void assignToSubjets(const reco::GenParticleRefVector& clusteredParticles,
                       const edm::Handle<edm::View<reco::Jet>>& subjets,
                       const std::vector<int>& subjetIndices,
                       std::vector<reco::GenParticleRefVector>& assignedParticles);

  // ----------member data ---------------------------
  const edm::EDGetTokenT<edm::View<reco::Jet>> jetsToken_;            // Input jet collection
  edm::EDGetTokenT<edm::View<reco::Jet>> groomedJetsToken_;           // Input groomed jet collection
  edm::EDGetTokenT<edm::View<reco::Jet>> subjetsToken_;               // Input subjet collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> bHadronsToken_;  // Input b hadron collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> cHadronsToken_;  // Input c hadron collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> partonsToken_;   // Input parton collection
  edm::EDGetTokenT<edm::ValueMap<float>> weightsToken_;               // Input weights collection
  edm::EDGetTokenT<reco::GenParticleRefVector> leptonsToken_;         // Input lepton collection

  const std::string jetAlgorithm_;
  const double rParam_;
  const double jetPtMin_;
  const double ghostRescaling_;
  const double relPtTolerance_;
  const bool hadronFlavourHasPriority_;

  /// New flavour definition algorithm-specific parameters
  /// - GHS algorithm-specific parameters
  const double ghsAlgoAlpha_;
  const double ghsAlgoOmega_;
  const double ghsAlgoPtMin_;  // Minimum Pt for GHS algorithm jets, default is 0.0
  /// Choosing the flavour summation scheme
  const std::string ghsAlgoFlavSummationSchemeStr_;
  std::shared_ptr<fastjet::contrib::FlavRecombiner> ghsAlgoFlavRecombinerPtr_;
  /// Flags for new flavour definition algorithm
  /// - General logic: require explicit configuration; will double-check if full gen particles collection is given.
  const bool enableGHSAlgoFlavour_;

  const bool useSubjets_;

  const bool useLeptons_;

  /// Flag for introducing full gen Particle collection, only needed when some new jet flavour definition is used.
  const bool enableAlgoFlav_;

  ClusterSequencePtr fjClusterSeq_;
  JetDefPtr fjJetDefinition_;
};

//
// static data member definitions
//

//
// constructors and destructor
//
JetFlavourClustering::JetFlavourClustering(const edm::ParameterSet& iConfig)
    : jetsToken_(consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
      bHadronsToken_(consumes<reco::GenParticleRefVector>(iConfig.getParameter<edm::InputTag>("bHadrons"))),
      cHadronsToken_(consumes<reco::GenParticleRefVector>(iConfig.getParameter<edm::InputTag>("cHadrons"))),
      partonsToken_(consumes<reco::GenParticleRefVector>(iConfig.getParameter<edm::InputTag>("partons"))),
      /// Input gen particles collection, only needed when some new jet flavour definition is used.
      jetAlgorithm_(iConfig.getParameter<std::string>("jetAlgorithm")),
      rParam_(iConfig.getParameter<double>("rParam")),

      jetPtMin_(
          0.),  // hardcoded to 0. since we simply want to recluster all input jets which already had some PtMin applied
      ghostRescaling_(iConfig.exists("ghostRescaling") ? iConfig.getParameter<double>("ghostRescaling") : 1e-18),
      relPtTolerance_(
          iConfig.exists("relPtTolerance")
              ? iConfig.getParameter<double>("relPtTolerance")
              : 1e-03),  // 0.1% relative difference in Pt should be sufficient to detect possible misconfigurations
      hadronFlavourHasPriority_(iConfig.getParameter<bool>("hadronFlavourHasPriority")),

      /// New jet flavour definition algorithm-specific parameters
      ghsAlgoAlpha_(iConfig.exists("ghsAlgoAlpha") ? iConfig.getParameter<double>("ghsAlgoAlpha") : 1.0),
      ghsAlgoOmega_(iConfig.exists("ghsAlgoOmega") ? iConfig.getParameter<double>("ghsAlgoOmega") : 2.0),
      ghsAlgoPtMin_(iConfig.exists("ghsAlgoPtMin") ? iConfig.getParameter<double>("ghsAlgoPtMin")
                                                   : 1e6 * ghostRescaling_),
      /// Choosing the flavour summation scheme for Fastjet-contrib Flavour Definition algorithms
      ghsAlgoFlavSummationSchemeStr_(iConfig.exists("ghsAlgoFlavSummationScheme")
                                         ? iConfig.getParameter<std::string>("ghsAlgoFlavSummationScheme")
                                         : "net_flav"),
      /// Flags for enabling new flavour definition algorithm. Double-check if full gen particles collection is given.
      enableGHSAlgoFlavour_(iConfig.exists("enableGHSAlgoFlavour") ? iConfig.getParameter<bool>("enableGHSAlgoFlavour")
                                                                   : true),

      useSubjets_(iConfig.exists("groomedJets") && iConfig.exists("subjets")),
      useLeptons_(iConfig.exists("leptons")),
      enableAlgoFlav_(enableGHSAlgoFlavour_)

{
  // register your products
  produces<reco::JetFlavourInfoMatchingCollection>();
  if (iConfig.existsAs<edm::InputTag>("weights"))
    weightsToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("weights"));

  if (useSubjets_)
    produces<reco::JetFlavourInfoMatchingCollection>("SubJets");

  // set jet algorithm
  if (jetAlgorithm_ == "Kt")
    fjJetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::kt_algorithm, rParam_);
  else if (jetAlgorithm_ == "CambridgeAachen")
    fjJetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::cambridge_algorithm, rParam_);
  else if (jetAlgorithm_ == "AntiKt")
    fjJetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::antikt_algorithm, rParam_);
  else
    throw cms::Exception("InvalidJetAlgorithm") << "Jet clustering algorithm is invalid: " << jetAlgorithm_
                                                << ", use CambridgeAachen | Kt | AntiKt" << std::endl;
  // set the flavour recombiner
  if (enableGHSAlgoFlavour_) {
    if (ghsAlgoFlavSummationSchemeStr_ == "net_flav") {
      ghsAlgoFlavRecombinerPtr_ =
          std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::net);
    } else if (ghsAlgoFlavSummationSchemeStr_ == "mod2_flav") {
      ghsAlgoFlavRecombinerPtr_ =
          std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::modulo_2);
    } else if (ghsAlgoFlavSummationSchemeStr_ == "any_flav") {
      ghsAlgoFlavRecombinerPtr_ =
          std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::any_abs);
    } else {
      throw cms::Exception("InvalidGHSAlgoFlavourSummationScheme")
          << "GHS flavour summation scheme is invalid: " << ghsAlgoFlavSummationSchemeStr_
          << ", use net_flav | mod2_flav | any_flav" << std::endl;
    }
  }

  if (useSubjets_) {
    groomedJetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("groomedJets"));
    subjetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("subjets"));
  }
  if (useLeptons_) {
    leptonsToken_ = consumes<reco::GenParticleRefVector>(iConfig.getParameter<edm::InputTag>("leptons"));
  }
}

JetFlavourClustering::~JetFlavourClustering() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void JetFlavourClustering::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<edm::View<reco::Jet>> jets;
  iEvent.getByToken(jetsToken_, jets);

  edm::Handle<edm::View<reco::Jet>> groomedJets;
  edm::Handle<edm::View<reco::Jet>> subjets;
  if (useSubjets_) {
    iEvent.getByToken(groomedJetsToken_, groomedJets);
    iEvent.getByToken(subjetsToken_, subjets);
  }

  edm::Handle<reco::GenParticleRefVector> bHadrons;
  iEvent.getByToken(bHadronsToken_, bHadrons);

  edm::Handle<reco::GenParticleRefVector> cHadrons;
  iEvent.getByToken(cHadronsToken_, cHadrons);

  edm::Handle<reco::GenParticleRefVector> partons;
  iEvent.getByToken(partonsToken_, partons);

  edm::Handle<edm::ValueMap<float>> weights;
  if (!weightsToken_.isUninitialized())
    iEvent.getByToken(weightsToken_, weights);

  edm::Handle<reco::GenParticleRefVector> leptons;
  if (useLeptons_)
    iEvent.getByToken(leptonsToken_, leptons);

  auto jetFlavourInfos = std::make_unique<reco::JetFlavourInfoMatchingCollection>(reco::JetRefBaseProd(jets));
  std::unique_ptr<reco::JetFlavourInfoMatchingCollection> subjetFlavourInfos;
  if (useSubjets_)
    subjetFlavourInfos = std::make_unique<reco::JetFlavourInfoMatchingCollection>(reco::JetRefBaseProd(subjets));

  // vector of constituents for reclustering jets and "ghosts"
  std::vector<fastjet::PseudoJet> fjInputs;
  std::vector<fastjet::PseudoJet> fjInputsForGHSAlgo;
  std::vector<fastjet::PseudoJet> fjGHSAlgoJetResults;
  std::vector<fastjet::PseudoJet> fjGHSFullAlgoJetResults;
  std::vector<int> fjGHSAlgoJetMatchingIndices;
  std::vector<int> fjGHSFullAlgoJetMatchingIndices;
  unsigned int reserve = jets->size() * 128 + bHadrons->size() + cHadrons->size() + partons->size();
  if (useLeptons_)
    reserve += leptons->size();
  fjInputs.reserve(reserve);
  // loop over all input jets and collect all their constituents
  for (edm::View<reco::Jet>::const_iterator it = jets->begin(); it != jets->end(); ++it) {
    std::vector<edm::Ptr<reco::Candidate>> constituents = it->getJetConstituents();
    std::vector<edm::Ptr<reco::Candidate>>::const_iterator m;
    for (m = constituents.begin(); m != constituents.end(); ++m) {
      const reco::CandidatePtr& constit = *m;
      if (!constit.isNonnull() || !constit.isAvailable()) {
        edm::LogError("MissingJetConstituent") << "Jet constituent required for jet reclustering is missing. "
                                                  "Reclustered jets are not guaranteed to reproduce the original jets!";
        continue;
      }
      if (constit->pt() == 0) {
        edm::LogWarning("NullTransverseMomentum") << "dropping input candidate with pt=0";
        continue;
      }
      if (it->isWeighted()) {
        if (weightsToken_.isUninitialized())
          throw cms::Exception("MissingConstituentWeight")
              << "JetFlavourClustering: No weights (e.g. PUPPI) given for weighted jet collection" << std::endl;
        float w = (*weights)[constit];
        fjInputs.push_back(
            fastjet::PseudoJet(constit->px() * w, constit->py() * w, constit->pz() * w, constit->energy() * w));
      } else {
        fjInputs.push_back(fastjet::PseudoJet(constit->px(), constit->py(), constit->pz(), constit->energy()));
      }
    }
  }
  // copy the jets for GHS algorithm and add the "ghost" final partons
  if (enableGHSAlgoFlavour_) {
    // insert "ghost" final partons in the vector of constituents
    insertGhostFinalPartons(partons, ghostRescaling_, fjInputsForGHSAlgo);
    // Produce the GHS algorithm flavoured jets with refactored code.
    // std::cout << "[DEBUG] GHS algorithm jets producing:" << std::endl;
    makeGHSAlgoJets(jets, partons, fjGHSAlgoJetMatchingIndices, fjGHSAlgoJetResults);
    // std::cout << "[DEBUG] GHS full algorithm jets producing:" << std::endl;
    makeGHSFullAlgoJets(jets, partons, weights, fjGHSFullAlgoJetMatchingIndices, fjGHSFullAlgoJetResults);
  }
  // insert "ghost" b hadrons in the vector of constituents
  insertGhosts(bHadrons, ghostRescaling_, true, true, false, false, fjInputs);
  // insert "ghost" c hadrons in the vector of constituents
  insertGhosts(cHadrons, ghostRescaling_, true, false, false, false, fjInputs);
  // insert "ghost" partons in the vector of constituents
  insertGhosts(partons, ghostRescaling_, false, false, true, false, fjInputs);
  // if used, insert "ghost" leptons in the vector of constituents
  if (useLeptons_)
    insertGhosts(leptons, ghostRescaling_, false, false, false, true, fjInputs);
  // initialize the indices matching reclustered jets to original jets
  std::vector<int> fjInputsMatchingIndices(fjInputs.size(), 0);
  // index the fastjet inputs to themselves as the order in fjInputs
  for(auto it = fjInputs.begin(); it != fjInputs.end(); ++it) {
    it->set_user_index(std::distance(fjInputs.begin(), it));
  }
  // define jet clustering sequence
  fjClusterSeq_ = std::make_shared<fastjet::ClusterSequence>(fjInputs, *fjJetDefinition_);
  // recluster jet constituents and inserted "ghosts"
  std::vector<fastjet::PseudoJet> inclusiveJets = fastjet::sorted_by_pt(fjClusterSeq_->inclusive_jets(jetPtMin_));

  if (inclusiveJets.size() < jets->size())
    edm::LogError("TooFewReclusteredJets")
        << "There are fewer reclustered (" << inclusiveJets.size() << ") than original jets (" << jets->size()
        << "). Please check that the jet algorithm and jet size match those used for the original jet collection.";

  // match reclustered and original jets
  std::vector<int> reclusteredIndices;
  matchReclusteredJets(jets, inclusiveJets, reclusteredIndices);

  // match groomed and original jets
  std::vector<int> groomedIndices;
  if (useSubjets_) {
    if (groomedJets->size() > jets->size())
      edm::LogError("TooManyGroomedJets")
          << "There are more groomed (" << groomedJets->size() << ") than original jets (" << jets->size()
          << "). Please check that the two jet collections belong to each other.";

    matchGroomedJets(jets, groomedJets, groomedIndices);
  }

  // match subjets and original jets
  std::vector<std::vector<int>> subjetIndices;
  if (useSubjets_) {
    matchSubjets(groomedIndices, groomedJets, subjets, subjetIndices);
  }
  std::cout << "[DEBUG] PAT Jets"  << std::endl;
  // determine jet flavour
  for (size_t i = 0; i < jets->size(); ++i) {
    reco::GenParticleRefVector clusteredbHadrons;
    reco::GenParticleRefVector clusteredcHadrons;
    reco::GenParticleRefVector clusteredPartons;
    reco::GenParticleRefVector clusteredLeptons;
    // std::cout << "[DEBUG] Processing original jet " << i << " with Pt = " << jets->at(i).pt() << " and eta = "
    //           << jets->at(i).eta() << std::endl;
    // if matching reclustered to original jets failed
    if (reclusteredIndices.at(i) < 0) {
      // set an empty JetFlavourInfo for this jet
      (*jetFlavourInfos)[jets->refAt(i)] =
          reco::JetFlavourInfo(clusteredbHadrons, clusteredcHadrons, clusteredPartons, clusteredLeptons, 0, 0);
    } else if (jets->at(i).pt() == 0) {
      edm::LogWarning("NullTransverseMomentum")
          << "The original jet " << i << " has Pt=0. This is not expected so the jet will be skipped.";

      // set an empty JetFlavourInfo for this jet
      (*jetFlavourInfos)[jets->refAt(i)] =
          reco::JetFlavourInfo(clusteredbHadrons, clusteredcHadrons, clusteredPartons, clusteredLeptons, 0, 0);

      // if subjets are used
      if (useSubjets_ && !subjetIndices.at(i).empty()) {
        // loop over subjets
        for (size_t sj = 0; sj < subjetIndices.at(i).size(); ++sj) {
          // set an empty JetFlavourInfo for this subjet
          (*subjetFlavourInfos)[subjets->refAt(subjetIndices.at(i).at(sj))] =
              reco::JetFlavourInfo(reco::GenParticleRefVector(),
                                   reco::GenParticleRefVector(),
                                   reco::GenParticleRefVector(),
                                   reco::GenParticleRefVector(),
                                   0,
                                   0);
        }
      }
    } else {
      // since the "ghosts" are extremely soft, the configuration and ordering of the reclustered and original jets should in principle stay the same
      if ((std::abs(inclusiveJets.at(reclusteredIndices.at(i)).pt() - jets->at(i).pt()) / jets->at(i).pt()) >
          relPtTolerance_) {
        if (jets->at(i).pt() < 10.)  // special handling for low-Pt jets (Pt<10 GeV)
          edm::LogWarning("JetPtMismatchAtLowPt")
              << "The reclustered and original jet " << i << " have different Pt's ("
              << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << jets->at(i).pt()
              << " GeV, respectively).\n"
              << "Please check that the jet algorithm and jet size match those used for the original jet collection "
                 "and also make sure the original jets are uncorrected. In addition, make sure you are not using "
                 "CaloJets which are presently not supported.\n"
              << "Since the mismatch is at low Pt (Pt<10 GeV), it is ignored and only a warning is issued.\n"
              << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision "
                 "in which case make sure the original jet collection is produced and reclustering is performed in the "
                 "same job.";
        else
          edm::LogError("JetPtMismatch")
              << "The reclustered and original jet " << i << " have different Pt's ("
              << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << jets->at(i).pt()
              << " GeV, respectively).\n"
              << "Please check that the jet algorithm and jet size match those used for the original jet collection "
                 "and also make sure the original jets are uncorrected. In addition, make sure you are not using "
                 "CaloJets which are presently not supported.\n"
              << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision "
                 "in which case make sure the original jet collection is produced and reclustering is performed in the "
                 "same job.";
      }

      // get jet constituents (sorted by Pt)
      std::vector<fastjet::PseudoJet> constituents =
          fastjet::sorted_by_pt(inclusiveJets.at(reclusteredIndices.at(i)).constituents());

      // loop over jet constituents and try to find "ghosts"
      for (std::vector<fastjet::PseudoJet>::const_iterator it = constituents.begin(); it != constituents.end(); ++it) {
        if (!it->has_user_info())
          continue;  // skip if not a "ghost"
        // std::cout << "[DEBUG] Identified a ghost with at index " << std::distance(static_cast<std::vector<fastjet::PseudoJet>::const_iterator>(constituents.begin()), it) << " and pt " << it->pt() << std::endl;

        // ghost clustering association for GHS
        fjInputsMatchingIndices[it->user_index()] = -(i + 1);
        // "ghost" hadron
        if (it->user_info<GhostInfo>().isHadron()) {
          // "ghost" b hadron
          if (it->user_info<GhostInfo>().isbHadron())
            clusteredbHadrons.push_back(it->user_info<GhostInfo>().particleRef());
          // "ghost" c hadron
          else
            clusteredcHadrons.push_back(it->user_info<GhostInfo>().particleRef());
        }
        // "ghost" parton
        else if (it->user_info<GhostInfo>().isParton())
          clusteredPartons.push_back(it->user_info<GhostInfo>().particleRef());
        // "ghost" lepton
        else if (it->user_info<GhostInfo>().isLepton())
          clusteredLeptons.push_back(it->user_info<GhostInfo>().particleRef());
      }

      int hadronFlavour = 0;  // default hadron flavour set to 0 (= undefined)
      int partonFlavour = 0;  // default parton flavour set to 0 (= undefined)

      // set hadron- and parton-based flavours
      setFlavours(clusteredbHadrons, clusteredcHadrons, clusteredPartons, hadronFlavour, partonFlavour);

      // set the JetFlavourInfo for this jet
      (*jetFlavourInfos)[jets->refAt(i)] = reco::JetFlavourInfo(
          clusteredbHadrons, clusteredcHadrons, clusteredPartons, clusteredLeptons, hadronFlavour, partonFlavour);
      // [DEBUG] Display associated partons
      std::cout << "[DEBUG] Jet " << i  << " : pt = "   << jets->at(i).pt()
                                        << " , eta = "  << jets->at(i).eta()
                                        << " , phi = "  << jets->at(i).phi() << std::endl;
      for (size_t p = 0; p < clusteredPartons.size(); ++p) {
        if (!isFinalParton(clusteredPartons.at(p)))
          continue;
        std::cout << "        PDG ID = "    << clusteredPartons.at(p)->pdgId()
                            << " , pt = "   << clusteredPartons.at(p)->pt() 
                            << " , eta = "  << clusteredPartons.at(p)->eta()
                            << " , phi = "  << clusteredPartons.at(p)->phi() << std::endl;
      }
      // begin setting the GHS algorithm flavour information
      if (enableGHSAlgoFlavour_) {
        // check if the GHS algorithm flavour information is available
        if (fjGHSAlgoJetMatchingIndices.at(i) >= 0) {
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(
              reco::FlavAlgo::kGHS,
              fastjet::contrib::FlavHistory::current_flavour_of(
                  fjGHSAlgoJetResults.at(fjGHSAlgoJetMatchingIndices.at(i))));
        }
        if (fjGHSFullAlgoJetMatchingIndices.at(i) >= 0) {
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(
              reco::FlavAlgo::kGHSFull,
              fastjet::contrib::FlavHistory::current_flavour_of(
                  fjGHSFullAlgoJetResults.at(fjGHSFullAlgoJetMatchingIndices.at(i))));
        }
      }
    }
    // if subjets are used, determine their flavour
    if (useSubjets_) {
      if (subjetIndices.at(i).empty())
        continue;  // continue if the original jet does not have subjets assigned

      // define vectors of GenParticleRefVectors for hadrons and partons assigned to different subjets
      std::vector<reco::GenParticleRefVector> assignedbHadrons(subjetIndices.at(i).size(),
                                                               reco::GenParticleRefVector());
      std::vector<reco::GenParticleRefVector> assignedcHadrons(subjetIndices.at(i).size(),
                                                               reco::GenParticleRefVector());
      std::vector<reco::GenParticleRefVector> assignedPartons(subjetIndices.at(i).size(), reco::GenParticleRefVector());
      std::vector<reco::GenParticleRefVector> assignedLeptons(subjetIndices.at(i).size(), reco::GenParticleRefVector());

      // loop over clustered b hadrons and assign them to different subjets based on smallest dR
      assignToSubjets(clusteredbHadrons, subjets, subjetIndices.at(i), assignedbHadrons);
      // loop over clustered c hadrons and assign them to different subjets based on smallest dR
      assignToSubjets(clusteredcHadrons, subjets, subjetIndices.at(i), assignedcHadrons);
      // loop over clustered partons and assign them to different subjets based on smallest dR
      assignToSubjets(clusteredPartons, subjets, subjetIndices.at(i), assignedPartons);
      // if used, loop over clustered leptons and assign them to different subjets based on smallest dR
      if (useLeptons_)
        assignToSubjets(clusteredLeptons, subjets, subjetIndices.at(i), assignedLeptons);

      // loop over subjets and determine their flavour
      for (size_t sj = 0; sj < subjetIndices.at(i).size(); ++sj) {
        int subjetHadronFlavour = 0;  // default hadron flavour set to 0 (= undefined)
        int subjetPartonFlavour = 0;  // default parton flavour set to 0 (= undefined)

        // set hadron- and parton-based flavours
        setFlavours(assignedbHadrons.at(sj),
                    assignedcHadrons.at(sj),
                    assignedPartons.at(sj),
                    subjetHadronFlavour,
                    subjetPartonFlavour);

        // set the JetFlavourInfo for this subjet
        (*subjetFlavourInfos)[subjets->refAt(subjetIndices.at(i).at(sj))] =
            reco::JetFlavourInfo(assignedbHadrons.at(sj),
                                 assignedcHadrons.at(sj),
                                 assignedPartons.at(sj),
                                 assignedLeptons.at(sj),
                                 subjetHadronFlavour,
                                 subjetPartonFlavour);
      }
    }
  }
  /**************************************************************************
   * [Section]
   *    Produce GHS-algorithm-based flavour information for jets.
   * [Implementation]
   *    1.  Prepare the inputs for GHS directly from the association result.
   *    2.  Feed to GHSCore.
   *    3.  Match to original jets directly using the reclusteredIndices
   * [Note]
   *    1.  We are effectively creating a copy of the original flavour inputs,
   *        and therefore we are not altering the order of the original input.
   **************************************************************************/
  std::vector<fastjet::PseudoJet> fjGHSAlgoInlineJets;
  std::vector<fastjet::PseudoJet> fjGHSAlgoInlineFlavPartons;
  std::vector<fastjet::PseudoJet> fjGHSAlgoInlineJetResults;
  for(auto it = jets->begin(); it != jets->end(); ++it) {
    fjGHSAlgoInlineJets.push_back(fastjet::PseudoJet(it->px(), it->py(), it->pz(), it->energy()));
    // std::cout << "[DEBUG] GHS inline jet: Pt = " << it->pt() << ", Eta = " << it->eta() << ", Phi = " << it->phi() << std::endl;
    fjGHSAlgoInlineJets.back().set_user_info(new fastjet::contrib::FlavHistory(0));
    // std::cout << "[DEBUG] GHS inline jet " << std::distance(jets->begin(), it) << " assigned flavour = 0." << std::endl;
    fjGHSAlgoInlineJets.back().set_user_index(std::distance(jets->begin(), it));
    // std::cout << "[DEBUG] GHS inline jet " << std::distance(jets->begin(), it) << " assigned user index = " << fjGHSAlgoInlineJets.back().user_index() << std::endl;
  }
  for(size_t idx = 0; idx < fjInputs.size(); idx++){
    if(!fjInputs[idx].has_user_info()){
      continue; // not ghost
    }
    if(fjInputs[idx].user_info<GhostInfo>().isParton() && isFinalParton(fjInputs[idx].user_info<GhostInfo>().particleRef())){
      fastjet::PseudoJet partonPseudoJet = fjInputs[idx];
      partonPseudoJet /= ghostRescaling_; // restore the original momentum
      partonPseudoJet.set_user_info(new fastjet::contrib::FlavHistory(fjInputs[idx].user_info<GhostInfo>().particleRef()->pdgId()));
      // set user index with the jet association index already found
      partonPseudoJet.set_user_index(fjInputsMatchingIndices[idx]);
      fjGHSAlgoInlineFlavPartons.push_back(partonPseudoJet);
    }
    else if(fjInputs[idx].user_info<GhostInfo>().isParton()){
      // std::cout << "[DEBUG] Identified non-final parton with pdgId = " << fjInputs[idx].user_info<GhostInfo>().particleRef()->pdgId() << " and Pt = " << fjInputs[idx].pt()/ghostRescaling_ << std::endl;
    }
  }
  // [DEBUG] Display all jets and associated partons
  std::cout << "[DEBUG] GHS inline jets" << std::endl;
  for(size_t jdx = 0; jdx < fjGHSAlgoInlineJets.size(); jdx++){
    std::cout << "[DEBUG] Jet " << jdx  << " : pt = "   << fjGHSAlgoInlineJets[jdx].pt()
                                        << " , eta = "  << fjGHSAlgoInlineJets[jdx].eta()
                                        << " , phi = "  << fjGHSAlgoInlineJets[jdx].phi_std() << std::endl;
    for(size_t pdx = 0; pdx < fjGHSAlgoInlineFlavPartons.size(); pdx++){
      if(fjGHSAlgoInlineFlavPartons[pdx].user_index() + 1 + static_cast<int>(jdx) == 0){
        std::cout << "        PDG ID = "  << fjGHSAlgoInlineFlavPartons[pdx].user_info<fastjet::contrib::FlavHistory>().current_flavour().pdg_code()
                            << " , pt = " << fjGHSAlgoInlineFlavPartons[pdx].pt()
                            << " , eta = " << fjGHSAlgoInlineFlavPartons[pdx].eta()
                            << " , phi = " << fjGHSAlgoInlineFlavPartons[pdx].phi_std() << std::endl;
      }
    }
  }
  // GHS Core is ready to use
  // [DEBUG] Step-by-step tracing of GHSCore
  std::cout << ">>>>>> [VERBOSE] Inline GHS initiated <<<<<" << std::endl;
  GHSCore(fjGHSAlgoInlineJets, fjGHSAlgoInlineFlavPartons, fjGHSAlgoInlineJetResults);
  // [DEBUG] Ending step-by-step tracing of GHSCore
  std::cout << ">>>>>> [VERBOSE] Inline GHS finished <<<<<<" << std::endl;
  // Adding the flavour assignment results.
  for(size_t idx = 0; idx < fjGHSAlgoInlineJetResults.size(); idx++){
    int origJetIdx = fjGHSAlgoInlineJetResults[idx].user_index();
    if(origJetIdx < 0 || origJetIdx >= static_cast<int>(jets->size())){
      continue; // not matched to any original jets
    }
    (*jetFlavourInfos)[jets->refAt(origJetIdx)].setAlgoFlav(
        reco::FlavAlgo::kGHSInlineParton,
        fastjet::contrib::FlavHistory::current_flavour_of(fjGHSAlgoInlineJetResults[idx]));
  }


  //deallocate only at the end of the event processing
  fjClusterSeq_.reset();

  // put jet flavour infos in the event
  iEvent.put(std::move(jetFlavourInfos));
  // put subjet flavour infos in the event
  if (useSubjets_)
    iEvent.put(std::move(subjetFlavourInfos), "SubJets");
}

// ------------ method that inserts "ghost" particles in the vector of jet constituents ------------
void JetFlavourClustering::insertGhosts(const edm::Handle<reco::GenParticleRefVector>& particles,
                                        const double ghostRescaling,
                                        const bool isHadron,
                                        const bool isbHadron,
                                        const bool isParton,
                                        const bool isLepton,
                                        std::vector<fastjet::PseudoJet>& constituents) {
  // insert "ghost" particles in the vector of jet constituents
  for (reco::GenParticleRefVector::const_iterator it = particles->begin(); it != particles->end(); ++it) {
    if ((*it)->pt() == 0) {
      edm::LogInfo("NullTransverseMomentum") << "dropping input ghost candidate with pt=0";
      continue;
    }
    fastjet::PseudoJet p((*it)->px(), (*it)->py(), (*it)->pz(), (*it)->energy());
    p *= ghostRescaling;  // rescale particle momentum
    p.set_user_info(new GhostInfo(isHadron, isbHadron, isParton, isLepton, *it));
    constituents.push_back(p);
  }
}

// ------------ method that inserts "ghost" partons in the vector of jet constituents for GHS algorithm ------------
void JetFlavourClustering::insertGhostFinalPartons(const edm::Handle<reco::GenParticleRefVector>& particles,
                                                   const double ghostRescaling,
                                                   std::vector<fastjet::PseudoJet>& constituents,
                                                   const bool& ghostInfoHasPriority) {
  // insert "ghost" partons in the vector of jet constituents
  for (reco::GenParticleRefVector::const_iterator it = particles->begin(); it != particles->end(); ++it) {
    if ((*it)->pt() == 0) {
      edm::LogInfo("NullTransverseMomentum") << "dropping input ghost candidate with pt=0";
      continue;
    }
    if (!isFinalParton(*it)) {
      edm::LogInfo("NonFinalParton") << "dropping non-final parton candidate with pt=" << (*it)->pt()
                                     << " and pdgId=" << (*it)->pdgId();
      continue;  // skip non-final partons
    }
    fastjet::PseudoJet p((*it)->px(), (*it)->py(), (*it)->pz(), (*it)->energy());
    fastjet::contrib::FlavInfo ghostFlavInfo((*it)->pdgId());
    p *= ghostRescaling;  // rescale particle momentum
    // Crucial: assign flavour info to the flavoured ghost partons...
    p.set_user_info(new fastjet::contrib::FlavHistory(static_cast<const fastjet::contrib::FlavInfo&>(ghostFlavInfo)));
    if (ghostInfoHasPriority) {
      p.set_user_info(new GhostFinalPartonInfo(*it, true));  // set user info for final parton
    }
    constituents.push_back(p);
  }
}

// ------------ method that matches reclustered and original jets based on minimum dR ------------
void JetFlavourClustering::matchReclusteredJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                                                const std::vector<fastjet::PseudoJet>& reclusteredJets,
                                                std::vector<int>& matchedIndices) {
  std::vector<bool> matchedLocks(reclusteredJets.size(), false);

  for (size_t j = 0; j < jets->size(); ++j) {
    double matchedDR2 = 1e9;
    int matchedIdx = -1;

    for (size_t rj = 0; rj < reclusteredJets.size(); ++rj) {
      if (matchedLocks.at(rj))
        continue;  // skip jets that have already been matched

      double tempDR2 = reco::deltaR2(jets->at(j).rapidity(),
                                     jets->at(j).phi(),
                                     reclusteredJets.at(rj).rapidity(),
                                     reclusteredJets.at(rj).phi_std());
      if (tempDR2 < matchedDR2) {
        matchedDR2 = tempDR2;
        matchedIdx = rj;
      }
    }

    if (matchedIdx >= 0) {
      if (matchedDR2 > rParam_ * rParam_) {
        edm::LogError("JetMatchingFailed") << "Matched reclustered jet " << matchedIdx << " and original jet " << j
                                           << " are separated by dR=" << sqrt(matchedDR2)
                                           << " which is greater than the jet size R=" << rParam_ << ".\n"
                                           << "This is not expected so please check that the jet algorithm and jet "
                                              "size match those used for the original jet collection.";
      } else
        matchedLocks.at(matchedIdx) = true;
    } else
      edm::LogError("JetMatchingFailed") << "Matching reclustered to original jets failed. Please check that the jet "
                                            "algorithm and jet size match those used for the original jet collection.";

    matchedIndices.push_back(matchedIdx);
  }
}

// ------------ method that matches groomed and original jets based on minimum dR ------------
void JetFlavourClustering::matchGroomedJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                                            const edm::Handle<edm::View<reco::Jet>>& groomedJets,
                                            std::vector<int>& matchedIndices) {
  std::vector<bool> jetLocks(jets->size(), false);
  std::vector<int> jetIndices;

  for (size_t gj = 0; gj < groomedJets->size(); ++gj) {
    double matchedDR2 = 1e9;
    int matchedIdx = -1;

    if (groomedJets->at(gj).pt() > 0.)  // skip pathological cases of groomed jets with Pt=0
    {
      for (size_t j = 0; j < jets->size(); ++j) {
        if (jetLocks.at(j))
          continue;  // skip jets that have already been matched

        double tempDR2 = reco::deltaR2(
            jets->at(j).rapidity(), jets->at(j).phi(), groomedJets->at(gj).rapidity(), groomedJets->at(gj).phi());
        if (tempDR2 < matchedDR2) {
          matchedDR2 = tempDR2;
          matchedIdx = j;
        }
      }
    }

    if (matchedIdx >= 0) {
      if (matchedDR2 > rParam_ * rParam_) {
        edm::LogWarning("MatchedJetsFarApart")
            << "Matched groomed jet " << gj << " and original jet " << matchedIdx
            << " are separated by dR=" << sqrt(matchedDR2) << " which is greater than the jet size R=" << rParam_
            << ".\n"
            << "This is not expected so the matching of these two jets has been discarded. Please check that the two "
               "jet collections belong to each other.";
        matchedIdx = -1;
      } else
        jetLocks.at(matchedIdx) = true;
    }
    jetIndices.push_back(matchedIdx);
  }

  for (size_t j = 0; j < jets->size(); ++j) {
    std::vector<int>::iterator matchedIndex = std::find(jetIndices.begin(), jetIndices.end(), j);

    matchedIndices.push_back(matchedIndex != jetIndices.end() ? std::distance(jetIndices.begin(), matchedIndex) : -1);
  }
}

// ------------ method that matches subjets and original jets ------------
void JetFlavourClustering::matchSubjets(const std::vector<int>& groomedIndices,
                                        const edm::Handle<edm::View<reco::Jet>>& groomedJets,
                                        const edm::Handle<edm::View<reco::Jet>>& subjets,
                                        std::vector<std::vector<int>>& matchedIndices) {
  for (size_t g = 0; g < groomedIndices.size(); ++g) {
    std::vector<int> subjetIndices;

    if (groomedIndices.at(g) >= 0) {
      for (size_t s = 0; s < groomedJets->at(groomedIndices.at(g)).numberOfDaughters(); ++s) {
        const edm::Ptr<reco::Candidate>& subjet = groomedJets->at(groomedIndices.at(g)).daughterPtr(s);

        for (size_t sj = 0; sj < subjets->size(); ++sj) {
          if (subjet == edm::Ptr<reco::Candidate>(subjets->ptrAt(sj))) {
            subjetIndices.push_back(sj);
            break;
          }
        }
      }

      if (subjetIndices.empty())
        edm::LogError("SubjetMatchingFailed") << "Matching subjets to original jets failed. Please check that the "
                                                 "groomed jet and subjet collections belong to each other.";

      matchedIndices.push_back(subjetIndices);
    } else
      matchedIndices.push_back(subjetIndices);
  }
}

// ------------ method that sets hadron- and parton-based flavours ------------
void JetFlavourClustering::setFlavours(const reco::GenParticleRefVector& clusteredbHadrons,
                                       const reco::GenParticleRefVector& clusteredcHadrons,
                                       const reco::GenParticleRefVector& clusteredPartons,
                                       int& hadronFlavour,
                                       int& partonFlavour) {
  reco::GenParticleRef hardestParton;
  reco::GenParticleRef hardestLightParton;
  reco::GenParticleRef flavourParton;

  // loop over clustered partons (already sorted by Pt)
  for (reco::GenParticleRefVector::const_iterator it = clusteredPartons.begin(); it != clusteredPartons.end(); ++it) {
    // hardest parton
    if (hardestParton.isNull())
      hardestParton = (*it);
    // hardest light-flavour parton
    if (hardestLightParton.isNull()) {
      if (CandMCTagUtils::isLightParton(*(*it)))
        hardestLightParton = (*it);
    }
    // c flavour
    if (flavourParton.isNull() && (std::abs((*it)->pdgId()) == 4))
      flavourParton = (*it);
    // b flavour gets priority
    if (std::abs((*it)->pdgId()) == 5) {
      if (flavourParton.isNull())
        flavourParton = (*it);
      else if (std::abs(flavourParton->pdgId()) != 5)
        flavourParton = (*it);
    }
  }

  // set hadron-based flavour
  if (!clusteredbHadrons.empty())
    hadronFlavour = 5;
  else if (!clusteredcHadrons.empty() && clusteredbHadrons.empty())
    hadronFlavour = 4;
  // set parton-based flavour
  if (flavourParton.isNull()) {
    if (hardestParton.isNonnull())
      partonFlavour = hardestParton->pdgId();
  } else
    partonFlavour = flavourParton->pdgId();

  // if enabled, check for conflicts between hadron- and parton-based flavours and give priority to the hadron-based flavour
  if (hadronFlavourHasPriority_) {
    if (hadronFlavour == 0 && (std::abs(partonFlavour) == 4 || std::abs(partonFlavour) == 5))
      partonFlavour = (hardestLightParton.isNonnull() ? hardestLightParton->pdgId() : 0);
    else if (hadronFlavour != 0 && std::abs(partonFlavour) != hadronFlavour)
      partonFlavour = hadronFlavour;
  }
}

// ------------ method that assigns clustered particles to subjets ------------
void JetFlavourClustering::assignToSubjets(const reco::GenParticleRefVector& clusteredParticles,
                                           const edm::Handle<edm::View<reco::Jet>>& subjets,
                                           const std::vector<int>& subjetIndices,
                                           std::vector<reco::GenParticleRefVector>& assignedParticles) {
  // loop over clustered particles and assign them to different subjets based on smallest dR
  for (reco::GenParticleRefVector::const_iterator it = clusteredParticles.begin(); it != clusteredParticles.end();
       ++it) {
    std::vector<double> dR2toSubjets;

    for (size_t sj = 0; sj < subjetIndices.size(); ++sj)
      dR2toSubjets.push_back(reco::deltaR2((*it)->rapidity(),
                                           (*it)->phi(),
                                           subjets->at(subjetIndices.at(sj)).rapidity(),
                                           subjets->at(subjetIndices.at(sj)).phi()));

    // find the closest subjet
    int closestSubjetIdx =
        std::distance(dR2toSubjets.begin(), std::min_element(dR2toSubjets.begin(), dR2toSubjets.end()));

    assignedParticles.at(closestSubjetIdx).push_back(*it);
  }
}

// ------------ method check if the clustered parton is immediately before hadronization ------------
bool JetFlavourClustering::isFinalParton(const reco::GenParticleRef& particle) const {
  // Require parton in the first place.
  if (!particle.isNonnull() || !particle.isAvailable() || !CandMCTagUtils::isParton(*particle)) {
    return false;
  }
  // Loop over daughters. Exclude particles that have parton daughters.
  bool hasPartonDaughter = false;
  for (size_t i = 0; i < particle->numberOfDaughters(); ++i) {
    const reco::Candidate* daughter = particle->daughter(i);
    if (daughter != nullptr && CandMCTagUtils::isParton(*daughter)) {
      hasPartonDaughter = true;
      break;
    }
  }
  return !hasPartonDaughter;  // Return true if no parton daughters are found.
}

// ------------ method produce a list of partons without parton daughters ------------
void JetFlavourClustering::makeFinalPartonSet(const reco::GenParticleRefVector& particles,
                                              reco::GenParticleRefVector& finalPartons) {
  // Verify if finalPartons is empty.
  if (!finalPartons.empty()) {
    throw cms::Exception("FinalPartonsNotEmpty")
        << "The vector of final partons is not empty. Please check the configuration.";
  }
  // Loop over particles and check if they are final partons.
  for (const auto& particle : particles) {
    if (isFinalParton(particle)) {
      finalPartons.push_back(particle);  // Add final parton to the vector.
    }
  }
}

// ------------ method that produces GHS algorithm flavoured jets as a full-chain ------------
void JetFlavourClustering::makeGHSAlgoJets(const edm::Handle<edm::View<reco::Jet>>& inputGenJets,
                                           const edm::Handle<reco::GenParticleRefVector>& inputGenPartons,
                                           std::vector<int>& matchedIndices,
                                           std::vector<fastjet::PseudoJet>& outputGHSAlgoJets) {
  // verify if output vectors are empty
  if (!matchedIndices.empty() || !outputGHSAlgoJets.empty()) {
    edm::LogError("GHSAlgoOutputVectorsNotEmpty")
        << "The output vectors for GHS algorithm are not empty. Please check the configuration.";
  }
  // insert "ghost" final partons in the vector of constituents
  std::vector<fastjet::PseudoJet> jetAndGhostPartons;
  for (auto& genJet : *inputGenJets) {
    fastjet::PseudoJet fjJet(genJet.px(), genJet.py(), genJet.pz(), genJet.energy());
    jetAndGhostPartons.push_back(fjJet);
    // Not inserting flavour info. Prioritize ghost info.
  }
  insertGhostFinalPartons(inputGenPartons, ghostRescaling_, jetAndGhostPartons, true);
  // Clustering with the algorithm specified by the user.
  ClusterSequencePtr baseClusterSeq = std::make_shared<fastjet::ClusterSequence>(jetAndGhostPartons, *fjJetDefinition_);
  fastjet::Selector jetPtSelector = fastjet::SelectorPtMin(ghsAlgoPtMin_);
  std::vector<fastjet::PseudoJet> baseJets = fastjet::sorted_by_pt(baseClusterSeq->inclusive_jets(jetPtMin_));
  std::vector<fastjet::PseudoJet> hardJets = jetPtSelector(baseJets);
  if (hardJets.size() == 0) {
    return;
  }
  std::vector<fastjet::PseudoJet> jetAndPartons;
  std::vector<fastjet::PseudoJet> finalJets = hardJets;
  std::vector<fastjet::PseudoJet> inputsFromCS(baseClusterSeq->jets().begin(),
                                               baseClusterSeq->jets().begin() + baseClusterSeq->n_particles());
  std::vector<fastjet::contrib::FlavInfo> finalJetsFlavInfo(finalJets.size());
  int njets = finalJets.size();

  /// Set up GHS Info
  GHSInfo ghsInfo;
  ghsInfo.jets = finalJets;
  ghsInfo.njets = njets;
  ghsInfo.alpha = ghsAlgoAlpha_;
  ghsInfo.omega = ghsAlgoOmega_;
  ghsInfo.flav_recombiner = *ghsAlgoFlavRecombinerPtr_;

  /// Set up elements for GHS Algorithm
  for (auto& jet : finalJets) {
    jet.set_user_info(
        new fastjet::contrib::FlavHistory(fastjet::contrib::FlavInfo(0)));  // Initialize with default flavour info
    jet.set_user_index(1);
    jetAndPartons.push_back(jet);
  }

  /// Adding constituents. Use only ghost partons and do back-scaling.
  for (auto constituent : inputsFromCS) {
    if (constituent.has_user_info<GhostInfo>() || constituent.pt() < ghsAlgoPtMin_) {
      constituent.set_user_index(0);
      for (int i = 0; i < njets; ++i) {
        if (constituent.is_inside(finalJets[i])) {
          constituent.set_user_index(-1 - i);
          break;
        }
      }
      constituent /= ghostRescaling_;  // Rescale the momentum of the ghost parton
      constituent.set_user_info(new fastjet::contrib::FlavHistory(
          fastjet::contrib::FlavInfo(constituent.user_info<GhostFinalPartonInfo>().particleRef()->pdgId())));
      jetAndPartons.push_back(constituent);
    }
  }
  // [DEBUG] Print jets and associated partons
  std::cout << "[DEBUG] GHS Jets"  << std::endl;
  for (size_t i = 0; i < finalJets.size(); ++i) {
    std::cout     << "[DEBUG] Jet " << i  << " : pt = "   << finalJets[i].pt()
                                          << " , eta = "  << finalJets[i].eta()
                                          << " , phi = "  << finalJets[i].phi_std() << std::endl;
    for (size_t j = njets; j < jetAndPartons.size(); ++j) {
      if (jetAndPartons[j].user_index() + 1 + static_cast<int>(i) == 0) {
        std::cout << "        PDG ID = "  << jetAndPartons[j].user_info<fastjet::contrib::FlavHistory>().current_flavour().pdg_code()
                            << " , pt = " << jetAndPartons[j].pt()
                            << " , eta = " << jetAndPartons[j].eta()
                            << " , phi = " << jetAndPartons[j].phi_std() << std::endl;
      }
    }
  }

  if (jetAndPartons.size() == 0) {
    return;
  }
  // Set up NNH for GHS algorithm
  fastjet::NNH<GHSBriefJet, GHSInfo> nnh(jetAndPartons, &ghsInfo);
  int iA, iB;
  while (njets > 0) {  // the loop does not change njets, but njets > 0 is necessary
                       // given that the selector could cut off all jets
    double dij = nnh.dij_min(iA, iB);
    // LS-2023-02-10: not sure this is very safe...
    // if (dij > 0.9*numeric_limits<double>::max()) {
    if (dij == std::numeric_limits<double>::max()) {
      break;
    }
    if (iB >= 0) {
      if (iA > iB)
        std::swap(iA, iB);
      // we must never have two jets
      assert(iB >= njets && "second entry must be a particle");
      if (iA < njets) {
        // if the first is a jet, assign B's flavour to A and then remove B
        // (note that through the shared pointer, this also affects the
        // flavour of the objects in the NNH object -- which is dangerous --
        // one should really remove the jet and add it back in)
        fastjet::contrib::FlavInfo flavB =
            jetAndPartons[iB].user_info<fastjet::contrib::FlavHistory>().current_flavour();
        finalJetsFlavInfo[iA] = finalJetsFlavInfo[iA] + flavB;
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(finalJetsFlavInfo[iA]);
        nnh.remove_jet(iB);
      } else {
        //> iA & iB are both flavour inputs
        // merge
        fastjet::PseudoJet mergedFlavoured = jetAndPartons[iA];
        mergedFlavoured.reset_momentum(jetAndPartons[iA] +
                                       jetAndPartons[iB]);  //<- resetting only the momentum keeps the
        //> determine the jet association for the merged cluster:
        //> can only be associated with a jet if *both* inputs were
        // associated with the *same* jet
        if (jetAndPartons[iA].user_index() == jetAndPartons[iB].user_index()) {
          mergedFlavoured.set_user_index(jetAndPartons[iA].user_index());
        } else {
          mergedFlavoured.set_user_index(0);
        }
        fastjet::contrib::FlavInfo flav = fastjet::contrib::FlavHistory::current_flavour_of(jetAndPartons[iA]) +
                                          fastjet::contrib::FlavHistory::current_flavour_of(jetAndPartons[iB]);
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(flav);
        /// set FlavInfo attribute
        mergedFlavoured.set_user_info(new fastjet::contrib::FlavHistory(flav));
        jetAndPartons.push_back(mergedFlavoured);
        nnh.merge_jets(iA, iB, mergedFlavoured, jetAndPartons.size() - 1);
      }
    } else {
      nnh.remove_jet(iA);
    }
  }
  for (unsigned i = 0; i < finalJets.size(); i++) {
    finalJetsFlavInfo[i].update_flavourless_attribute();
    finalJets[i].set_user_info(new fastjet::contrib::FlavHistory(fastjet::contrib::FlavInfo(finalJetsFlavInfo[i])));
    // restore user index to what it was
    finalJets[i].set_user_index(hardJets[i].user_index());
  }
  outputGHSAlgoJets = finalJets;
  // match reclustered jets to original jets
  matchReclusteredJets(inputGenJets, outputGHSAlgoJets, matchedIndices);
}

void JetFlavourClustering::makeGHSFullAlgoJets(const edm::Handle<edm::View<reco::Jet>>& inputGenJets,
                                               const edm::Handle<reco::GenParticleRefVector>& inputGenPartons,
                                               const edm::Handle<edm::ValueMap<float>>& inputWeights,
                                               std::vector<int>& matchedIndices,
                                               std::vector<fastjet::PseudoJet>& outputGHSAlgoJets) {
  // verify if output vectors are empty
  if (!matchedIndices.empty() || !outputGHSAlgoJets.empty()) {
    edm::LogError("GHSAlgoOutputVectorsNotEmpty")
        << "The output vectors for GHS algorithm are not empty. Please check the configuration.";
  }
  std::cout << ">>>>>>>>>>>>>>>>> [VERBOSE] Making GHS Full Jets <<<<<<<<<<<<<<<" << std::endl;
  // insert "ghost" final partons in the vector of constituents
  std::vector<fastjet::PseudoJet> jetAndGhostPartons;
  for (auto& genJet : *inputGenJets) {
    std::vector<edm::Ptr<reco::Candidate>> constituents = genJet.getJetConstituents();
    std::vector<edm::Ptr<reco::Candidate>>::const_iterator m;
    for (m = constituents.begin(); m != constituents.end(); ++m) {
      const reco::CandidatePtr& constit = *m;
      if (!constit.isNonnull() || !constit.isAvailable()) {
        edm::LogError("MissingJetConstituent") << "Jet constituent required for jet reclustering is missing. "
                                                  "Reclustered jets are not guaranteed to reproduce the original jets!";
        continue;
      }
      if (constit->pt() == 0) {
        edm::LogWarning("NullTransverseMomentum") << "dropping input candidate with pt=0";
        continue;
      }
      if (genJet.isWeighted()) {
        if (weightsToken_.isUninitialized())
          throw cms::Exception("MissingConstituentWeight")
              << "JetFlavourClustering: No weights (e.g. PUPPI) given for weighted jet collection" << std::endl;
        float w = (*inputWeights)[constit];
        jetAndGhostPartons.push_back(
            fastjet::PseudoJet(constit->px() * w, constit->py() * w, constit->pz() * w, constit->energy() * w));
      } else {
        jetAndGhostPartons.push_back(
            fastjet::PseudoJet(constit->px(), constit->py(), constit->pz(), constit->energy()));
      }
    }
    // Not inserting flavour info. Prioritize ghost info.
  }
  insertGhostFinalPartons(inputGenPartons, ghostRescaling_, jetAndGhostPartons, true);
  // Clustering with the algorithm specified by the user.
  ClusterSequencePtr baseClusterSeq = std::make_shared<fastjet::ClusterSequence>(jetAndGhostPartons, *fjJetDefinition_);
  fastjet::Selector jetPtSelector = fastjet::SelectorPtMin(ghsAlgoPtMin_);
  std::vector<fastjet::PseudoJet> baseJets = fastjet::sorted_by_pt(baseClusterSeq->inclusive_jets(jetPtMin_));
  std::vector<fastjet::PseudoJet> hardJets = jetPtSelector(baseJets);
  if (hardJets.size() == 0) {
    return;
  }
  std::vector<fastjet::PseudoJet> jetAndPartons;
  std::vector<fastjet::PseudoJet> finalJets = hardJets;
  std::vector<fastjet::PseudoJet> inputsFromCS(baseClusterSeq->jets().begin(),
                                               baseClusterSeq->jets().begin() + baseClusterSeq->n_particles());
  std::vector<fastjet::contrib::FlavInfo> finalJetsFlavInfo(finalJets.size());
  int njets = finalJets.size();

  /// Set up GHS Info
  GHSInfo ghsInfo;
  ghsInfo.jets = finalJets;
  ghsInfo.njets = njets;
  ghsInfo.alpha = ghsAlgoAlpha_;
  ghsInfo.omega = ghsAlgoOmega_;
  ghsInfo.flav_recombiner = *ghsAlgoFlavRecombinerPtr_;

  /// Set up elements for GHS Algorithm
  for (auto& jet : finalJets) {
    jet.set_user_info(
        new fastjet::contrib::FlavHistory(fastjet::contrib::FlavInfo(0)));  // Initialize with default flavour info
    jet.set_user_index(1);
    jetAndPartons.push_back(jet);
  }

  /// Adding constituents. Use only ghost partons and do back-scaling.
  for (auto constituent : inputsFromCS) {
    if (constituent.has_user_info<GhostInfo>()) {
      constituent.set_user_index(0);
      for (int i = 0; i < njets; ++i) {
        if (constituent.is_inside(finalJets[i])) {
          constituent.set_user_index(-1 - i);
          break;
        }
      }
      constituent /= ghostRescaling_;  // Rescale the momentum of the ghost parton
      constituent.set_user_info(new fastjet::contrib::FlavHistory(
          fastjet::contrib::FlavInfo(constituent.user_info<GhostInfo>().particleRef()->pdgId())));
      jetAndPartons.push_back(constituent);
    }
  }
  // [DEBUG] Display all jets and associated partons.
  std::cout << "[DEBUG] GHS Full Jets" << std::endl;
  for(size_t jdx = 0; jdx < hardJets.size(); jdx++){
    std::cout << "[DEBUG] Jet " << jdx  << " : pt = "    << hardJets[jdx].pt()
                                        << " , eta = "  << hardJets[jdx].eta()
                                        << " , phi = "  << hardJets[jdx].phi_std() << std::endl;
    for(size_t pdx = 0; pdx < jetAndPartons.size(); pdx++){
      if(jetAndPartons[pdx].user_index() + 1 + static_cast<int>(jdx) == 0){
        std::cout << "        PDG ID = " << jetAndPartons[pdx].user_info<fastjet::contrib::FlavHistory>().current_flavour().pdg_code()
                            << " , pt = " << jetAndPartons[pdx].pt()
                            << " , eta = " << jetAndPartons[pdx].eta()
                            << " , phi = " << jetAndPartons[pdx].phi_std() << std::endl;
      }
    }
  }

  if (jetAndPartons.size() == 0) {
    return;
  }
  // Set up NNH for GHS algorithm
  fastjet::NNH<GHSBriefJet, GHSInfo> nnh(jetAndPartons, &ghsInfo);
  int iA, iB;
  while (njets > 0) {  // the loop does not change njets, but njets > 0 is necessary
                       // given that the selector could cut off all jets
    double dij = nnh.dij_min(iA, iB);
    // LS-2023-02-10: not sure this is very safe...
    // if (dij > 0.9*numeric_limits<double>::max()) {
    if (dij == std::numeric_limits<double>::max()) {
      break;
    }
    if (iB >= 0) {
      if (iA > iB)
        std::swap(iA, iB);
      // we must never have two jets
      assert(iB >= njets && "second entry must be a particle");
      /// [VERBOSE Show the ongoing flavour dressing process
      std::cout << ">>>>>> [VERBOSE] GHS Clustering Step <<<<<<" << std::endl;
      std::cout << "       [VERBOSE] dij = " << dij << std::endl;
      std::cout << "       [VERBOSE] iA = "  << iA
                << " , user_index = "     << jetAndPartons[iA].user_index()
                << " , pt = "             << jetAndPartons[iA].pt()
                << " , eta = "            << jetAndPartons[iA].eta()
                << std::endl;
      std::cout << "       [VERBOSE] iB = "  << iB
                << " , user_index = "     << jetAndPartons[iB].user_index()
                << " , pt = "             << jetAndPartons[iB].pt()
                << " , eta = "            << jetAndPartons[iB].eta()
                << std::endl;
      if (iA < njets) {
        // if the first is a jet, assign B's flavour to A and then remove B
        // (note that through the shared pointer, this also affects the
        // flavour of the objects in the NNH object -- which is dangerous --
        // one should really remove the jet and add it back in)
        fastjet::contrib::FlavInfo flavB =
            jetAndPartons[iB].user_info<fastjet::contrib::FlavHistory>().current_flavour();
        finalJetsFlavInfo[iA] = finalJetsFlavInfo[iA] + flavB;
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(finalJetsFlavInfo[iA]);
        nnh.remove_jet(iB);
        std::cout << "       [VERBOSE] Merging flavour from iB = " << iB << " to jet iA = " << iA << std::endl;
      } else {
        //> iA & iB are both flavour inputs
        // merge
        fastjet::PseudoJet mergedFlavoured = jetAndPartons[iA];
        mergedFlavoured.reset_momentum(jetAndPartons[iA] +
                                       jetAndPartons[iB]);  //<- resetting only the momentum keeps the
        //> determine the jet association for the merged cluster:
        //> can only be associated with a jet if *both* inputs were
        // associated with the *same* jet
        if (jetAndPartons[iA].user_index() == jetAndPartons[iB].user_index()) {
          mergedFlavoured.set_user_index(jetAndPartons[iA].user_index());
          std::cout << "       [VERBOSE] Keeping jet-parton association for [ " << iA << " ] and [ " << iB << " ]"<< std::endl;
        } else {
          mergedFlavoured.set_user_index(0);
          std::cout << "       [VERBOSE] Removing jet-parton association for [ " << iA << " ] and [ " << iB << " ]"<< std::endl;
        }
        fastjet::contrib::FlavInfo flav = fastjet::contrib::FlavHistory::current_flavour_of(jetAndPartons[iA]) +
                                          fastjet::contrib::FlavHistory::current_flavour_of(jetAndPartons[iB]);
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(flav);
        /// set FlavInfo attribute
        mergedFlavoured.set_user_info(new fastjet::contrib::FlavHistory(flav));
        jetAndPartons.push_back(mergedFlavoured);
        nnh.merge_jets(iA, iB, mergedFlavoured, jetAndPartons.size() - 1);
      }
    } else {
      nnh.remove_jet(iA);
      std::cout << "       [VERBOSE] Removing jet [ " << iA << " ] which gives dij with beam."<< std::endl;
    }
    std::cout << ">>>>>> [VERBOSE] End of GHS Clustering Step <<<<<<" << std::endl;
  }
  for (unsigned i = 0; i < finalJets.size(); i++) {
    finalJetsFlavInfo[i].update_flavourless_attribute();
    finalJets[i].set_user_info(new fastjet::contrib::FlavHistory(fastjet::contrib::FlavInfo(finalJetsFlavInfo[i])));
    // restore user index to what it was
    finalJets[i].set_user_index(hardJets[i].user_index());
  }
  outputGHSAlgoJets = finalJets;
  // match reclustered jets to original jets
  matchReclusteredJets(inputGenJets, outputGHSAlgoJets, matchedIndices);
  std::cout << ">>>>>>>>>>>>>>> [VERBOSE] End Making GHS Full Jets <<<<<<<<<<<<<" << std::endl;
}
/******************************************************************************
 *  [Name of Method]
 *    GHSCore
 *  [Description]
 *    Produce GHS algorithm flavour for a given set of jets and flavour inputs.
 *  [Arguments]
 *    const std::vector<fastjet::PseudoJet>& inputJets
 *    : Vector of jets to be flavoured. Require user_index pointing to matched
 *      jet in the original collection.
 *    const std::vector<fastjet::PseudoJet>& inputFlavParticles
 *    : Vector of flavour particles to be used for flavouring the jets. Can be
 *      either b/c hadrons or partons. Require user_index -(i + 1) for
 *      association with jet number i and 0 if not associated. 
 *    std::vector<fastjet::PseudoJet>& outputGHSAlgoJets
 *    : Vector of GHS algorithm flavoured jets to be produced.
 *  [Return]
 *    (void)
 *  [Note]
 *    1.  The index pointed by user_index of inputJets is temporarily kept by
 *        inputJets during the algorithm and restored at the end.
******************************************************************************/
void JetFlavourClustering::GHSCore(const std::vector<fastjet::PseudoJet>& inputJets,
                                   const std::vector<fastjet::PseudoJet>& inputFlavParticles,
                                         std::vector<fastjet::PseudoJet>& outputGHSAlgoJets) {
  // verify if output vectors are empty
  if (!outputGHSAlgoJets.empty()) {
    edm::LogError("GHSAlgoOutputVectorsNotEmpty")
        << "The output vectors for GHS algorithm are not empty. Please check the configuration.";
  }
  if (inputJets.size() == 0) {
    // No jets to be assigned flavour
    return;
  }
  std::vector<fastjet::PseudoJet> inputJetAndFlavParticles = inputJets;
  inputJetAndFlavParticles.insert(inputJetAndFlavParticles.end(),
                                  inputFlavParticles.begin(),
                                  inputFlavParticles.end());
  outputGHSAlgoJets = inputJets;
  std::vector<fastjet::contrib::FlavInfo> finalJetsFlavInfo(outputGHSAlgoJets.size());
  int njets = outputGHSAlgoJets.size();

  /// Prepare the indices of the input jets in the inputJetAndFlavParticles
  for (size_t idx = 0; idx < inputJets.size(); ++idx) {
    inputJetAndFlavParticles[idx].set_user_index(1);  // unified index "1"
  }

  /// Set up GHS Info
  GHSInfo ghsInfo;
  ghsInfo.jets = outputGHSAlgoJets;
  ghsInfo.njets = njets;
  ghsInfo.alpha = ghsAlgoAlpha_;
  ghsInfo.omega = ghsAlgoOmega_;
  ghsInfo.flav_recombiner = *ghsAlgoFlavRecombinerPtr_;
  /// Final check of input particles
  if (inputJetAndFlavParticles.size() == 0) {
    return;
  }
  // Set up NNH for GHS algorithm
  fastjet::NNH<GHSBriefJet, GHSInfo> nnh(inputJetAndFlavParticles, &ghsInfo);
  int iA, iB;
  while (njets > 0) {  // the loop does not change njets, but njets > 0 is necessary
                       // given that the selector could cut off all jets
    double dij = nnh.dij_min(iA, iB);
    // LS-2023-02-10: not sure this is very safe...
    // if (dij > 0.9*numeric_limits<double>::max()) {
    if (dij == std::numeric_limits<double>::max()) {
      break;
    }
    if (iB >= 0) {
      if (iA > iB)
        std::swap(iA, iB);
      // we must never have two jets
      assert(iB >= njets && "second entry must be a particle");
      /// [VERBOSE Show the ongoing flavour dressing process
      std::cout << ">>>>>> [VERBOSE] GHS Clustering Step <<<<<<" << std::endl;
      std::cout << "       [VERBOSE] dij = " << dij << std::endl;
      std::cout << "       [VERBOSE] iA = "  << iA
                << " , user_index = "     << inputJetAndFlavParticles[iA].user_index()
                << " , pt = "             << inputJetAndFlavParticles[iA].pt()
                << " , eta = "            << inputJetAndFlavParticles[iA].eta()
                << std::endl;
      std::cout << "       [VERBOSE] iB = "  << iB
                << " , user_index = "     << inputJetAndFlavParticles[iB].user_index()
                << " , pt = "             << inputJetAndFlavParticles[iB].pt()
                << " , eta = "            << inputJetAndFlavParticles[iB].eta()
                << std::endl;
      if (iA < njets) {
        // if the first is a jet, assign B's flavour to A and then remove B
        // (note that through the shared pointer, this also affects the
        // flavour of the objects in the NNH object -- which is dangerous --
        // one should really remove the jet and add it back in)
        fastjet::contrib::FlavInfo flavB =
            inputJetAndFlavParticles[iB].user_info<fastjet::contrib::FlavHistory>().current_flavour();
        finalJetsFlavInfo[iA] = finalJetsFlavInfo[iA] + flavB;
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(finalJetsFlavInfo[iA]);
        nnh.remove_jet(iB);
        std::cout << "       [VERBOSE] Merging flavour from iB = " << iB << " to jet iA = " << iA << std::endl;
      } else {
        //> iA & iB are both flavour inputs
        // merge
        fastjet::PseudoJet mergedFlavoured = inputJetAndFlavParticles[iA];
        mergedFlavoured.reset_momentum(inputJetAndFlavParticles[iA] +
                                       inputJetAndFlavParticles[iB]);  //<- resetting only the momentum keeps the
        //> determine the jet association for the merged cluster:
        //> can only be associated with a jet if *both* inputs were
        // associated with the *same* jet
        if (inputJetAndFlavParticles[iA].user_index() == inputJetAndFlavParticles[iB].user_index()) {
          mergedFlavoured.set_user_index(inputJetAndFlavParticles[iA].user_index());
          std::cout << "       [VERBOSE] Keeping jet-parton association for [ " << iA << " ] and [ " << iB << " ]"<< std::endl;
        } else {
          mergedFlavoured.set_user_index(0);
          std::cout << "       [VERBOSE] Removing jet-parton association for [ " << iA << " ] and [ " << iB << " ]"<< std::endl;
        }
        fastjet::contrib::FlavInfo flav = fastjet::contrib::FlavHistory::current_flavour_of(inputJetAndFlavParticles[iA]) +
                                          fastjet::contrib::FlavHistory::current_flavour_of(inputJetAndFlavParticles[iB]);
        ghsAlgoFlavRecombinerPtr_->apply_summation_choice(flav);
        /// set FlavInfo attribute
        mergedFlavoured.set_user_info(new fastjet::contrib::FlavHistory(flav));
        inputJetAndFlavParticles.push_back(mergedFlavoured);
        nnh.merge_jets(iA, iB, mergedFlavoured, inputJetAndFlavParticles.size() - 1);
      }
    } else {
      nnh.remove_jet(iA);
      std::cout << "       [VERBOSE] Removing jet [ " << iA << " ] which gives dij with beam."<< std::endl;
    }
    std::cout << ">>>>>> [VERBOSE] End of GHS Clustering Step <<<<<<" << std::endl;
  }
  for (unsigned i = 0; i < outputGHSAlgoJets.size(); i++) {
    finalJetsFlavInfo[i].update_flavourless_attribute();
    outputGHSAlgoJets[i].set_user_info(new fastjet::contrib::FlavHistory(fastjet::contrib::FlavInfo(finalJetsFlavInfo[i])));
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void JetFlavourClustering::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetFlavourClustering);

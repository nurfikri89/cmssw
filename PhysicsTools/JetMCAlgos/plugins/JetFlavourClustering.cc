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
#include "fastjet/contrib/GHSAlgo.hh"
#include "fastjet/contrib/IFNPlugin.hh"

#include "fastjet/NNH.hh"

//
// constants, enums and typedefs
//
typedef std::shared_ptr<fastjet::ClusterSequence> ClusterSequencePtr;
typedef std::shared_ptr<fastjet::JetDefinition> JetDefPtr;
typedef std::shared_ptr<fastjet::JetDefinition::Plugin> PluginPtr;

//
// class declaration
//
class JetFlavourClustering : public edm::stream::EDProducer<> {
public:
  explicit JetFlavourClustering(const edm::ParameterSet&);
  ~JetFlavourClustering() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  template <typename OutputIt>
  void insertGhosts(const edm::Handle<reco::GenParticleRefVector>& particles,
                    const double ghostRescaling,
                    const bool withPdgId,
                    OutputIt out);
  void matchReclusteredJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                            const std::vector<fastjet::PseudoJet>& matchedJets,
                            std::vector<int>& matchedIndices,
                            const bool allowUnmatchedReclusteredJets);
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

  void assignToSubjets(const reco::GenParticleRefVector& clusteredParticles, const edm::Handle<edm::View<reco::Jet>>& subjets,
                       const std::vector<int>& subjetIndices,
                       std::vector<reco::GenParticleRefVector>& assignedParticles);

  void assignToSubjets(const std::vector<fastjet::PseudoJet>& clusteredParticles, const edm::Handle<edm::View<reco::Jet>>& subjets,
                       const std::vector<int>& subjetIndices,
                       std::vector<std::vector<fastjet::PseudoJet>>& assignedParticles);

  // ----------member data ---------------------------
  const edm::EDGetTokenT<edm::View<reco::Jet>> jetsToken_;            // Input jet collection
  edm::EDGetTokenT<edm::View<reco::Jet>> groomedJetsToken_;           // Input groomed jet collection
  edm::EDGetTokenT<edm::View<reco::Jet>> subjetsToken_;               // Input subjet collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> bHadronsToken_;  // Input b hadron collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> cHadronsToken_;  // Input c hadron collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> partonsToken_;       // Input parton collection
  const edm::EDGetTokenT<reco::GenParticleRefVector> finalPartonsToken_;  // Input final parton collection
  edm::EDGetTokenT<edm::ValueMap<float>> weightsToken_;                   // Input weights collection
  edm::EDGetTokenT<reco::GenParticleRefVector> leptonsToken_;             // Input lepton collection

  const std::string jetAlgorithm_;
  const std::string subJetAlgorithm_;
  const double rParam_;
  const double rParamSubjets_;
  const double jetPtMin_;
  const double ghostRescaling_;
  const double relPtTolerance_;
  const bool hadronFlavourHasPriority_;

  /// GHS algorithm
  struct GHSParams {
    bool enabled;
    double alpha;
    double omega;
    double ptMin;
    std::string flavSummationScheme;
    std::shared_ptr<fastjet::contrib::FlavRecombiner> flavRecombiner;
  };
  GHSParams ghsParams_;

  /// IFN algorithm
  struct IFNParams {
    bool enabled;
    double alpha;
    double omega;
    std::string flavSummationScheme;
    fastjet::contrib::FlavRecombiner::FlavSummation flavSummation;
    std::shared_ptr<fastjet::contrib::FlavRecombiner> flavRecombiner;
  };
  IFNParams ifnParams_;

  const bool useSubjets_;

  const bool useLeptons_;

  ClusterSequencePtr fjClusterSeq_;
  JetDefPtr fjJetDefinition_;
  JetDefPtr fjSubjetDefinition_;

  PluginPtr fjPlugin_ifn_;
  JetDefPtr fjJetDefinition_ifn_;
  ClusterSequencePtr fjClusterSeq_ifn_;
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
      finalPartonsToken_(consumes<reco::GenParticleRefVector>(iConfig.getParameter<edm::InputTag>("finalPartons"))),
      /// Input gen particles collection, only needed when some new jet flavour definition is used.
      jetAlgorithm_(iConfig.getParameter<std::string>("jetAlgorithm")),
      subJetAlgorithm_(iConfig.exists("subjetAlgorithm") ? iConfig.getParameter<std::string>("subjetAlgorithm") : iConfig.getParameter<std::string>("jetAlgorithm")),
      rParam_(iConfig.getParameter<double>("rParam")),
      rParamSubjets_(iConfig.exists("rParamSubjets") ? iConfig.getParameter<double>("rParamSubjets") : 0.3),

      jetPtMin_(
          0.),  // hardcoded to 0. since we simply want to recluster all input jets which already had some PtMin applied
      ghostRescaling_(iConfig.exists("ghostRescaling") ? iConfig.getParameter<double>("ghostRescaling") : 1e-18),
      relPtTolerance_(
          iConfig.exists("relPtTolerance")
              ? iConfig.getParameter<double>("relPtTolerance")
              : 1e-03),  // 0.1% relative difference in Pt should be sufficient to detect possible misconfigurations
      hadronFlavourHasPriority_(iConfig.getParameter<bool>("hadronFlavourHasPriority")),

      useSubjets_(iConfig.exists("groomedJets") && iConfig.exists("subjets")),
      useLeptons_(iConfig.exists("leptons"))

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

  /// Configure flavour algorithms
  // GHS 
  if (iConfig.exists("ghsAlgorithm")) {
    edm::ParameterSet ghsPSet = iConfig.getParameter<edm::ParameterSet>("ghsAlgorithm");
    ghsParams_.enabled = ghsPSet.getParameter<bool>("enabled");
    
    if (ghsParams_.enabled) {
      ghsParams_.alpha = ghsPSet.getParameter<double>("alpha");
      ghsParams_.omega = ghsPSet.getParameter<double>("omega");
      ghsParams_.ptMin = ghsPSet.getParameter<double>("ptMin");
      ghsParams_.flavSummationScheme = ghsPSet.getParameter<std::string>("flavSummationScheme");
      
      // Set up the flavour recombiner based on summation scheme
      if (ghsParams_.flavSummationScheme == "net_flav" || ghsParams_.flavSummationScheme == "net") {
        ghsParams_.flavRecombiner =
            std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::net);
      } else if (ghsParams_.flavSummationScheme == "modulo_2" || ghsParams_.flavSummationScheme == "mod2") {
        ghsParams_.flavRecombiner =
            std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::modulo_2);
      } else if (ghsParams_.flavSummationScheme == "any_abs" || ghsParams_.flavSummationScheme == "any") {
        ghsParams_.flavRecombiner =
            std::make_shared<fastjet::contrib::FlavRecombiner>(fastjet::contrib::FlavRecombiner::FlavSummation::any_abs);
      } else {
        throw cms::Exception("InvalidGHSFlavourSummationScheme")
            << "GHS flavour summation scheme is invalid: " << ghsParams_.flavSummationScheme
            << ", use net_flav | modulo_2 | any_abs" << std::endl;
      }
    }
  } else {
    // Backward compatibility: if ghsAlgorithm PSet doesn't exist, GHS is disabled
    ghsParams_.enabled = false;
  }

  // IFN
  if (iConfig.exists("ifnAlgorithm")) {
    edm::ParameterSet ifnPSet = iConfig.getParameter<edm::ParameterSet>("ifnAlgorithm");
    ifnParams_.enabled = ifnPSet.getParameter<bool>("enabled");

    if (ifnParams_.enabled) {
      ifnParams_.alpha = ifnPSet.getParameter<double>("alpha");
      ifnParams_.omega = ifnPSet.getParameter<double>("omega");
      ifnParams_.flavSummationScheme = ifnPSet.getParameter<std::string>("flavSummationScheme");
      
      // Set up the flavour recombiner based on summation scheme
      if (ifnParams_.flavSummationScheme == "net_flav" || ifnParams_.flavSummationScheme == "net") {
          ifnParams_.flavSummation = fastjet::contrib::FlavRecombiner::FlavSummation::net;
          ifnParams_.flavRecombiner = std::make_shared<fastjet::contrib::FlavRecombiner>(ifnParams_.flavSummation);
      } else if (ifnParams_.flavSummationScheme == "modulo_2" || ifnParams_.flavSummationScheme == "mod2") {
          ifnParams_.flavSummation = fastjet::contrib::FlavRecombiner::FlavSummation::modulo_2;
          ifnParams_.flavRecombiner = std::make_shared<fastjet::contrib::FlavRecombiner>(ifnParams_.flavSummation);
      } else if (ifnParams_.flavSummationScheme == "any_abs" || ifnParams_.flavSummationScheme == "any") {
          ifnParams_.flavSummation = fastjet::contrib::FlavRecombiner::FlavSummation::any_abs;
          ifnParams_.flavRecombiner = std::make_shared<fastjet::contrib::FlavRecombiner>(ifnParams_.flavSummation);
      } else {
          throw cms::Exception("InvalidIFNFlavourSummationScheme") << "IFN flavour summation scheme is invalid: " << ifnParams_.flavSummationScheme
            << ", use net_flav | modulo_2 | any_abs" << std::endl;
      }
      fjPlugin_ifn_ = PluginPtr(
        new fastjet::contrib::IFNPlugin(
          *fjJetDefinition_,
          ifnParams_.alpha,
          ifnParams_.omega,
          ifnParams_.flavSummation
        )
      );
      fjJetDefinition_ifn_ = std::make_shared<fastjet::JetDefinition>(&*fjPlugin_ifn_);
    }
  } else {
    // Backward compatibility: if ifnAlgorithm PSet doesn't exist, IFN is disabled
    ifnParams_.enabled = false;
  }

  if (useSubjets_) {
    groomedJetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("groomedJets"));
    subjetsToken_ = consumes<edm::View<reco::Jet>>(iConfig.getParameter<edm::InputTag>("subjets"));

    // set subjet algorithm
    if (subJetAlgorithm_ == "Kt")
      fjSubjetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::kt_algorithm, rParamSubjets_);
    else if (subJetAlgorithm_ == "CambridgeAachen")
      fjSubjetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::cambridge_algorithm, rParamSubjets_);
    else if (subJetAlgorithm_ == "AntiKt")
      fjSubjetDefinition_ = std::make_shared<fastjet::JetDefinition>(fastjet::antikt_algorithm, rParamSubjets_);
    else
      throw cms::Exception("InvalidSubJetAlgorithm") << "Subjet clustering algorithm is invalid: " << subJetAlgorithm_
                                                      << ", use CambridgeAachen | Kt | AntiKt" << std::endl;
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

  edm::Handle<reco::GenParticleRefVector> finalPartons;
  iEvent.getByToken(finalPartonsToken_, finalPartons);

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
  unsigned int reserve = jets->size() * 128 + bHadrons->size() + cHadrons->size() + partons->size();
  if (useLeptons_)
    reserve += leptons->size();
  fjInputs.reserve(reserve);
  // loop over all input jets and collect all their constituents
  for (const auto& jet : *jets) {
    const bool weighted = jet.isWeighted();
    if (weighted && weightsToken_.isUninitialized())
      throw cms::Exception("MissingConstituentWeight")
          << "JetFlavourClustering: No weights given";
    const auto& constituents = jet.getJetConstituents();
    for (const auto& constit : constituents) {
      if (!constit.isNonnull() || !constit.isAvailable()) {
        edm::LogError("MissingJetConstituent")
            << "Jet constituent required for jet reclustering is missing.";
        continue;
      }
      if (constit->pt() == 0) {
        edm::LogWarning("NullTransverseMomentum")
            << "dropping input candidate with pt=0";
        continue;
      }
      const float w = weighted ? (*weights)[constit] : 1.f;
      fjInputs.emplace_back(
          constit->px() * w,
          constit->py() * w,
          constit->pz() * w,
          constit->energy() * w
      );
    }
  }

  // create the "ghosts"
  size_t bHadronStart = fjInputs.size();
  insertGhosts(bHadrons, ghostRescaling_, false, std::back_inserter(fjInputs));
  size_t cHadronStart = fjInputs.size();
  insertGhosts(cHadrons, ghostRescaling_, false, std::back_inserter(fjInputs));
  size_t partonStart = fjInputs.size();
  insertGhosts(partons, ghostRescaling_, true, std::back_inserter(fjInputs));
  std::vector<fastjet::PseudoJet> ghostFinalPartons;
  insertGhosts(finalPartons, ghostRescaling_, true, std::back_inserter(ghostFinalPartons)); // Final partons are not rescaled since they are only used for the optional IFN algorithm
  size_t leptonStart = fjInputs.size();
  if (useLeptons_) {
    insertGhosts(leptons, ghostRescaling_, false, std::back_inserter(fjInputs));
  }

  // initialize the indices matching reclustered jets to original jets
  std::vector<int> fjInputsMatchingIndices(fjInputs.size(), 0);
  // index the fastjet inputs to themselves as the order in fjInputs
  for(auto it = fjInputs.begin(); it != fjInputs.end(); ++it) {
    it->set_user_index(std::distance(fjInputs.begin(), it));
    if (!it->has_user_info()) {
      it->set_user_info(new fastjet::contrib::FlavHistory(0));
    }
  }

  if (ghsParams_.enabled) {
    // Set the recombiner for the fjJetDefinition to the GHS flavoured recombiner
    fjJetDefinition_->set_recombiner(ghsParams_.flavRecombiner.get());
  }

  std::vector<int> ifnIndices;
  std::vector<fastjet::PseudoJet> ifnJets;
  if (ifnParams_.enabled) {
    fjClusterSeq_ifn_ = std::make_shared<fastjet::ClusterSequence>(ghostFinalPartons, *fjJetDefinition_ifn_);
    ifnJets = fastjet::sorted_by_pt(fjClusterSeq_ifn_->inclusive_jets());
    matchReclusteredJets(jets, ifnJets, ifnIndices, true);
  }

  // define jet clustering sequence
  fjClusterSeq_ = std::make_shared<fastjet::ClusterSequence>(fjInputs, *fjJetDefinition_);

  // recluster jet constituents and the inserted "ghosts"
  std::vector<fastjet::PseudoJet> inclusiveJets = fastjet::sorted_by_pt(fjClusterSeq_->inclusive_jets(jetPtMin_));

  if (inclusiveJets.size() < jets->size())
    edm::LogError("TooFewReclusteredJets")
        << "There are fewer reclustered (" << inclusiveJets.size() << ") than original jets (" << jets->size()
        << "). Please check that the jet algorithm and jet size match those used for the original jet collection.";

  // match reclustered and original jets
  std::vector<int> reclusteredIndices;
  matchReclusteredJets(jets, inclusiveJets, reclusteredIndices, false);

  std::vector<int> ghsIndices;
  std::vector<fastjet::PseudoJet> ghsJets;
  if (ghsParams_.enabled) {
    ghsJets = fastjet::contrib::run_GHS(inclusiveJets, ghsParams_.ptMin, ghsParams_.alpha, ghsParams_.omega, *ghsParams_.flavRecombiner);
    matchReclusteredJets(jets, ghsJets, ghsIndices, true);
  }

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

  // Find to which jet each ghost pseudojet belongs to
  std::vector<int> inputToJetIdx = fjClusterSeq_->particle_jet_indices(inclusiveJets);

  for (size_t i = 0; i < jets->size(); ++i) {
    reco::GenParticleRefVector clusteredbHadrons;
    reco::GenParticleRefVector clusteredcHadrons;
    reco::GenParticleRefVector clusteredPartons;
    reco::GenParticleRefVector clusteredLeptons;
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
        if (jets->at(i).pt() < 10.){  // special handling for low-Pt jets (Pt<10 GeV)
          // edm::LogWarning("JetPtMismatchAtLowPt")
          //     << "The reclustered and original jet " << i << " have different Pt's ("
          //     << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << jets->at(i).pt()
          //     << " GeV, respectively).\n"
          //     << "Please check that the jet algorithm and jet size match those used for the original jet collection "
          //        "and also make sure the original jets are uncorrected. In addition, make sure you are not using "
          //        "CaloJets which are presently not supported.\n"
          //     << "Since the mismatch is at low Pt (Pt<10 GeV), it is ignored and only a warning is issued.\n"
          //     << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision "
          //        "in which case make sure the original jet collection is produced and reclustering is performed in the "
          //        "same job.";
        }
        else{
          // edm::LogError("JetPtMismatch")
          //     << "The reclustered and original jet " << i << " have different Pt's ("
          //     << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << jets->at(i).pt()
          //     << " GeV, respectively).\n"
          //     << "Please check that the jet algorithm and jet size match those used for the original jet collection "
          //        "and also make sure the original jets are uncorrected. In addition, make sure you are not using "
          //        "CaloJets which are presently not supported.\n"
          //     << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision "
          //        "in which case make sure the original jet collection is produced and reclustering is performed in the "
          //        "same job.";
        }
      }


      // go through the indices of the reclustered jets and find the "ghost" particles clustered inside the matched reclustered jet
      for (size_t j = 0; j < inputToJetIdx.size(); ++j) {
        if (inputToJetIdx.at(j) == reclusteredIndices.at(i)) {
          if (j >= bHadronStart && j < cHadronStart) {
            clusteredbHadrons.push_back(bHadrons->at(j - bHadronStart));
          } else if (j >= cHadronStart && j < partonStart) {
            clusteredcHadrons.push_back(cHadrons->at(j - cHadronStart));
          } else if (j >= partonStart && j < leptonStart) {
            clusteredPartons.push_back(partons->at(j - partonStart));
          } else if (useLeptons_ && j >= leptonStart) {
            clusteredLeptons.push_back(leptons->at(j - leptonStart));
          }
        }      
      }

      int hadronFlavour = 0;  // default hadron flavour set to 0 (= undefined)
      int partonFlavour = 0;  // default parton flavour set to 0 (= undefined)

      // set hadron- and parton-based flavours
      setFlavours(clusteredbHadrons, clusteredcHadrons, clusteredPartons, hadronFlavour, partonFlavour);

      // set the JetFlavourInfo for this jet
      (*jetFlavourInfos)[jets->refAt(i)] = reco::JetFlavourInfo(
          clusteredbHadrons, clusteredcHadrons, clusteredPartons, clusteredLeptons, hadronFlavour, partonFlavour);

      if (ifnParams_.enabled) {
        if (ifnIndices.at(i) < 0) {
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(reco::FlavAlgo::kIFN, fastjet::contrib::FlavInfo(0));
        } else {
          std::string ifnDescription = fastjet::contrib::FlavHistory::current_flavour_of(ifnJets.at(ifnIndices.at(i))).description();
          // std::cout << "IFN flavour for jet " << i << ": " << ifnDescription << std::endl;
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(reco::FlavAlgo::kIFN, fastjet::contrib::FlavHistory::current_flavour_of(ifnJets.at(ifnIndices.at(i))));
        }
      }
      if (ghsParams_.enabled) {
        if (ghsIndices.at(i) < 0) {
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(reco::FlavAlgo::kGHS, fastjet::contrib::FlavInfo(0));
        } else {
          std::string ghsDescription = fastjet::contrib::FlavHistory::current_flavour_of(ghsJets.at(ghsIndices.at(i))).description();
          // std::cout << "GHS flavour for jet " << i << ": " << ghsDescription << std::endl;
          (*jetFlavourInfos)[jets->refAt(i)].setAlgoFlav(reco::FlavAlgo::kGHS, fastjet::contrib::FlavHistory::current_flavour_of(ghsJets.at(ghsIndices.at(i))));
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

  //deallocate only at the end of the event processing
  fjClusterSeq_.reset();

  fjClusterSeq_ifn_.reset();

  // put jet flavour infos in the event
  iEvent.put(std::move(jetFlavourInfos));
  // put subjet flavour infos in the event
  if (useSubjets_)
    iEvent.put(std::move(subjetFlavourInfos), "SubJets");
}

// ------------ method that creates "ghost" particles from GenParticles ------------
template<typename OutputIt>
void JetFlavourClustering::insertGhosts(const edm::Handle<reco::GenParticleRefVector>& particles, const double ghostRescaling, const bool withPdgId, OutputIt out) {

  for (const auto& it : *particles) {
    if (it->pt() == 0) {
      edm::LogInfo("NullTransverseMomentum") << "dropping input ghost candidate with pt=0";
      continue;
    }
    fastjet::PseudoJet ghost(it->px(), it->py(), it->pz(), it->energy());
    ghost *= ghostRescaling;  // rescale particle momentum
    if (withPdgId) {
      ghost.set_user_info(new fastjet::contrib::FlavHistory(it->pdgId()));
    }
    else {
      ghost.set_user_info(new fastjet::contrib::FlavHistory(0));
    }
    *out++ = ghost;
  }
}

// ------------ method that matches reclustered and original jets based on minimum dR ------------
void JetFlavourClustering::matchReclusteredJets(const edm::Handle<edm::View<reco::Jet>>& jets,
                                                const std::vector<fastjet::PseudoJet>& reclusteredJets,
                                                std::vector<int>& matchedIndices,
                                                const bool allowUnmatchedReclusteredJets) {
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
        if (allowUnmatchedReclusteredJets) {
          // edm::LogWarning("MatchedJetsFarApart")
          //     << "Matched reclustered jet " << matchedIdx << " and original jet " << j
          //     << " are separated by dR=" << sqrt(matchedDR2) << " which is greater than the jet size R=" << rParam_
          //     << ".\n"
          //     << "This is not expected so the matching of these two jets has been discarded. Please check that the jet "
          //        "algorithm and jet size match those used for the original jet collection.";
          matchedIdx = -1;
        } 
        else{
        // edm::LogError("JetMatchingFailed") << "Matched reclustered jet " << matchedIdx << " and original jet " << j
        //                                    << " are separated by dR=" << sqrt(matchedDR2)
        //                                    << " which is greater than the jet size R=" << rParam_ << ".\n"
        //                                    << "This is not expected so please check that the jet algorithm and jet "
        //                                       "size match those used for the original jet collection.";
        }
      } 
    else
        matchedLocks.at(matchedIdx) = true;
    } else{
      // edm::LogError("JetMatchingFailed") << "Matching reclustered to original jets failed. Please check that the jet "
      //                                       "algorithm and jet size match those used for the original jet collection.";
    }

    matchedIndices.push_back(matchedIdx);
  }
}

// ------------ method that matches groomed and original jets based on minimum dR. Also used for IFN parton jet matching ------------
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
  reco::GenParticleRef hardestBParton;
  reco::GenParticleRef hardestCParton;

  for (reco::GenParticleRefVector::const_iterator it = clusteredPartons.begin(); it != clusteredPartons.end(); ++it) {
    const reco::GenParticleRef& parton = *it;
    int absId = std::abs(parton->pdgId());

    // hardest parton overall
    if (hardestParton.isNull() || parton->pt() > hardestParton->pt())
      hardestParton = parton;

    // hardest light-flavour parton
    if (CandMCTagUtils::isLightParton(*parton))
      if (hardestLightParton.isNull() || parton->pt() > hardestLightParton->pt())
        hardestLightParton = parton;

    // hardest b parton
    if (absId == 5)
      if (hardestBParton.isNull() || parton->pt() > hardestBParton->pt())
        hardestBParton = parton;

    // hardest c parton
    if (absId == 4)
      if (hardestCParton.isNull() || parton->pt() > hardestCParton->pt())
        hardestCParton = parton;
  }

  // b gets priority over c, c gets priority over everything else
  reco::GenParticleRef flavourParton;
  if (hardestBParton.isNonnull())
    flavourParton = hardestBParton;
  else if (hardestCParton.isNonnull())
    flavourParton = hardestCParton;

  // set hadron-based flavour
  if (!clusteredbHadrons.empty())
    hadronFlavour = 5;
  else if (!clusteredcHadrons.empty())
    hadronFlavour = 4;

  // set parton-based flavour
  if (flavourParton.isNull()) {
    if (hardestParton.isNonnull())
      partonFlavour = hardestParton->pdgId();
  } else
    partonFlavour = flavourParton->pdgId();

  // if enabled, resolve conflicts between hadron- and parton-based flavours
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

void JetFlavourClustering::assignToSubjets(const std::vector<fastjet::PseudoJet>& clusteredParticles,
                                           const edm::Handle<edm::View<reco::Jet>>& subjets,
                                           const std::vector<int>& subjetIndices,
                                           std::vector<std::vector<fastjet::PseudoJet>>& assignedParticles) {
  // loop over clustered particles and assign them to different subjets based on smallest dR
  for (std::vector<fastjet::PseudoJet>::const_iterator it = clusteredParticles.begin(); it != clusteredParticles.end();
       ++it) {
    std::vector<double> dR2toSubjets;

    for (size_t sj = 0; sj < subjetIndices.size(); ++sj)
      dR2toSubjets.push_back(reco::deltaR2(it->rapidity(),
                                           it->phi(),
                                           subjets->at(subjetIndices.at(sj)).rapidity(),
                                           subjets->at(subjetIndices.at(sj)).phi()));

    // find the closest subjet
    int closestSubjetIdx =
        std::distance(dR2toSubjets.begin(), std::min_element(dR2toSubjets.begin(), dR2toSubjets.end()));

    assignedParticles.at(closestSubjetIdx).push_back(*it);
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

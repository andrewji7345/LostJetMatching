// Record the number of reco fatjets 'kept, matched', 'kept, unmatched', 'lost, matched', and 'lost, unmatched'
//    to gen chi daughter particles both per-chi and globally
// Record positions of reco fatjets and gen chi daughter particles for event display purposes
// Strictly an improvement on JetMatching.cc

#include <memory>
#include <vector>
#include <cmath>
#include <unordered_set>

// Framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

// Data formats
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/deltaR.h"

// ROOT
#include "TH2F.h"
#include "TTree.h"

// TFileService
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

//--------------------------------------------------
// Find last copy of a particle
//--------------------------------------------------

const reco::Candidate* findLastCopy(const reco::Candidate* p)
{
  const reco::Candidate* current = p;

  bool foundSameParticle = true;

  while (foundSameParticle) {

    foundSameParticle = false;

    for (unsigned int i=0; i<current->numberOfDaughters(); i++) {

      const reco::Candidate* d = current->daughter(i);

      if (d->pdgId() == current->pdgId()) {

        current = d;
        foundSameParticle = true;
        break;
      }
    }
  }

  return current;
}

class LostJetMatching : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit LostJetMatching(const edm::ParameterSet&);
  ~LostJetMatching() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  //------------------------------------
  // Tokens
  //------------------------------------

  edm::EDGetTokenT<std::vector<reco::GenParticle>> genToken_;

  edm::EDGetTokenT<std::vector<pat::Jet>> jetToken_;

  edm::EDGetTokenT<std::vector<pat::Jet>> fatJetToken_;

  //------------------------------------
  // Signal PDG ID
  //------------------------------------

  int chiID_;

  //------------------------------------
  // Histograms
  //------------------------------------

  TH2F* hnKeptMatchChiPt_;
  TH2F* hnKeptUnmatchChiPt_;
  TH2F* hnLostMatchChiPt_;
  TH2F* hnLostUnmatchChiPt_;
  TH2F* hnKeptMatchChiPt_perChi_;
  TH2F* hnKeptUnmatchChiPt_perChi_;
  TH2F* hnLostMatchChiPt_perChi_;
  TH2F* hnLostUnmatchChiPt_perChi_;
  TH2F* hEtaKeptMatchChiPt_;
  TH2F* hEtaKeptUnmatchChiPt_;
  TH2F* hEtaLostMatchChiPt_;
  TH2F* hEtaLostUnmatchChiPt_;
  TH1F* hMatchJetPt_;
  TH1F* hUnmatchJetPt_;
  TH2F* hAllJetEtaPt_;

  // Event Display
  TTree* tree_;
  std::vector<float> gen_eta_;
  std::vector<float> gen_phi_;
  std::vector<float> gen_pt_;
  std::vector<int>   gen_label_;   // encode particle type

  std::vector<float> kept_matched_jet_eta_;
  std::vector<float> kept_matched_jet_phi_;
  std::vector<float> kept_matched_jet_pt_;
  std::vector<int>   kept_matched_jet_index_;

  std::vector<float> kept_unmatched_jet_eta_;
  std::vector<float> kept_unmatched_jet_phi_;
  std::vector<float> kept_unmatched_jet_pt_;
  std::vector<int>   kept_unmatched_jet_index_;

  std::vector<float> lost_matched_jet_eta_;
  std::vector<float> lost_matched_jet_phi_;
  std::vector<float> lost_matched_jet_pt_;
  std::vector<int>   lost_matched_jet_index_;

  std::vector<float> lost_unmatched_jet_eta_;
  std::vector<float> lost_unmatched_jet_phi_;
  std::vector<float> lost_unmatched_jet_pt_;
  std::vector<int>   lost_unmatched_jet_index_;

  int                n_kept_matched_jets_;
  int                n_kept_unmatched_jets_;
  int                n_lost_matched_jets_;
  int                n_lost_unmatched_jets_;
};

//------------------------------------
// Constructor
//------------------------------------

LostJetMatching::LostJetMatching(const edm::ParameterSet& iConfig)
    : genToken_(
          consumes<std::vector<reco::GenParticle>>(
              iConfig.getParameter<edm::InputTag>("genParticles"))),

      jetToken_(
          consumes<std::vector<pat::Jet>>(
              iConfig.getParameter<edm::InputTag>("jets"))),

      fatJetToken_(
          consumes<std::vector<pat::Jet>>(
              iConfig.getParameter<edm::InputTag>("fatjets"))),

      chiID_(
          iConfig.getParameter<int>("chiID"))

{
  usesResource("TFileService");

  edm::Service<TFileService> fs;

  //------------------------------------
  // Define histograms
  //------------------------------------

  // Event-wide
  hnKeptMatchChiPt_ = fs->make<TH2F>(
      "nKeptMatch_vs_avgchipt",
      ";#chi p_{T} (avg);nKeptMatch_{reco jet}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnKeptUnmatchChiPt_ = fs->make<TH2F>(
      "nKeptUnmatch_vs_avgchipt",
      ";#chi p_{T} (avg);nKeptUnmatch_{reco jet}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnLostMatchChiPt_ = fs->make<TH2F>(
      "nLostMatch_vs_avgchipt",
      ";#chi p_{T} (avg);nLostMatch_{reco jet}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnLostUnmatchChiPt_ = fs->make<TH2F>(
      "nLostUnmatch_vs_avgchipt",
      ";#chi p_{T} (avg);nLostUnmatch_{reco jet}",
      100, 0., 5000.,
      8, 0., 8.);

  // Per chi
  hnKeptMatchChiPt_perChi_ = fs->make<TH2F>(
      "nKeptMatch_vs_chipt_perChi",
      ";#chi p_{T};nKeptMatch_{reco jet, per #chi}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnKeptUnmatchChiPt_perChi_ = fs->make<TH2F>(
      "nKeptUnmatch_vs_chipt_perChi",
      ";#chi p_{T};nKeptUnmatch_{reco jet, per #chi}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnLostMatchChiPt_perChi_ = fs->make<TH2F>(
      "nLostMatch_vs_chipt_perChi",
      ";#chi p_{T};nLostMatch_{reco jet, per #chi}",
      100, 0., 5000.,
      8, 0., 8.);  

  hnLostUnmatchChiPt_perChi_ = fs->make<TH2F>(
      "nLostUnmatch_vs_chipt_perChi",
      ";#chi p_{T};nLostUnmatch_{reco jet, per #chi}",
      100, 0., 5000.,
      8, 0., 8.);

  // jet etas

  hEtaKeptMatchChiPt_ = fs->make<TH2F>(
      "etaKeptMatch_vs_avgchipt",
      ";|#eta_{reco jet, kept, matched}|;#chi p_{T} (avg)",
      100, 0., 3.,
      100, 0., 5000.);

  hEtaKeptUnmatchChiPt_ = fs->make<TH2F>(
      "etaKeptUnmatch_vs_avgchipt",
      ";|#eta_{reco jet, kept, unmatched}|;#chi p_{T} (avg)",
      100, 0., 3.,
      100, 0., 5000.);

  hEtaLostMatchChiPt_ = fs->make<TH2F>(
      "etaLostMatch_vs_avgchipt",
      ";|#eta_{reco jet, lost, matched}|;#chi p_{T} (avg)",
      100, 0., 3.,
      100, 0., 5000.);

  hEtaLostUnmatchChiPt_ = fs->make<TH2F>(
      "etaLostUnmatch_vs_avgchipt",
      ";|#eta_{reco jet, lost, unmatched}|;#chi p_{T} (avg)",
      100, 0., 3.,
      100, 0., 5000.);

  hMatchJetPt_ = fs->make<TH1F>(
      "matchjetpt",
      ";matched reco jet p_{T};count",
      100,0.,5000.);

  hUnmatchJetPt_ = fs->make<TH1F>(
      "unmatchjetpt",
      ";unmatched reco jet p_{T};count",
      100,0.,5000.);

  hAllJetEtaPt_ = fs->make<TH2F>(
      "eta_vs_jetpt",
      ";|#eta_{reco jet, all}|;reco jet p_{T}",
      100, 0., 3.,
      100, 0., 5000.);

  tree_ = fs->make<TTree>("eventDisplay", "eta-phi event display");

  tree_->Branch("gen_eta", &gen_eta_);
  tree_->Branch("gen_phi", &gen_phi_);
  tree_->Branch("gen_pt",  &gen_pt_);
  tree_->Branch("gen_label", &gen_label_);

  tree_->Branch("kept_matched_jet_eta", &kept_matched_jet_eta_);
  tree_->Branch("kept_matched_jet_phi", &kept_matched_jet_phi_);
  tree_->Branch("kept_matched_jet_pt", &kept_matched_jet_pt_);
  tree_->Branch("kept_matched_jet_index", &kept_matched_jet_index_);

  tree_->Branch("kept_unmatched_jet_eta", &kept_unmatched_jet_eta_);
  tree_->Branch("kept_unmatched_jet_phi", &kept_unmatched_jet_phi_);
  tree_->Branch("kept_unmatched_jet_pt",  &kept_unmatched_jet_pt_);
  tree_->Branch("kept_unmatched_jet_index", &kept_unmatched_jet_index_);

  tree_->Branch("lost_matched_jet_eta", &lost_matched_jet_eta_);
  tree_->Branch("lost_matched_jet_phi", &lost_matched_jet_phi_);
  tree_->Branch("lost_matched_jet_pt",  &lost_matched_jet_pt_);
  tree_->Branch("lost_matched_jet_index", &lost_matched_jet_index_);

  tree_->Branch("lost_unmatched_jet_eta", &lost_unmatched_jet_eta_);
  tree_->Branch("lost_unmatched_jet_phi", &lost_unmatched_jet_phi_);
  tree_->Branch("lost_unmatched_jet_pt",  &lost_unmatched_jet_pt_);
  tree_->Branch("lost_unmatched_jet_index", &lost_unmatched_jet_index_);

  tree_->Branch("n_kept_matched_jets", &n_kept_matched_jets_);
  tree_->Branch("n_kept_unmatched_jets", &n_kept_unmatched_jets_);
  tree_->Branch("n_lost_matched_jets", &n_lost_matched_jets_);
  tree_->Branch("n_lost_unmatched_jets", &n_lost_unmatched_jets_);
  }

//------------------------------------
// Event loop
//------------------------------------

void LostJetMatching::analyze(const edm::Event& iEvent,
                          const edm::EventSetup&) {

  //------------------------------------
  // Retrieve collections
  //------------------------------------

  edm::Handle<std::vector<reco::GenParticle>> gens;
  iEvent.getByToken(genToken_, gens);

  edm::Handle<std::vector<pat::Jet>> jets;
  iEvent.getByToken(jetToken_, jets);

  edm::Handle<std::vector<pat::Jet>> fatjets;
  iEvent.getByToken(fatJetToken_, fatjets);

  // define vector of chi pts
  std::vector<double> p_pt =  {};

  // Clear event display vars
  gen_eta_.clear();
  gen_phi_.clear();
  gen_pt_.clear();
  gen_label_.clear();

  kept_matched_jet_eta_.clear();
  kept_matched_jet_phi_.clear();
  kept_matched_jet_pt_.clear();
  kept_matched_jet_index_.clear();

  kept_unmatched_jet_eta_.clear();
  kept_unmatched_jet_phi_.clear();
  kept_unmatched_jet_pt_.clear();
  kept_unmatched_jet_index_.clear();

  lost_matched_jet_eta_.clear();
  lost_matched_jet_phi_.clear();
  lost_matched_jet_pt_.clear();
  lost_matched_jet_index_.clear();

  lost_unmatched_jet_eta_.clear();
  lost_unmatched_jet_phi_.clear();
  lost_unmatched_jet_pt_.clear();
  lost_unmatched_jet_index_.clear();

  //n_kept_matched_jets_.clear();
  //n_kept_unmatched_jets_.clear();
  //n_lost_matched_jets_.clear();
  //n_lost_unmatched_jets_.clear();
  int n_kept_matched_jets = 0;
  int n_kept_unmatched_jets = 0;
  int n_lost_matched_jets = 0;
  int n_lost_unmatched_jets = 0;
  int n_kept_matched_jets_perChi = 0;
  int n_kept_unmatched_jets_perChi = 0;
  int n_lost_matched_jets_perChi = 0;
  int n_lost_unmatched_jets_perChi = 0;

  std::vector<bool> global_matched(fatjets->size(), false);
  std::vector<bool> global_lost(fatjets->size(), false);
  std::vector<bool> matched(fatjets->size(), false);
  std::vector<bool> lost(fatjets->size(), false);

  // Determine how many jets are lost
  for (size_t i = 0; i < fatjets->size(); ++i) {
    if (fatjets->at(i).pt() < 300) {
      global_lost[i] = true;
      lost[i] = true;
    }
  }

  //------------------------------------
  // Loop over GenParticles
  //------------------------------------

  for (const auto &p : *gens) {

    //------------------------------------
    // Select χ particles
    //------------------------------------

    if (std::abs(p.pdgId()) != chiID_) {
      continue;
    }

    if (!p.isLastCopy()) {
      continue;
    }

    //------------------------------------
    // Find daughters
    //------------------------------------

    const reco::Candidate* W = nullptr;
    const reco::Candidate* W_q_0 = nullptr;
    const reco::Candidate* W_q_1 = nullptr;
    const reco::Candidate* b = nullptr;
    const reco::Candidate* Z = nullptr;
    const reco::Candidate* Z_q_0 = nullptr;
    const reco::Candidate* Z_q_1 = nullptr;
    const reco::Candidate* h = nullptr;
    const reco::Candidate* h_q_0 = nullptr;
    const reco::Candidate* h_q_1 = nullptr;
    const reco::Candidate* t = nullptr;
    const reco::Candidate* t_W = nullptr;
    const reco::Candidate* t_W_q_0 = nullptr;
    const reco::Candidate* t_W_q_1 = nullptr;
    const reco::Candidate* t_b = nullptr;

    n_kept_matched_jets_perChi = 0;
    n_kept_unmatched_jets_perChi = 0;
    n_lost_matched_jets_perChi = 0;
    n_lost_unmatched_jets_perChi = 0;
    std::fill(matched.begin(), matched.end(), false);

    bool valid_chi = true;
    
    for (unsigned int i=0; i<p.numberOfDaughters(); i++) {

      const reco::Candidate* daughter = p.daughter(i);

      // descend to last copy
      daughter = findLastCopy(daughter);

      // determine if decay is Wb, ht, or Zt
      if (std::abs(daughter->pdgId()) == 24) {

        W = daughter;

        if (W->numberOfDaughters() != 2) {
          valid_chi = false; // skip particle if W has anything other than 2 daughters
          break;
        }

        if (std::abs(W->daughter(0)->pdgId()) > 6 || std::abs(W->daughter(1)->pdgId()) > 6) {
          valid_chi = false; // skip particle if W has non-quark daughters
          break;
        }

        const reco::Candidate* granddaughter_0 = W->daughter(0);
        const reco::Candidate* granddaughter_1 = W->daughter(1);

        W_q_0 = findLastCopy(granddaughter_0);
        W_q_1 = findLastCopy(granddaughter_1);

      }

      else if (std::abs(daughter->pdgId()) == 5) {
        b = daughter;
      }

      else if (std::abs(daughter->pdgId()) == 23) {
        Z = daughter;

        if (Z->numberOfDaughters() != 2) {
          valid_chi = false; // skip particle if Z has more than 2 daughters
          break;
        }

        if (std::abs(Z->daughter(0)->pdgId()) > 6 || std::abs(Z->daughter(1)->pdgId()) > 6) {
          valid_chi = false; // skip particle if Z has non-quark daughters
          break;
        }

        const reco::Candidate* granddaughter_0 = Z->daughter(0);
        const reco::Candidate* granddaughter_1 = Z->daughter(1);

        Z_q_0 = findLastCopy(granddaughter_0);
        Z_q_1 = findLastCopy(granddaughter_1);

      }

      else if (std::abs(daughter->pdgId()) == 25) {
        h = daughter;

        if (h->numberOfDaughters() != 2) {
          valid_chi = false; // skip particle if h has more than 2 daughters
          break;
        }

        if (std::abs(h->daughter(0)->pdgId()) > 6 || std::abs(h->daughter(1)->pdgId()) > 6) {
          valid_chi = false; // skip particle if h has non-quark daughters
          break;
        }

        const reco::Candidate* granddaughter_0 = h->daughter(0);
        const reco::Candidate* granddaughter_1 = h->daughter(1);

        h_q_0 = findLastCopy(granddaughter_0);
        h_q_1 = findLastCopy(granddaughter_1);
      }

      else if (std::abs(daughter->pdgId()) == 6) {
        t = daughter;

        if (t->numberOfDaughters() != 2) {
          valid_chi = false; // skip particle if h does not have 2 daughters
          break;
        }

        if (std::abs(t->daughter(0)->pdgId()) == 24 && std::abs(t->daughter(1)->pdgId()) == 5) {
          const reco::Candidate* granddaughter_0 = t->daughter(0);
          const reco::Candidate* granddaughter_1 = t->daughter(1);

          t_W = findLastCopy(granddaughter_0);
          t_b = findLastCopy(granddaughter_1);

          if (t_W->numberOfDaughters() != 2) {
            valid_chi = false; // skip particle if W has more than 2 daughters
            break;
          }

          if (std::abs(t_W->daughter(0)->pdgId()) > 6 || std::abs(t_W->daughter(1)->pdgId()) > 6) {
            valid_chi = false; // skip particle if W has non-quark daughters
            break;
          }

          const reco::Candidate* greatgranddaughter_0 = t_W->daughter(0);
          const reco::Candidate* greatgranddaughter_1 = t_W->daughter(1);

          t_W_q_0 = findLastCopy(greatgranddaughter_0);
          t_W_q_1 = findLastCopy(greatgranddaughter_1);
        }

        else if (std::abs(t->daughter(0)->pdgId()) == 5 && std::abs(t->daughter(1)->pdgId()) == 24) {
          const reco::Candidate* granddaughter_0 = t->daughter(0);
          const reco::Candidate* granddaughter_1 = t->daughter(1);

          t_b = findLastCopy(granddaughter_0);
          t_W = findLastCopy(granddaughter_1);

          if (t_W->numberOfDaughters() != 2) {
            valid_chi = false; // skip particle if W has more than 2 daughters
            break;
          }

          if (std::abs(t_W->daughter(0)->pdgId()) > 6 || std::abs(t_W->daughter(1)->pdgId()) > 6) {
            valid_chi = false; // skip particle if W has non-quark daughters
            break;
          }

          const reco::Candidate* greatgranddaughter_0 = t_W->daughter(0);
          const reco::Candidate* greatgranddaughter_1 = t_W->daughter(1);

          t_W_q_0 = findLastCopy(greatgranddaughter_0);
          t_W_q_1 = findLastCopy(greatgranddaughter_1);
        }
      }
    }

    if (!valid_chi){continue;}

    //------------------------------------
    // Require all correct daughters, granddaughters, greatgranddaughters
    //------------------------------------
    if (W && b) {
      if (!W_q_0 || !W_q_1) {
        valid_chi = false;
        continue;
      }

      // store event display information
      gen_eta_.push_back(W_q_0->eta());
      gen_phi_.push_back(W_q_0->phi());
      gen_pt_.push_back(W_q_0->pt());
      gen_label_.push_back(1); // W_q_0

      gen_eta_.push_back(W_q_1->eta());
      gen_phi_.push_back(W_q_1->phi());
      gen_pt_.push_back(W_q_1->pt());
      gen_label_.push_back(2); // W_q_1

      gen_eta_.push_back(b->eta());
      gen_phi_.push_back(b->phi());
      gen_pt_.push_back(b->pt());
      gen_label_.push_back(3); // b
    }
    else if (Z && t) {
      if (!Z_q_0 || !Z_q_1 || !t_W || !t_b || !t_W_q_0 || !t_W_q_1) {
        valid_chi = false;
        continue;
      }

      // store event display information
      gen_eta_.push_back(Z_q_0->eta());
      gen_phi_.push_back(Z_q_0->phi());
      gen_pt_.push_back(Z_q_0->pt());
      gen_label_.push_back(4); // Z_q_0

      gen_eta_.push_back(Z_q_1->eta());
      gen_phi_.push_back(Z_q_1->phi());
      gen_pt_.push_back(Z_q_1->pt());
      gen_label_.push_back(5); // Z_q_1

      gen_eta_.push_back(t->eta());
      gen_phi_.push_back(t->phi());
      gen_pt_.push_back(t->pt());
      gen_label_.push_back(6); // t

      gen_eta_.push_back(t_W_q_0->eta());
      gen_phi_.push_back(t_W_q_0->phi());
      gen_pt_.push_back(t_W_q_0->pt());
      gen_label_.push_back(7); // t_W_q_0

      gen_eta_.push_back(t_W_q_1->eta());
      gen_phi_.push_back(t_W_q_1->phi());
      gen_pt_.push_back(t_W_q_1->pt());
      gen_label_.push_back(8); // t_W_q_1

      gen_eta_.push_back(t_b->eta());
      gen_phi_.push_back(t_b->phi());
      gen_pt_.push_back(t_b->pt());
      gen_label_.push_back(9); // t_b

      gen_eta_.push_back(t_W->eta());
      gen_phi_.push_back(t_W->phi());
      gen_pt_.push_back(t_W->pt());
      gen_label_.push_back(12); // t_W
    }
    else if (h && t) {
      if (!h_q_0 || !h_q_1 || !t_b || !t_W || !t_W_q_0 || !t_W_q_1) {
        valid_chi = false;
        continue;
      }

      // store event display information
      gen_eta_.push_back(h_q_0->eta());
      gen_phi_.push_back(h_q_0->phi());
      gen_pt_.push_back(h_q_0->pt());
      gen_label_.push_back(10); // h_q_0

      gen_eta_.push_back(h_q_1->eta());
      gen_phi_.push_back(h_q_1->phi());
      gen_pt_.push_back(h_q_1->pt());
      gen_label_.push_back(11); // h_q_1

      gen_eta_.push_back(t->eta());
      gen_phi_.push_back(t->phi());
      gen_pt_.push_back(t->pt());
      gen_label_.push_back(6); // t

      gen_eta_.push_back(t_W_q_0->eta());
      gen_phi_.push_back(t_W_q_0->phi());
      gen_pt_.push_back(t_W_q_0->pt());
      gen_label_.push_back(7); // t_W_q_0

      gen_eta_.push_back(t_W_q_1->eta());
      gen_phi_.push_back(t_W_q_1->phi());
      gen_pt_.push_back(t_W_q_1->pt());
      gen_label_.push_back(8); // t_W_q_1

      gen_eta_.push_back(t_b->eta());
      gen_phi_.push_back(t_b->phi());
      gen_pt_.push_back(t_b->pt());
      gen_label_.push_back(9); // t_b

      gen_eta_.push_back(t_W->eta());
      gen_phi_.push_back(t_W->phi());
      gen_pt_.push_back(t_W->pt());
      gen_label_.push_back(12); // t_W
    }
    else {
      continue; // not any of the allowed decays
    }

    if (!valid_chi) {continue;}

    // Find nearest AK8 jet for each jet parent; match if within 0.4

    int bestW_q_0Jet    = -1;
    int bestW_q_1Jet    = -1;
    int bestbJet        = -1;
    int bestZ_q_0Jet    = -1;
    int bestZ_q_1Jet    = -1;
    int besth_q_0Jet    = -1;
    int besth_q_1Jet    = -1;
    int bestt_W_q_0Jet  = -1;
    int bestt_W_q_1Jet  = -1;
    int bestt_bJet      = -1;
    double minDRW_q_0   = .4;
    double minDRW_q_1   = .4;
    double minDRb       = .4;
    double minDRZ_q_0   = .4;
    double minDRZ_q_1   = .4;
    double minDRh_q_0   = .4;
    double minDRh_q_1   = .4;
    double minDRt_W_q_0 = .4;
    double minDRt_W_q_1 = .4;
    double minDRt_b     = .4;

    for (unsigned int i = 0; i < fatjets->size(); i++) {
      if (W_q_0) {
        double dRW_q_0 =
          reco::deltaR(
            W_q_0->eta(),
            W_q_0->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRW_q_0 < minDRW_q_0) {
          minDRW_q_0 = dRW_q_0;
          bestW_q_0Jet = i;
        }
      }
      if (W_q_1) {
        double dRW_q_1 =
          reco::deltaR(
            W_q_1->eta(),
            W_q_1->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRW_q_1 < minDRW_q_1) {
          minDRW_q_1 = dRW_q_1;
          bestW_q_1Jet = i;
        }
      }
      if (b) {
        double dRb =
          reco::deltaR(
            b->eta(),
            b->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRb < minDRb) {
          minDRb = dRb;
          bestbJet = i;
        }
      }
      if (Z_q_0) {
        double dRZ_q_0 =
          reco::deltaR(
            Z_q_0->eta(),
            Z_q_0->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRZ_q_0 < minDRZ_q_0) {
          minDRZ_q_0 = dRZ_q_0;
          bestZ_q_0Jet = i;
        }
      }
      if (Z_q_1) {
        double dRZ_q_1 =
          reco::deltaR(
            Z_q_1->eta(),
            Z_q_1->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRZ_q_1 < minDRZ_q_1) {
          minDRZ_q_1 = dRZ_q_1;
          bestZ_q_1Jet = i;
        }
      }
      if (h_q_0) {
        double dRh_q_0 =
          reco::deltaR(
            h_q_0->eta(),
            h_q_0->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRh_q_0 < minDRh_q_0) {
          minDRh_q_0 = dRh_q_0;
          besth_q_0Jet = i;
        }
      }
      if (h_q_1) {
        double dRh_q_1 =
          reco::deltaR(
            h_q_1->eta(),
            h_q_1->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRh_q_1 < minDRh_q_1) {
          minDRh_q_1 = dRh_q_1;
          besth_q_1Jet = i;
        }
      }
      if (t_W_q_0) {
        double dRt_W_q_0 =
          reco::deltaR(
            t_W_q_0->eta(),
            t_W_q_0->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRt_W_q_0 < minDRt_W_q_0) {
          minDRt_W_q_0 = dRt_W_q_0;
          bestt_W_q_0Jet = i;
        }
      }
      if (t_W_q_1) {
        double dRt_W_q_1 =
          reco::deltaR(
            t_W_q_1->eta(),
            t_W_q_1->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRt_W_q_1 < minDRt_W_q_1) {
          minDRt_W_q_1 = dRt_W_q_1;
          bestt_W_q_1Jet = i;
        }
      }
      if (t_b) {
        double dRt_b =
          reco::deltaR(
            t_b->eta(),
            t_b->phi(),
            fatjets->at(i).eta(),
            fatjets->at(i).phi());
        if (dRt_b < minDRt_b) {
          minDRt_b = dRt_b;
          bestt_bJet = i;
        }
      }
    } // finish matching

    //------------------------------------
    // Fill histograms
    //------------------------------------

    // Find unique jet id's
    std::vector<int> all_matched_jet_ids_perChi = {bestW_q_0Jet, bestW_q_1Jet, bestbJet, bestZ_q_0Jet, bestZ_q_1Jet, besth_q_0Jet, besth_q_1Jet, bestt_W_q_0Jet, bestt_W_q_1Jet, bestt_bJet};
    std::unordered_set<int> all_unique_matched_jet_ids_perChi(all_matched_jet_ids_perChi.begin(), all_matched_jet_ids_perChi.end());

    // count as matched if unique AND matched
    for (const auto& unique_matched_jet_id_perChi: all_unique_matched_jet_ids_perChi) {
      if (unique_matched_jet_id_perChi != -1) { // skip if id = -1
        matched[unique_matched_jet_id_perChi] = true;
        global_matched[unique_matched_jet_id_perChi] = true;
      }
    }

    for (size_t i = 0; i < matched.size(); ++i) {
      if (matched[i] && !lost[i]){
        n_kept_matched_jets_perChi++;
      }
      else if (!matched[i] && !lost[i]){
        n_kept_unmatched_jets_perChi++;
      }
      else if (matched[i] && lost[i]){
        n_lost_matched_jets_perChi++;
      }
      else if (!matched[i] && lost[i]){
        n_lost_unmatched_jets_perChi++;
      }
    }

    // insert chi pt
    p_pt.push_back(p.pt());

    // fill nMatch vs chi pt hists (per chi)
    hnKeptMatchChiPt_perChi_->Fill(
      p.pt(),
      n_kept_matched_jets_perChi);
    hnKeptUnmatchChiPt_perChi_->Fill(
      p.pt(),
      n_kept_unmatched_jets_perChi);
    hnLostMatchChiPt_perChi_->Fill(
      p.pt(),
      n_lost_matched_jets_perChi);
    hnLostUnmatchChiPt_perChi_->Fill(
      p.pt(),
      n_lost_unmatched_jets_perChi);
  } // end loop over GenParticles

  for (size_t i = 0; i < matched.size(); ++i) {
    if (global_matched[i] && !lost[i]){
      kept_matched_jet_eta_.push_back(fatjets->at(i).eta());
      kept_matched_jet_phi_.push_back(fatjets->at(i).phi());
      kept_matched_jet_pt_.push_back(fatjets->at(i).pt());
      kept_matched_jet_index_.push_back(i);
      n_kept_matched_jets++;
    }
    else if (!global_matched[i] && !lost[i]){
      kept_unmatched_jet_eta_.push_back(fatjets->at(i).eta());
      kept_unmatched_jet_phi_.push_back(fatjets->at(i).phi());
      kept_unmatched_jet_pt_.push_back(fatjets->at(i).pt());
      kept_unmatched_jet_index_.push_back(i);
      n_kept_unmatched_jets++;
    }
    else if (global_matched[i] && lost[i]){
      lost_matched_jet_eta_.push_back(fatjets->at(i).eta());
      lost_matched_jet_phi_.push_back(fatjets->at(i).phi());
      lost_matched_jet_pt_.push_back(fatjets->at(i).pt());
      lost_matched_jet_index_.push_back(i);
      n_lost_matched_jets++;
    }
    else if (!global_matched[i] && lost[i]){
      lost_unmatched_jet_eta_.push_back(fatjets->at(i).eta());
      lost_unmatched_jet_phi_.push_back(fatjets->at(i).phi());
      lost_unmatched_jet_pt_.push_back(fatjets->at(i).pt());
      lost_unmatched_jet_index_.push_back(i);
      n_lost_unmatched_jets++;
    }
  }

  for (size_t i = 0; i < matched.size(); ++i) {
    if (global_matched[i]) {
      hMatchJetPt_->Fill(fatjets->at(i).pt());
    }
    else {
      hUnmatchJetPt_->Fill(fatjets->at(i).pt());
    }
  }

  // find average chi pt
  double p_sum_pt = 0;
  for (double pt : p_pt) {
      p_sum_pt += pt;
  }
  if (p_pt.size() == 0) { // if no chis found, return
    return;
  }
  double p_avg_pt = p_sum_pt / p_pt.size();
  
  // fill nMatch vs chi pt hists
  hnKeptMatchChiPt_->Fill(
    p_avg_pt,
    n_kept_matched_jets);
  hnKeptUnmatchChiPt_->Fill(
    p_avg_pt,
    n_kept_unmatched_jets);
  hnLostMatchChiPt_->Fill(
    p_avg_pt,
    n_lost_matched_jets);
  hnLostUnmatchChiPt_->Fill(
    p_avg_pt,
    n_lost_unmatched_jets);

  // Fill eta vs chi pt hists
  for (const auto& kept_matched_jet_id : kept_matched_jet_index_) {
    hEtaKeptMatchChiPt_->Fill(
      std::abs(fatjets->at(kept_matched_jet_id).eta()),
      p_avg_pt);
  }
  for (const auto& kept_unmatched_jet_id : kept_unmatched_jet_index_) {
    hEtaKeptUnmatchChiPt_->Fill(
      std::abs(fatjets->at(kept_unmatched_jet_id).eta()),
      p_avg_pt);
  }
  for (const auto& lost_matched_jet_id : lost_matched_jet_index_) {
    hEtaLostMatchChiPt_->Fill(
      std::abs(fatjets->at(lost_matched_jet_id).eta()),
      p_avg_pt);
  }
  for (const auto& lost_unmatched_jet_id : lost_unmatched_jet_index_) {
    hEtaLostUnmatchChiPt_->Fill(
      std::abs(fatjets->at(lost_unmatched_jet_id).eta()),
      p_avg_pt);
  }

  // Fill eta vs pt hist
  for (unsigned int i = 0; i < fatjets->size(); i++){
    hAllJetEtaPt_->Fill(
      std::abs(fatjets->at(i).eta()),
      std::abs(fatjets->at(i).pt())
    );
  }

  // Fill tree
  //n_kept_matched_jets_.push_back(n_kept_matched_jets);
  //n_kept_unmatched_jets_.push_back(n_kept_unmatched_jets);
  //n_lost_matched_jets_.push_back(n_lost_matched_jets);
  //n_lost_unmatched_jets_.push_back(n_lost_unmatched_jets);
  n_kept_matched_jets_ = n_kept_matched_jets;
  n_kept_unmatched_jets_ = n_kept_unmatched_jets;
  n_lost_matched_jets_ = n_lost_matched_jets;
  n_lost_unmatched_jets_ = n_lost_unmatched_jets;
  tree_->Fill();

} // end analyze()

//------------------------------------
// Parameter descriptions
//------------------------------------

void LostJetMatching::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>(
      "genParticles",
      edm::InputTag("prunedGenParticles"));

  desc.add<edm::InputTag>(
      "jets",
      edm::InputTag("slimmedJets"));

  desc.add<edm::InputTag>(
      "fatjets",
      edm::InputTag("slimmedJetsAK8"));

  desc.add<int>(
      "chiID",
      9936662);

  descriptions.add(
      "lostJetMatching",
      desc);
}

//------------------------------------
// Define plugin
//------------------------------------

DEFINE_FWK_MODULE(LostJetMatching);

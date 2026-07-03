import uproot
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from pathlib import Path

Path("plots").mkdir(parents=True, exist_ok=True)

#filename = "WbWb_4000_1000_"
#filename = "WbWb_8000_2000_"
#filename = "WbZt_4000_1000_"
#filename = "WbZt_8000_2000_"
#filename = "Wbht_4000_1000_"
#filename = "Wbht_8000_2000_"
#filename = "ZtZt_4000_1000_"
#filename = "ZtZt_8000_2000_"
#filename = "htZt_4000_1000_"
#filename = "htZt_8000_2000_"
#filename = "htht_4000_1000_"
#filename = "htht_8000_2000_"

#filename = "WbWb_8000_1000_"
#filename = "WbWb_8000_1500_"
#filename = "WbWb_8000_2500_"
filename = "WbWb_8000_3000_"

f = uproot.open("/eos/uscms/store/user/aji/rootfiles_lostJetMatching/" + filename + "LostJetMatching.root")
tree = f["lostJetMatching/eventDisplay;1"]

particle_key = {
    1: "W_q_0",
    2: "W_q_1",
    3: "b",
    4: "Z_q_0",
    5: "Z_q_1",
    6: "t",
    7: "t_W_q_0",
    8: "t_W_q_1",
    9: "t_b",
    10:"h_b_0",
    11:"h_b_1",
    12:"t_W",
}

jet_R = 0.8  # AK8
matching_R = 0.4 # Jet clustering
nEvents = 10 # How many to display

for i in range(nEvents):
    gen_eta = tree["gen_eta"].array(library="np")[i]
    gen_phi = tree["gen_phi"].array(library="np")[i]
    gen_pt  = tree["gen_pt"].array(library="np")[i]
    gen_lab = tree["gen_label"].array(library="np")[i]

    kept_matched_jet_eta = tree["kept_matched_jet_eta"].array(library="np")[i]
    kept_matched_jet_phi = tree["kept_matched_jet_phi"].array(library="np")[i]
    kept_matched_jet_pt  = tree["kept_matched_jet_pt"].array(library="np")[i]

    kept_unmatched_jet_eta = tree["kept_unmatched_jet_eta"].array(library="np")[i]
    kept_unmatched_jet_phi = tree["kept_unmatched_jet_phi"].array(library="np")[i]
    kept_unmatched_jet_pt  = tree["kept_unmatched_jet_pt"].array(library="np")[i]

    lost_matched_jet_eta = tree["lost_matched_jet_eta"].array(library="np")[i]
    lost_matched_jet_phi = tree["lost_matched_jet_phi"].array(library="np")[i]
    lost_matched_jet_pt  = tree["lost_matched_jet_pt"].array(library="np")[i]

    lost_unmatched_jet_eta = tree["lost_unmatched_jet_eta"].array(library="np")[i]
    lost_unmatched_jet_phi = tree["lost_unmatched_jet_phi"].array(library="np")[i]
    lost_unmatched_jet_pt  = tree["lost_unmatched_jet_pt"].array(library="np")[i]

    n_kept_matched_jets  = tree["n_kept_matched_jets"].array(library="np")[i]
    n_kept_unmatched_jets  = tree["n_kept_unmatched_jets"].array(library="np")[i]
    n_lost_matched_jets  = tree["n_lost_matched_jets"].array(library="np")[i]
    n_lost_unmatched_jets  = tree["n_lost_unmatched_jets"].array(library="np")[i]

    fig, ax = plt.subplots(figsize=(7, 6))
    ax.minorticks_on()
    ax.grid(which='major', color='black', linestyle='-')

    # frame
    ax.set_xlim(-5, 5)
    ax.set_ylim(-3.2, 3.2)
    ax.set_xlabel(r"$\eta$")
    ax.set_ylabel(r"$\phi$")
    ax.set_title(f"{filename}: Event {i}, {n_kept_matched_jets + n_lost_matched_jets} matched jets / {n_kept_matched_jets + n_lost_matched_jets + n_kept_unmatched_jets + n_lost_unmatched_jets} total jets")

    # GEN particles (colored markers)
    ax.scatter(gen_eta, gen_phi, s=60, marker=".", c="blue", alpha=0.3, label="Gen daughters")
    for eta, phi, label in zip(gen_eta, gen_phi, gen_lab):
        ax.text(eta + 0.1, phi + 0.1, particle_key[label])
        if (label != 6) and (label != 12):
            circle = patches.Circle((eta, phi), matching_R, fill=False, linestyle="--", edgecolor="blue")
            ax.add_patch(circle)

    # Kept Matched Jets (Green)
    ax.scatter(kept_matched_jet_eta, kept_matched_jet_phi, s=120, marker="o", c="green", alpha=0.3, label="AK8 jets (kept, matched)")

    for eta, phi in zip(kept_matched_jet_eta, kept_matched_jet_phi):
        circle = patches.Circle((eta, phi), jet_R, fill=False, linestyle="--", edgecolor="green")
        ax.add_patch(circle)

    # Kept Unmatched Jets (Orange)
    ax.scatter(kept_unmatched_jet_eta, kept_unmatched_jet_phi, s=120, marker="o", c="orange", alpha=0.3, label="AK8 jets (kept, unmatched)")

    for eta, phi in zip(kept_unmatched_jet_eta, kept_unmatched_jet_phi):
        circle = patches.Circle((eta, phi), jet_R, fill=False, linestyle="--", edgecolor="orange")
        ax.add_patch(circle)

    # Lost Matched Jets (Red)
    ax.scatter(lost_matched_jet_eta, lost_matched_jet_phi, s=120, marker="o", c="red", alpha=0.3, label="AK8 jets (lost, matched)")

    for eta, phi in zip(lost_matched_jet_eta, lost_matched_jet_phi):
        circle = patches.Circle((eta, phi), jet_R, fill=False, linestyle="--", edgecolor="red")
        ax.add_patch(circle)

    # Lost Unmatched Jets (Gray)
    ax.scatter(lost_unmatched_jet_eta,lost_unmatched_jet_phi, s=120, marker="o", c="gray", alpha=0.3, label="AK8 jets (lost, unmatched)")

    for eta, phi in zip(lost_unmatched_jet_eta, lost_unmatched_jet_phi):
        circle = patches.Circle((eta, phi), jet_R, fill=False, linestyle="--", edgecolor="gray")
        ax.add_patch(circle)

    ax.legend()
    plt.savefig("plots/" + filename + f"_eventDisplay_R_{matching_R}_event_{i}.png")
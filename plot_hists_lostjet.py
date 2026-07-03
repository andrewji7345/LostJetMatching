import os
import ROOT
from pathlib import Path

Path("plots").mkdir(parents=True, exist_ok=True)

# Input ROOT file
#masspoint = "WbWb_4000_1000_"
#masspoint = "WbWb_8000_2000_"
#masspoint = "WbZt_4000_1000_"
#masspoint = "WbZt_8000_2000_"
#masspoint = "Wbht_4000_1000_"
#masspoint = "Wbht_8000_2000_"
#masspoint = "ZtZt_4000_1000_"
#masspoint = "ZtZt_8000_2000_"
#masspoint = "htZt_4000_1000_"
#masspoint = "htZt_8000_2000_"
#masspoint = "htht_4000_1000_"
#masspoint = "htht_8000_2000_"

#masspoint = "WbWb_8000_1000_"
#masspoint = "WbWb_8000_1500_"
#masspoint = "WbWb_8000_2500_"
masspoint = "WbWb_8000_3000_"

filename = "/eos/uscms/store/user/aji/rootfiles_lostJetMatching/" + masspoint + "LostJetMatching.root"

# Output directory for plots
outdir = "plots_lostJetMatching"
os.makedirs(outdir, exist_ok=True)

# Open file
f = ROOT.TFile.Open(filename)

if not f or f.IsZombie():
    raise RuntimeError(f"Could not open {filename}")

ROOT.gROOT.SetBatch(True)

def save_directory(directory, prefix=""):
    """Recursively save all TH1/TH2 objects in a directory."""
    for key in directory.GetListOfKeys():
        obj = key.ReadObj()
        name = obj.GetName()

        if obj.InheritsFrom("TDirectory"):
            save_directory(obj, prefix + name + "_")

        elif obj.InheritsFrom("TH1"):
            canvas = ROOT.TCanvas("c", "c", 800, 600)

            if obj.InheritsFrom("TH2"):
                obj.Draw("COLZ")
            else:
                obj.Draw()

            outfile = os.path.join(outdir, masspoint + prefix + name + ".png")
            canvas.SaveAs(outfile)
            print(f"Saved {outfile}")

            canvas.Close()

save_directory(f)

f.Close()
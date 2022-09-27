import json
import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Specify input probe_config and output Makefile directory.")

    parser.add_argument("-e", type=int, dest='event_per_run', required=True)
    parser.add_argument("-r", type=int, dest='number_of_runs', required=True)
    parser.add_argument("-o", type=str, dest='output_path', required=True)

    args = parser.parse_args()

    # Validate args
    assert(args.event_per_run > 0 and args.number_of_runs > 0)
    
    # Get the relative path to the Makefile
    muonveto_path = os.path.relpath(os.path.dirname(__file__)+'/../build/MuonVeto', args.output_path)

    with open(f"{args.output_path}/Makefile", "w") as makefile:
        makefile.write("all: ")
        for runNb in range(args.number_of_runs):
            makefile.write(f"./run{runNb}/RunConditions.json ")
        makefile.write("\n\n")

        for runNb in range(args.number_of_runs):
            makefile.write(f"./run{runNb}/RunConditions.json: \n")
            makefile.write(f"\t{muonveto_path} -r -e {args.event_per_run} -o . -t 1 -n {runNb}\n\n")

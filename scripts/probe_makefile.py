import json
import argparse
import os

def read_from_json(json_file, key, default_value):
    '''
    Read a certain key from a json file. If the key does not exist, return the default value.
    '''
    if(key not in json_file):
        return default_value
    else:
        return json_file[key]

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Specify input probe_config and output Makefile directory.")

    parser.add_argument("-i", type=str, dest='input_file', required=True)
    parser.add_argument("-o", type=str, dest='output_path', required=True)

    args = parser.parse_args()

    # Validate args
    assert(os.path.isfile(args.input_file))
    assert(os.path.isdir(args.output_path))

    # Read probe_config.json
    with open(args.input_file, "r") as config_file:
        probe_config = json.load(config_file)
    
    # Parse probe_config.json
    GunXMin = read_from_json(probe_config, "GunXMin/cm", 0)
    GunXMax = read_from_json(probe_config, "GunXMax/cm", 0)
    GunXStep = read_from_json(probe_config, "GunXStep/cm", 100)
    GunYMin = read_from_json(probe_config, "GunYMin/cm", 0)
    GunYMax = read_from_json(probe_config, "GunYMax/cm", 0)
    GunYStep = read_from_json(probe_config, "GunYStep/cm", 100)
    GunZMin = read_from_json(probe_config, "GunZMin/cm", 0)
    GunZMax = read_from_json(probe_config, "GunZMax/cm", 0)
    GunZStep = read_from_json(probe_config, "GunZStep/cm", 100)
    NbOfRuns = read_from_json(probe_config, "Number of Runs", 0)

    # Check whether the config file is valid
    assert(GunXMax >= GunXMin and GunYMax >= GunYMin and GunZMax >= GunZMin)
    assert(GunXStep > 0 and GunYStep > 0 and GunZStep > 0)
    assert((GunXMax-GunXMin)%GunXStep < 1e-10 and (GunYMax-GunYMin)%GunYStep < 1e-10 and (GunZMax-GunZMin)%GunZStep < 1e-10)
    assert(NbOfRuns > 0 and isinstance(NbOfRuns, int))

    # Calculate the total number of runs
    run_sum = int(((GunXMax-GunXMin)/GunXStep+1) * ((GunYMax-GunYMin)/GunYStep+1) * ((GunZMax-GunZMin)/GunZStep+1))
    
    # Get the relative path to the Makefile
    muonveto_path = os.path.relpath(os.path.dirname(__file__)+'/../build/MuonVeto', args.output_path)
    probe_config_path = os.path.relpath(args.input_file, args.output_path)

    with open(f"{args.output_path}/Makefile", "w") as makefile:
        makefile.write("all: ")
        for runNb in range(run_sum):
            makefile.write(f"./run{runNb}/RunConditions.json ")
        makefile.write("\n\n")

        for runNb in range(run_sum):
            makefile.write(f"./run{runNb}/RunConditions.json: \n")
            makefile.write(f"\t{muonveto_path} -p {probe_config_path} -o . -t 1 -n {runNb}\n\n")


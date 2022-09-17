import json

def read_from_json(json_file, key, default_value):
    '''
    Read a certain key from a json file. If the key does not exist, return the default value.
    '''
    if(key not in json_file):
        return default_value
    else:
        return json_file[key]

if __name__ == "__main__":
    # Read probe_config.json
    with open("probe_config.json", "r") as config_file:
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
    
    with open("Makefile", "w") as makefile:
        makefile.write("all: ")
        for runNb in range(run_sum):
            makefile.write(f"./run{runNb}/RunConditions.json ")
        makefile.write("\n\n")

        for runNb in range(run_sum):
            makefile.write(f"./run{runNb}/RunConditions.json: \n")
            makefile.write(f"\t../build/MuonVeto -p probe_config.json -o . -t 1 -n {runNb}\n\n")


'''
get_probe: From probe files, use interpolation to create an LCE map for any location inside the pscint.
'''

import json
import os
import numpy as np
import scipy.interpolate
from tqdm import tqdm
from matplotlib import pyplot as plt

def wc_count(path):
    '''
    wc_count: count how many lines there are in a regular csv file
    '''
    with open(path, 'r') as input_file:
        return sum(1 for line in input_file)-1

class get_probe:

    def __init__(self, data_path, data_type, method, **kwds) -> None:
        '''
        This __init__ method should get all useful data from probe_path.
        It checks all data in 'data_path' according to 'data_type', read them, and create interpolators according to 'method'.
        From probe_path, self.points, self.probes, self.interpolators are stored.

        args:
          data_path: path of data
          data_type: must be 'probe' or 'random'
          method   : must be 'grid-linear', 'RBF' or 'inverse_distance' 
        '''

        # Check args
        if(data_type not in ['probe', 'random']): raise ValueError(f'Unexpected data_type "{data_type}"')
        if(method not in ['grid_linear', 'RBF', 'inverse_distance']): raise NotImplementedError(f'Method "{method}" not supported')
        if(data_type == 'random' and method == 'grid_linear'): raise ValueError(f'data_type must be "probe" if method is "grid_linear"')

        print("Initializing get_probe...")

        # Count number of runs
        print('Checking data...')
        number_of_runs = 0
        file_or_dirs = os.listdir(data_path)
        for file_or_dir in file_or_dirs:
            file_or_dir_path = os.path.join(data_path, file_or_dir)
            if os.path.isdir(file_or_dir_path) and file_or_dir[:3] == 'run' and os.path.isfile(file_or_dir_path+"/RunConditions.json"):
                number_of_runs += 1
        if number_of_runs == 0:
            raise ValueError(f'data_path "{data_path}" does not seem valid!')
        
        # Check whether runs are valid
        if data_type == 'probe':
            self.points = []
            self.grid_xs, self.grid_ys, self.grid_zs = [], [], []
            events_per_run = wc_count(f"{data_path}/run0/PEs of SiPM/SiPM_0.csv")
            for runNb in tqdm(range(number_of_runs)):
                with open(f"{data_path}/run{runNb}/RunConditions.json", "r") as run_conditions_file:
                    run_conditions = json.load(run_conditions_file)
                    assert('RandomPoints' not in run_conditions or run_conditions['RandomPoints'] is False), f'{data_path}/run{runNb}/RunConditions.json does not seem to be probe!'
                    assert('GunXPosition/cm' in run_conditions and 'GunYPosition/cm' in run_conditions and 'GunZPosition/cm' in run_conditions), f'{data_path}/run{runNb}/RunConditions.json does not seem to contain positions'
                    self.points.append([run_conditions['GunXPosition/cm'], run_conditions['GunYPosition/cm'], run_conditions['GunZPosition/cm']])
                    if run_conditions['GunXPosition/cm'] not in self.grid_xs:
                        self.grid_xs.append(run_conditions['GunXPosition/cm'])
                    if run_conditions['GunYPosition/cm'] not in self.grid_ys:
                        self.grid_ys.append(run_conditions['GunYPosition/cm'])
                    if run_conditions['GunZPosition/cm'] not in self.grid_zs:
                        self.grid_zs.append(run_conditions['GunZPosition/cm'])
                for SiPMNb in range(2):
                    assert(wc_count(f"{data_path}/run0/PEs of SiPM/SiPM_{SiPMNb}.csv") == events_per_run), f'Invalid data from /run0/PEs of SiPM/SiPM_{SiPMNb}.csv: size does not match'
            points_should_be = np.stack(np.meshgrid(self.grid_xs, self.grid_ys, self.grid_zs, indexing='ij'), axis=-1).reshape(-1,3)
            self.points = np.array(self.points)
            self.grid_xs = np.array(self.grid_xs)
            self.grid_ys = np.array(self.grid_ys)
            self.grid_zs = np.array(self.grid_zs)
            assert((points_should_be == self.points).all())

        if data_type == 'random':
            events_per_run = np.zeros(number_of_runs, dtype=int)
            for runNb in tqdm(range(number_of_runs)):
                with open(f"{data_path}/run{runNb}/RunConditions.json", "r") as run_conditions_file:
                    run_conditions = json.load(run_conditions_file)
                    assert(run_conditions['RandomPoints'] is True), f'{data_path}/run{runNb}/RunConditions.json does not seem to be random!'
                events_per_run[runNb] = wc_count(f"{data_path}/run{runNb}/PEs of SiPM/SiPM_0.csv")
                assert(events_per_run[runNb] == wc_count(f"{data_path}/run{runNb}/PEs of SiPM/SiPM_1.csv"))
                assert(events_per_run[runNb] == wc_count(f"{data_path}/run{runNb}/Gun Position/Gun_Position.csv"))
        print(f'All test passed! Found {number_of_runs} runs in {data_type} mode.')
        print(f'Events per run: {events_per_run}')
        if data_type == 'probe':
            print(f'GunXPosition/cm ranges from {self.grid_xs.min()} to {self.grid_xs.max()}')
            print(f'GunYPosition/cm ranges from {self.grid_ys.min()} to {self.grid_ys.max()}')
            print(f'GunZPosition/cm ranges from {self.grid_zs.min()} to {self.grid_zs.max()}')
        
        # Initialize data
        print('Reading data...')
        if data_type == 'probe':
            self.points = np.zeros((number_of_runs,3))
            self.probes = np.zeros((number_of_runs,2))
        else:
            self.points = np.zeros((np.sum(events_per_run),3))
            self.probes = np.zeros((np.sum(events_per_run),2))

        # From data_path, read all "PEs of SiPM" and "RunConditions.json". If data_type is 'random', "Gun Position" is read instead of "RunConditions.json"
        for runNb in tqdm(range(number_of_runs)):
            if data_type == 'probe':
                with open(f"{data_path}/run{runNb}/RunConditions.json", "r") as run_conditions_file:
                    run_conditions = json.load(run_conditions_file)
                    self.points[runNb] = run_conditions['GunXPosition/cm'], run_conditions['GunYPosition/cm'], run_conditions['GunZPosition/cm']
                for SiPMNb in range(2):
                    with open(f"{data_path}/run{runNb}/PEs of SiPM/SiPM_{SiPMNb}.csv", "r") as PE_file:
                        probe = np.loadtxt(PE_file, dtype=int, comments='#', delimiter=',')
                        assert(probe.shape[0] == events_per_run), f"{data_path}/run{runNb}/PEs of SiPM/SiPM_{SiPMNb}.csv may not be a valid csv file"
                        self.probes[runNb, SiPMNb] = np.average(probe)

            else:
                event_index = np.insert(np.cumsum(events_per_run), 0, 0)
                with open(f"{data_path}/run{runNb}/Gun Position/Gun_Position.csv", "r") as gun_position_file:
                    point = np.loadtxt(gun_position_file, comments='#', delimiter=',')
                    self.points[event_index[runNb]:event_index[runNb+1], :] = point
                for SiPMNb in range(2):
                    with open(f"{data_path}/run{runNb}/PEs of SiPM/SiPM_{SiPMNb}.csv", "r") as PE_file:
                        probe = np.loadtxt(PE_file, dtype=int, comments='#', delimiter=',')
                        self.probes[event_index[runNb]:event_index[runNb+1], SiPMNb] = probe
            
        print(self.probes)
        print(self.points)
        
        # Initialize interpolators
        '''
        if method == 'grid_linear':
            self.interpolators = scipy.interpolate.LinearNDInterpolator(self.points, self.probes[:, 0]), scipy.interpolate.LinearNDInterpolator(self.points, self.probes[:, 1])
        else if method == ''
        '''

    def __call__(self, *args, **kwds):
        '''
        __call__: get probe at (x, y, z)
        
        Parameters: points: 
        '''
        return self.linearNDInterpolator_0(args[0]), self.linearNDInterpolator_1(args[0])


if __name__ == "__main__":
    # unit test
    a = get_probe("../random", 'random', 'inverse_distance')
    b = get_probe("../probe", 'probe', 'inverse_distance')
    x = np.arange(-4.9, 4.9, 0.01)
    y = np.arange(-0.9, 0.9, 0.01)
    z = np.arange(-49, 49, 1)
    '''
    probe_0, probe_1 = a(np.array([0,0,0]))
    xs, ys, zs = np.meshgrid(x, y, z)
    data_points = np.stack([xs.flatten(), ys.flatten(), zs.flatten()], axis=1)
    probe_rbf = a.RBF(data_points)
    print(probe_0, probe_rbf)
    '''
import matplotlib.pyplot as plt
import json

data_file = open('results_length.json', 'r')
data = json.load(data_file)
data_file.close()

print(data)

fgets_data = data['fgets']
mmap_data = data['mmap']
buff_data = data['buffered']
simple_data = data['simple']


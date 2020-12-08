import matplotlib.pyplot as plt
import numpy as np
import json

data_file = open('results_randjump_1.json', 'r')
data = json.load(data_file)
data_file.close()

data = data['randjump']

fgets_data = data['fgets']
mmap_data = data['mmap']
buff_data = data['buffered']
simple_data = data['simple']

# plot fgets data file_size/time
plt.style.use('ggplot')

# sizes = sorted([ int(val[1] / 1e6) for _, val in fgets_data.items() ])
# time =  sorted([ val[0] * 1000 for _, val in fgets_data.items() ])

sizes = []
time = []

for _, val in mmap_data.items():
    sizes.append(int(val['500000'][1] / 1e6))
    time.append(val['500000'][0] * 1000)

sizes = sorted(sizes)[9:]
time = sorted(time)[9:]
y_pos = np.arange(len(sizes))

ax = plt.subplot(111)
ax.set_ylabel('Execution time (ms)')
ax.set_xlabel('File size (MB)')
ax.xaxis.set_tick_params(labelsize=10)
plt.xticks(y_pos, sizes)

plt.bar(y_pos, time)
plt.show()


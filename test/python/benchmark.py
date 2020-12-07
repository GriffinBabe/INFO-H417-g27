import subprocess
import argparse
import resource
import json
import os
import sys
import time

# Parse arguments 
parser = argparse.ArgumentParser(prog='benchmark', description='Benchmarkes the length and randjump program.')
parser.add_argument('executable_folder', type=str, help='Folder where the length and randjump executables are.')
parser.add_argument('database_folder', type=str, help='Folder to database files')
args = parser.parse_args()

executable_folder = args.executable_folder
database_folder = args.database_folder

executable_folder_files = []
database_folder_files = []

results = {}
results['length'] = {}
results['length']['fgets'] = {}
results['length']['simple'] = {}
results['length']['buffered'] = {}
results['length']['mmap'] = {}

# Checks for the executables
try:
	executable_folder_files = os.listdir(executable_folder)
	if 'randjump' not in executable_folder_files or 'length' not in executable_folder_files:
		print('Couldn\' find randjump or length', file=sys.stderr)
		sys.exit(1)
	database_folder_files = os.listdir(database_folder)
except FileNotFoundError:
	print('Wrong directory.', file=sys.stderr)
	sys.exit(1)

# Starts the tests

command = '{}{} {} {} {}{}'

current_command = command
random_reads = 100000 # 100K jumps

# length program fgets stream
for file in database_folder_files:
	# fgets stream
	_command = command.format(executable_folder, 'length', '-f', '', database_folder, file).split(' ')
	_command = list(filter(lambda a: a != '', _command))
	print(_command)
	start = time.time()
	# launches script
	subprocess.run(_command)
	elapsed = time.time() - start
	print(elapsed)
	results['length']['fgets'][file] = elapsed

# length program buffered stream
buffer_sizes = [1000, 4096, 8192, 15000, 30000, 100000, 500000, 1000000]
for file in database_folder_files:
	file_path = database_folder+"/"+file
	size = os.path.getsize(file_path)
	for b_size in buffer_sizes:
		# fgets stream
		_command = command.format(executable_folder, 'length', '-b', b_size, database_folder, file).split(' ')
		_command = list(filter(lambda a: a != '', _command))
		print(_command)
		start = time.time()
		# launches script
		subprocess.run(_command)
		elapsed = time.time() - start
		print(elapsed)
		results['length']['buffered'][file] = {}
		results['length']['buffered'][file][b_size] = elapsed 

"""
for file in database_folder_files:
	for size in buffered:
		_command = command.format(executable_folder, 'length', '-m', size, database_folder, file).split(' ')
		_command = list(filter(lambda a: a != '', _command))
		print(_command)
		start = time.time()
		# launches script
		subprocess.run(_command)
		elapsed = time.time() - start
		print(elapsed)
		results['length']['mmap'][file] = {}
		results['length']['mmap'][file][size] = elapsed 
"""


# saves the result file
a_file = open('results.json', 'w')
json.dump(results, a_file)
a_file.close()

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

results = {}
results['randjump'] = {}
results['randjump']['fgets'] = {}
results['randjump']['simple'] = {}
results['randjump']['buffered'] = {}
results['randjump']['mmap'] = {}
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
        results['length']['fgets'][file] = (elapsed, os.path.getsize(database_folder+'/'+file))

# length program buffered stream
# 4kb, 100kB, 1MB, 100MB, 500MB
buffer_sizes = [4096, 100000, 1000000, 100000000, 500000000]
for file in database_folder_files:
        results['length']['buffered'][file] = {}
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
                results['length']['buffered'][file][b_size] = (elapsed, os.path.getsize(database_folder+'/'+file))

# length program mmap stream
for file in database_folder_files:
        file_path = database_folder+"/"+file
        size = os.path.getsize(file_path)
        results['length']['mmap'][file] = {}
        for b_size in buffer_sizes:
                # fgets stream
                _command = command.format(executable_folder, 'length', '-m', b_size, database_folder, file).split(' ')
                _command = list(filter(lambda a: a != '', _command))
                print(_command)
                start = time.time()
                # launches script
                subprocess.run(_command)
                elapsed = time.time() - start
                print(elapsed)
                results['length']['mmap'][file][b_size] = (elapsed, os.path.getsize(database_folder+'/'+file))
# length program simple stream
for file in database_folder_files:
        # fgets stream
        _command = command.format(executable_folder, 'length', '-s', '', database_folder, file).split(' ')
        _command = list(filter(lambda a: a != '', _command))
        print(_command)
        start = time.time()
        # launches script
        subprocess.run(_command)
        elapsed = time.time() - start
        print(elapsed)
        results['length']['simple'][file] = (elapsed, os.path.getsize(database_folder+'/'+file))
command = '{}{} {} {} {}{} {}'
jumps = 20000 # 20k random jumps will be repeated
# randjump program fgets stream
for file in database_folder_files:
        # fgets stream
        _command = command.format(executable_folder, 'randjump', '-f', '', database_folder, file, jumps).split(' ')
        _command = list(filter(lambda a: a != '', _command))
        print(_command)
        start = time.time()
        # launches script
        subprocess.run(_command)
        elapsed = time.time() - start
        print(elapsed)
        results['randjump']['fgets'][file] = (elapsed, os.path.getsize(database_folder+'/'+file))

# randjump program buffered stream
buffer_sizes = [4096, 100000, 1000000, 100000000, 500000000]
for file in database_folder_files:
        results['randjump']['buffered'][file] = {}
        for b_size in buffer_sizes:
                # fgets stream
                _command = command.format(executable_folder, 'randjump', '-b', b_size, database_folder, file, jumps).split(' ')
                _command = list(filter(lambda a: a != '', _command))
                print(_command)
                start = time.time()
                # launches script
                subprocess.run(_command)
                elapsed = time.time() - start
                print(elapsed)
                results['randjump']['buffered'][file][b_size] = (elapsed, os.path.getsize(database_folder+'/'+file))

# randjump program mmap stream
for file in database_folder_files:
        file_path = database_folder+"/"+file
        size = os.path.getsize(file_path)
        results['randjump']['mmap'][file] = {}
        for b_size in buffer_sizes:
                # fgets stream
                _command = command.format(executable_folder, 'randjump', '-m', b_size, database_folder, file, jumps).split(' ')
                _command = list(filter(lambda a: a != '', _command))
                print(_command)
                start = time.time()
                # launches script
                subprocess.run(_command)
                elapsed = time.time() - start
                print(elapsed)
                results['randjump']['mmap'][file][b_size] = (elapsed, os.path.getsize(database_folder+'/'+file))

# randjump program simple stream
for file in database_folder_files:
        # fgets stream
        _command = command.format(executable_folder, 'randjump', '-s', '', database_folder, file, jumps).split(' ')
        _command = list(filter(lambda a: a != '', _command))
        print(_command)
        start = time.time()
        # launches script
        subprocess.run(_command)
        elapsed = time.time() - start
        print(elapsed)
        results['randjump']['simple'][file] = (elapsed, os.path.getsize(database_folder+'/'+file))

# saves the result file
a_file = open('result_length.json', 'w')
json.dump(results, a_file)
a_file.close()

import os
import subprocess
from subprocess import PIPE
from itertools import product
import json
import numpy as np
from xml.dom import minidom
import sys


result = {}
configs = [('config1.json', 'output1.json'), ('config2.json', 'output2.json'), ('config3.json', 'output3.json'),
           ('config_chain.json', 'output_chain.json')]
mains = [('./mains/main.cpp', configs),
         ('./mains/mainCopy.cpp', [configs[0]]),
         ('./mains/mainMove.cpp', [configs[1]]),
         ('./mains/treeMain.cpp', [(None, 'treeOutput.json')])]


def check_compilation(compile_output, compile_err):
    warnings = len(compile_err)>0
    error = 'recipe for target' in str(compile_output)
    return warnings, error


def clean():
    subprocess.run(['make', 'clean'], cwd='./ass1', stdout=PIPE, stderr=PIPE)
    subprocess.run(['rm', '-f', '*.json'], cwd='./ass1', stdout=PIPE, stderr=PIPE)
    
def compile(should_clean=False, main_name=''):
    if should_clean:
        clean()
    p = subprocess.run(['make'], cwd='./ass1', stdout=PIPE, stderr=PIPE)
    warnings, error = check_compilation(p.stdout, p.stderr)
    result[main_name]['warnings'] = warnings
    result[main_name]['errors'] = error
    return error


def change_main(main_path):
    p = subprocess.run(['cp', main_path, './ass1/src/main.cpp'])


def return_orig_main():
    p = subprocess.run(['rm', '-f', './ass1/src/main.cpp'])
    p = subprocess.run(['cp', './mains/main.cpp', './ass1/src/main.cpp'])


def check_mem():
    mydoc = minidom.parse('./ass1/mem.xml')
    errors = mydoc.getElementsByTagName('error')
    return len(errors)


def copy_config(input):
    p = subprocess.run(['cp', 'configs/{}'.format(input), 'ass1/config.json'], stdout=PIPE, stderr=PIPE)
    if not os.path.isfile('ass1/config.json'):
        raise BaseException("Couldn't find config file {}".format(input))

def get_args(input, valgrind):
    args = ['./bin/cTrace']
    if input is not None:
        copy_config(input)
        args.append('./config.json')
    if valgrind:
        args = ['valgrind', '--leak-check=full', '--show-reachable=yes', '--xml=yes', '--xml-file=mem.xml'] + args
    return args


def run(input, valgrind=False, name=''):
    memory = None
    ans = None
    args = get_args(input, valgrind)
    try:
        p = subprocess.run(args, stdout=PIPE, stderr=PIPE, cwd='./ass1')
        if valgrind:
            memory = check_mem()
        with open('./result/{}_stdout'.format(name), 'w') as output_file:
            output_file.write(str(p.stdout))
        with open('./result/{}_stderr'.format(name), 'w') as output_file:
            output_file.write(str(p.stderr))        
        with open('./ass1/output.json') as file:
            j = json.load(file)
            ans = j
    except KeyboardInterrupt:
        print("interrupting")
    finally:
        if os.path.isfile('./ass1/config.json'):
            p = subprocess.run(['rm', 'config.json'], cwd='./ass1')
        if os.path.isfile('./ass1/output.json'):
            p = subprocess.run(['rm', 'output.json'], cwd='./ass1')
    return ans, memory


def compare_edges(mat1, mat2):
    if mat1.shape != mat2.shape:
        return None
    correct = np.invert(np.logical_xor(mat1, mat2))
    return [(i, j) for i, j in product(*[range(m) for m in mat1.shape]) if not correct[i][j]]


def compare(output, expected_output):
    comparision = {'missing_keys':[]}
    for key in expected_output.keys():
        if key not in output:
            comparision['missing_keys'].append(key)
            continue
        if key == 'infected':
            comparision['infected_diff'] = list(set(output[key]).symmetric_difference(set(expected_output[key])))
            continue
        if key == 'graph':
            mat1, mat2 = np.array(output[key]), np.array(expected_output[key])
            comparision['wrong_edges'] = compare_edges(mat1, mat2)
            continue
        comparision[key] = output[key]==expected_output[key]
    return comparision


def test(test_num, input, expected_output, main='', valgrind=False):
    full_test_name = '{}{}v={}'.format(main[8:], test_num, valgrind) #main[8:] main without directory
    test_name = '{}{}'.format(test_num, 'valgrind' if valgrind else '')
    output, memory = run(input, valgrind=valgrind, name=full_test_name)
    if output is None:
        result[main][test_name]='NO OUTPUT'
    else:
        with open('configs/{}'.format(expected_output)) as file:
            expected_json = json.load(file)
            result[main][test_name] = compare(output, expected_json)
    if valgrind:
        result[main]['{}_mem'.format(test_num)] = memory
        if memory is not None:
            p = subprocess.run(['mv', './ass1/mem.xml', './result/{}.xml'.format(full_test_name)])
        

def print_report(result):
    
    def print_test_result(main_dict, key):
        if '{}'.format(key) not in main_dict:
            return
        print('Result without valgrind:', main_dict['{}'.format(key)])
        print('Result with valgrind:', main_dict['{}valgrind'.format(key)])
        print('Memory leaks/errors:{}'.format(main_dict['{}_mem'.format(key)]))
    
    def print_main_result(main):
        print('TEST {}'.format(main['name']))
        print('Compilation errors: {}'.format(main['errors']))
        print('Compilation warnings: {}'.format(main['warnings']))
        for i in range(main['num_tests']):
            print_test_result(main, i)
    
    for key in result.keys():
        print_main_result(result[key])


if __name__ == '__main__':
    for main, configs in mains:
        try:
            result[main] = {'name': main, 'num_tests': len(configs)}
            change_main(main)
            print("Compiling with {}".format(main))
            error = compile(should_clean=True, main_name=main)
            if error:
                sys.stderr.write("Couldn't compile {}\n".format(main))
                continue
            for ((test_ind, (input, output)), valgrind) in product(enumerate(configs), [False, True]):
                print("Running test {} {}".format(test_ind, "" if not valgrind else "with valgrind"))
                test(test_ind, input, output, main=main, valgrind=valgrind)
        except BaseException as e:
            print(e)
        finally:
            clean()
            return_orig_main()


    with open('./result/result.json', 'w') as output_file:
        json.dump(result, output_file)
        
    print_report(result)

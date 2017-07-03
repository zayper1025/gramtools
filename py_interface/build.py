import os
import time
import logging
import argparse
import subprocess

from . import utils


log = logging.getLogger('gramtools')


def get_project_dirpath(prg_fpath):
    prg_fname = os.path.basename(prg_fpath)
    if prg_fname.endswith('.prg'):
        project_dir = prg_fname[:-len('.prg')]
    else:
        project_dir = prg_fname
    project_dir = project_dir.split('.')[0]

    current_working_directory = os.getcwd()
    project_dirpath = os.path.join(current_working_directory, project_dir)
    return project_dirpath


def get_paths(args):
    project_dirpath = get_project_dirpath(args.vcf)

    paths = {
        'project': project_dirpath,
        'prg': os.path.join(project_dirpath, 'prg'),
        'vcf': os.path.abspath(args.vcf),
        'sites_mask': os.path.join(project_dirpath, 'sites_mask'),
        'allele_mask': os.path.join(project_dirpath, 'allele_mask'),
        'reference': args.reference,

        'kmer': os.path.join(project_dirpath, 'kmer'),
        'kmer_file': os.path.join(project_dirpath, 'kmer',
                                  'ksize_' + str(args.ksize)),
        'cache': os.path.join(project_dirpath, 'cache'),
        'int_encoded_prg': os.path.join(
            project_dirpath, 'cache', 'int_encoded_prg'),
        'fm_index': os.path.join(project_dirpath, 'cache', 'fm_index'),
        'kmer_suffix_array': os.path.join(project_dirpath, 'cache',
                                          'kmer_suffix_array'),

        'perl_generated_fa': os.path.join(project_dirpath,
                                          'cache',
                                          'perl_generated_fa'),
    }
    return paths


def setup_file_structure(paths):
    dirs = [
        paths['project'],
        paths['cache'],
        paths['kmer'],
        paths['kmer_suffix_array'],
    ]
    for dirpath in dirs:
        if os.path.isdir(dirpath):
            continue
        log.debug('Creating directory: %s', dirpath)
        os.mkdir(dirpath)


def execute_command_generate_prg(paths, args):
    command = [
        'perl', utils.prg_build_exec_fpath,
        '--outfile', paths['prg'],
        '--vcf', paths['vcf'],
        '--ref', paths['reference'],
    ]

    log.debug('Executing command:\n\n%s\n', ' '.join(command))
    timer_start = time.time()

    current_working_directory = os.getcwd()
    process_handle = subprocess.Popen(command,
                                      cwd=current_working_directory,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.PIPE)
    utils.handle_process_result(process_handle)
    timer_end = time.time()
    log.debug('Finished executing command: %s seconds', timer_end - timer_start)


def execute_command_generate_kmers(paths, args):
    command = [
        'python2.7', utils.kmers_script_fpath,
        '-f', paths['perl_generated_fa'],
        '-k', str(args.ksize),
    ]

    log.debug('Executing command:\n\n%s\n', ' '.join(command))
    timer_start = time.time()

    current_working_directory = os.getcwd()
    with open(paths['kmer_file'], 'wb') as kmers_fhandle:
        process_handle = subprocess.Popen(command,
                                          cwd=current_working_directory,
                                          stdout=kmers_fhandle,
                                          stderr=subprocess.PIPE)
    utils.handle_process_result(process_handle)
    timer_end = time.time()
    log.debug('Finished executing command: %s seconds', timer_end - timer_start)


def file_cleanup_generate_prg(paths):
    original_fpath = paths['prg'] + '.mask_alleles'
    target_fpath = os.path.join(paths['project'], 'allele_mask')
    os.rename(original_fpath, target_fpath)

    original_fpath = paths['prg'] + '.mask_sites'
    target_fpath = os.path.join(paths['project'], 'sites_mask')
    os.rename(original_fpath, target_fpath)

    original_fpath = paths['prg']
    target_fpath = os.path.join(paths['project'], 'prg')
    os.rename(original_fpath, target_fpath)

    # TODO: should .prg.vcf be generated at all?
    original_fpath = paths['prg'] + '.vcf'
    target_fpath = os.path.join(paths['project'], 'cache',
                                'perl_generated_vcf')
    os.rename(original_fpath, target_fpath)

    original_fpath = paths['prg'] + '.fa'
    target_fpath = os.path.join(paths['project'], 'cache',
                                'perl_generated_fa')
    os.rename(original_fpath, target_fpath)


def run(args):
    log.info('Start process: build')

    paths = get_paths(args)
    setup_file_structure(paths)

    execute_command_generate_prg(paths, args)
    file_cleanup_generate_prg(paths)

    execute_command_generate_kmers(paths, args)

    log.info('End process: build')
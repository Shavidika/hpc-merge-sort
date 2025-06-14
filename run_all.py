import subprocess
import sys
import os
import platform

def is_windows():
    return platform.system().lower().startswith('win')

def exe_prefix():
    return '' if is_windows() else './'

def compile_generate_dataset():
    print("Compiling generate_dataset.cpp...")
    result = subprocess.run(["g++", "src/generate_dataset.cpp", "-o", "generate_dataset.exe"], shell=True)
    if result.returncode != 0:
        print("Failed to compile generate_dataset.cpp.")
        sys.exit(1)

def compile_merge_sort_serial():
    print("Compiling merge_sort_serial.cpp...")
    result = subprocess.run(["g++", "src/merge_sort_serial.cpp", "-o", "merge_sort_serial.exe"], shell=True)
    if result.returncode != 0:
        print("Failed to compile merge_sort_serial.cpp.")
        sys.exit(1)

def run_generate_dataset():
    print("Running dataset generator...")
    exe = exe_prefix() + "generate_dataset.exe"
    result = subprocess.run([exe], shell=True)
    if result.returncode != 0:
        print("Failed to generate dataset.")
        sys.exit(1)

def run_merge_sort_serial():
    print("Running serial merge sort...")
    exe = exe_prefix() + "merge_sort_serial.exe"
    result = subprocess.run([exe], shell=True)
    if result.returncode != 0:
        print("Serial merge sort failed.")
        sys.exit(1)

def compile_and_run_mpi_in_wsl():
    print("Switching to Ubuntu (WSL), compiling, and running MPI parallel merge sort...")
    wsl_command = (
        'ubuntu run "cd /mnt/c/Users/Shavidika/Desktop/HPC/Project/hpc-merge-sort && '
        'mpic++ -o merge_sort_parallel_MPI src/merge_sort_parallel_MPI.cpp && '
        'mpiexec -n 4 ./merge_sort_parallel_MPI"'
    )
    result = subprocess.run(wsl_command, shell=True)
    if result.returncode != 0:
        print("MPI parallel merge sort failed.")
        sys.exit(1)

def main():
    regenerate = input("Do you want to regenerate the dataset? (yes/no): ").strip().lower()
    compile_generate_dataset()
    if regenerate == "yes":
        run_generate_dataset()
    compile_merge_sort_serial()
    run_merge_sort_serial()
    compile_and_run_mpi_in_wsl()

if __name__ == "__main__":
    main()

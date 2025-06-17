import subprocess
import sys
import os
import platform

def is_windows():
    return platform.system().lower().startswith('win')

def exe_prefix():
    return '' if is_windows() else './'

def print_section(title):
    print("\n" + "=" * 60)
    print(f"{title}")
    print("=" * 60)

def compile_generate_dataset():
    print_section("[1] Compiling Dataset Generator")
    result = subprocess.run(["g++", "src/generate_dataset.cpp", "-o", "generate_dataset.exe"], shell=True)
    if result.returncode != 0:
        print("Failed to compile generate_dataset.cpp.")
        sys.exit(1)

def compile_merge_sort_serial():
    print_section("[2] Compiling Serial Merge Sort")
    result = subprocess.run(["g++", "src/merge_sort_serial.cpp", "-o", "merge_sort_serial.exe"], shell=True)
    if result.returncode != 0:
        print("Failed to compile merge_sort_serial.cpp.")
        sys.exit(1)

def compile_merge_sort_openmp():
    print_section("[3] Compiling Parallel Merge Sort (OpenMP)")
    result = subprocess.run(["g++", "-fopenmp", "src/merge_sort_parallel_openMP.cpp", "-o", "merge_sort_parallel_openMP.exe"], shell=True)
    if result.returncode != 0:
        print("Failed to compile merge_sort_parallel_openMP.cpp.")
        sys.exit(1)

def run_generate_dataset():
    print_section("[1] Dataset Generation")
    exe = exe_prefix() + "generate_dataset.exe"
    result = subprocess.run([exe], shell=True)
    if result.returncode != 0:
        print("Failed to generate dataset.")
        sys.exit(1)

def run_merge_sort_serial():
    print_section("[2] Serial Merge Sort")
    exe = exe_prefix() + "merge_sort_serial.exe"
    result = subprocess.run([exe], shell=True)
    if result.returncode != 0:
        print("Serial merge sort failed.")
        sys.exit(1)

def run_merge_sort_openmp():
    print_section("[4] Parallel Merge Sort (OpenMP)")
    exe = exe_prefix() + "merge_sort_parallel_openMP.exe"
    result = subprocess.run([exe], shell=True)
    if result.returncode != 0:
        print("OpenMP parallel merge sort failed.")
        sys.exit(1)

def compile_and_run_mpi_in_wsl():
    print_section("[3] Parallel Merge Sort with MPI (WSL)")
    print("Switching to Ubuntu (WSL) and running MPI parallel merge sort...")
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
    print("AUTOMATED MERGE SORT BENCHMARK\n" + "-" * 60)
    regenerate = input("Do you want to regenerate the dataset? (yes/no): ").strip().lower()
    compile_generate_dataset()
    if regenerate == "yes":
        run_generate_dataset()
    else:
        print("[1] Skipping dataset generation (using existing files).\n")
    compile_merge_sort_serial()
    run_merge_sort_serial()
    compile_merge_sort_openmp()
    run_merge_sort_openmp()
    compile_and_run_mpi_in_wsl()
    print("\n" + "=" * 60)
    print("All steps completed. See above for timing and accuracy results.")
    print("=" * 60)

if __name__ == "__main__":
    main()

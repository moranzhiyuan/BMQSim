# BMQSim

BMQSim is a state-vector quantum circuit simulator based on SV-Sim. The paper artifact calls it BML-QSim. The `dev_compqsim` branch adapts that artifact for the CompQSim CUDA and HIP benchmark. [Upstream artifact](https://github.com/paperuse62/code/tree/530d77f90f1f367dcbc11480bbb2db2008de26a3)

## HIP and CUDA with PFPL

The fork provides the simulator headers, the OpenQASM 2 converter, and example circuits. The CompQSim checkout provides the CMake target, the original PFPL FP64 ABS adapter, and the validation scripts in `benchmark/baseline/bmqsim`.

From the root of a CompQSim checkout that includes the benchmark, initialize the pinned BMQSim submodule.

```bash
git submodule update --init benchmark/baseline/bmqsim/source/bmqsim
cd benchmark/baseline/bmqsim
```

Follow the README in that directory to set up PFPL and build the benchmark. The HIP build also needs a checkout of the HIP implementation of original PFPL FP64 ABS. Use the revisions listed in the CompQSim README and point `BMQSIM_PFPL_DIR` and `BMQSIM_HIPPFPL_DIR` at those checkouts.

The following is the build used on a `gfx936` DCU with DTK 26.04. It assumes the pinned PFPL sources are available at `source/pfpl` and `source/hippfpl`. The default `compqsim_bmqsim` executable runs the bundled 24-qubit GHZ circuit with two visible devices.

```bash
dtk=/opt/dtk-26.04
source "$dtk/env.sh"
cmake -S . -B build/hip -G Ninja \
  -DBMQSIM_BACKEND=HIP -DCMAKE_HIP_ARCHITECTURES=gfx936 \
  -DCMAKE_HIP_COMPILER="$dtk/llvm/bin/clang++" \
  -DCMAKE_HIP_COMPILER_ROCM_ROOT="$dtk" \
  -DBMQSIM_CIRCUIT="$PWD/source/bmqsim/cu_files/ghz_n24.cu" \
  -DBMQSIM_PRINT_PARTITIONS=OFF
cmake --build build/hip --parallel
HIP_VISIBLE_DEVICES=0,1 OMP_NUM_THREADS=2 build/hip/compqsim_bmqsim
```

The simulator uses the upstream gate formulas, cooperative kernel, grid synchronization between gates, and two streams per GPU. In the PFPL benchmark, compressed chunks stay in pinned host memory. PFPL encodes the original log-domain values using FP64 ABS with the default absolute bound `2^-11`. Signs and zeros are stored separately. The CompQSim build handles the DTK 26.04 grid synchronization header issue locally.

## Upstream CUDA and Bitcomp workflow

This workflow uses the fork directly. It requires `nvcc`, a compatible host C++ compiler with OpenMP, Python 3, an nvCOMP installation that provides `native/bitcomp.h` and `libnvcomp_bitcomp`, and a working cuFile runtime. The simulator opens cuFile at startup in this mode.

```bash
git clone --branch dev_compqsim https://github.com/moranzhiyuan/BMQSim.git
cd BMQSim
mkdir -p build
python3 qasm_to_c.py -i qasm_files/ghz_n24.qasm -o build/ghz_n24.cu

nvcomp_root=/path/to/nvcomp
nvcomp_lib="$nvcomp_root/lib"
nvcc -O3 -arch=sm_80 -m64 -std=c++14 -rdc=true \
  -Xcompiler -fPIC -Xcompiler -fopenmp \
  -I . -I "$nvcomp_root/include" -L "$nvcomp_lib" \
  build/ghz_n24.cu -lcuda -lcufile -lnvcomp_bitcomp -lm -o build/ghz_n24

CUDA_VISIBLE_DEVICES=0,1 \
  LD_LIBRARY_PATH="$nvcomp_lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" build/ghz_n24
```

Set `nvcomp_lib` to the actual library directory and choose the CUDA architecture for your GPUs. The converter sets the GPU count in the generated source, with two GPUs by default. For runs that spill to SSD, configure `ssdPath` in `bmlqsim.cuh` as a writable directory ending in `/`.

## Timing and validation

`Overall end to end time` measures the call to `circuitPartition()` in `beginSimulation()`. It includes data movement, gate execution, and compression for the stages. Initial compression, device buffer allocation, and validation state export run outside this interval.

The validation runs used original PFPL FP64 ABS on CUDA 12.0 (`sm_89`) and DTK 26.04 (`gfx936`) with host-resident compressed chunks.

| Circuit | Configuration | Result |
|---|---|---|
| 12 qubits, 11 gates | CUDA single GPU and HIP single GPU | Matching stage and compressed chunk records. FP64 state vectors matched bit for bit. |
| 24-qubit GHZ | CUDA single GPU and HIP single GPU | Two matching stages. FP64 state vectors matched bit for bit. |
| 12 qubits, 11 gates | HIP two GPUs | The FP64 state matched the single-GPU reference bit for bit. |
| 24-qubit GHZ default entry point | HIP two GPUs | Both stages completed. The default entry point reports timings. |

CUDA two-GPU runs, the original Bitcomp workflow, and SSD spill were outside this validation scope. The HIP benchmark reports an error if a run requires SSD spill.

## Acknowledgements

BML-QSim builds on the PNNL SV-Sim simulator. [SV-Sim repository](https://github.com/pnnl/SV-Sim)

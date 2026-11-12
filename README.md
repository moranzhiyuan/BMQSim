# BMQSim

BMQSim, called BML-QSim in the paper artifact, is an FP64 quantum circuit simulator based on SV-Sim. The `dev_compqsim` branch adds HIP and PFPL support for use in CompQSim. [Upstream artifact](https://github.com/paperuse62/code/tree/530d77f90f1f367dcbc11480bbb2db2008de26a3) · [SV-Sim](https://github.com/pnnl/SV-Sim)

## CompQSim integration

Build the PFPL version from `benchmark/baseline/bmqsim` in CompQSim. That directory provides the CMake configuration and original PFPL FP64 ABS adapter for CUDA and HIP. The HIP build keeps compressed chunks in host memory.

## CUDA with Bitcomp

The upstream CUDA workflow requires nvcc, OpenMP, Python 3, nvCOMP Bitcomp, and cuFile. Set the nvCOMP paths and GPU architecture in `compile.py` before building. For SSD spill, set `ssdPath` in `bmlqsim.cuh` to a writable directory ending in `/`.

```bash
mkdir -p build
python3 qasm_to_c.py -i /path/to/circuit.qasm -o build/circuit.cu
python3 compile.py build/circuit.cu build/circuit
./build/circuit
```

The generated entry point uses two GPUs by default.

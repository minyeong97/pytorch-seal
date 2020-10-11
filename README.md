# pytorch-seal
#### Disclaimer: Pytorch-seal is named pytorch-seal only because it has a similar interface with pytorch. It does not use any pytorch code.

## How use pytorch-seal

1. You need Microsoft SEAL[!github.com/microsoft/seal.git] library installed
2. GCC > 6.0 required

clone the project
```
git clone https://github.com/vimsiin/pytorch-seal.git
```

then you have to install pytorchsealcpp module by first going to the main directory. If `git clone` made the directory `pytorch-seal`, then
```
cd pytorch-seal
```
change the library directory in `ext_modules` of `setup.py` to your path to SEAL library.
```
ext_modules = [
    Extension(
        'torchsealcpp',
        # Sort input source files to ensure bit-for-bit reproducible builds
        # (https://github.com/pybind/python_example/pull/53)
        sorted(['src/torchseal.cpp']),
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            'include',
        ],
        language='c++',
        library_dirs=['/path/to/seal/library'], # CHANGE!!!
        libraries=['seal']
    ),
]
```

install the module by
```
pip install .
```
then run the test by
```
python test.py
```

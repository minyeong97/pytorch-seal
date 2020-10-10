# pytorch-seal
#### Disclaimer! Pytorch-seal is named pytorch-seal only because it has a similar interface with pytorch. It does not use any pytorch code.

## How use pytorch-seal

clone the project
```
git clone https://github.com/vimsiin/pytorch-seal.git
```
you need to add the lib directory to the `LD_LIBRARY_PATH`
```
export LD_LIBRARY_PATH=/absolute/directory/to/pytorch-seal/lib
```
then you have to install pytorchsealcpp module by first going to the main directory. If `git clone` made the directory `pytorch-seal`, then
```
cd pytorch-seal
```
install the module by
```
pip install .
```
then run the test by
```
python test.py
```

import torchseal as ts
import torchsealcpp as t

## the original CryptoNets have 5 layers
## CONV-SQUARE-POOL(scaled mean pooling)-SQUARE-OUPUT

## the reported time in the original paper
## Encoding+Encryption: 122 seconds
## CNN inference: 570 seconds
## Decryption+Decoding: 5 seconds
class CryptoNets(ts.Module):
  def __init__(self):
    super(CryptoNets, self).__init__()
    self.conv_layer = ts.Sequential(
      ts.Conv2d(1, 5, 5, 2),  ## (29-5)/2+1 = 13, 13*13*5=845
      ts.Square()
    ) 
    self.fc_layer = ts.Sequential(
      ts.Linear(845, 100),
      ts.Square(),
      ts.Linear(100, 10)
    )
  def forward(self, x):
    r = self.conv_layer(x)
    f = ts.flatten(r)
    return self.fc_layer(f)

if __name__ == '__main__':
  cnn = CryptoNets()
  x = t.Tensor3(1, 29, 29) ## no padding but assume that the image size is 29x29 (not 28x28) 
  t.fill3(x)
  cnn(x)

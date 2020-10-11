import torchseal as ts
import torchsealcpp as t

class CNN(ts.Module):
    def __init__(self):
        super(CNN, self).__init__()
        self.conv_layer = ts.Sequential(
                ts.Conv2d(1, 1, 5, 1),
                ts.Square()
            )
        self.fc_layer = ts.Sequential(
                ts.Linear(9, 2),
            )

    def forward(self, x):
        r = self.conv_layer(x)
        f = ts.flatten(r)
        return self.fc_layer(f)

if __name__=='__main__':
   cnn = CNN()
   x = t.Tensor3(1, 5, 5)
   t.fill3(x)
   cnn(x)
